/*
   Copyright (C) 2009 Andreas Franz Borchert
   ----------------------------------------------------------------------------
   The Astl Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version
   2 of the License, or (at your option) any later version.

   The Astl Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <cassert>
#include <cstdlib>
#include <memory>
#include <astl/rules.hpp>
#include <astl/attribute.hpp>
#include <astl/state-machine.hpp>
#include <astl/exception.hpp>
#include <astl/expression.hpp>
#include <astl/execution.hpp>
#include <astl/operators.hpp>

namespace Astl {

// ========== StateMachine =================================================

StateMachine::StateMachine(BindingsPtr bindings,
      const std::string& name, unsigned int id) :
   abstract(false), global(true),
   name(name), bindings(bindings), id(id) {
}

StateMachine::StateMachine(BindingsPtr bindings,
      const std::string& name) :
   abstract(true), global(true),
   name(name), bindings(bindings) {
}

void StateMachine::import_asm(StateMachinePtr sm) {
   // copy everything but bindings, name, id, and states
   global = sm->global;
   vars = sm->vars;
   shared_var_list = sm->shared_var_list;
   private_var_list = sm->private_var_list;
   rules = sm->rules;
   creating_rules = sm->creating_rules;
   close_handlers = sm->close_handlers;
   local_functions = sm->local_functions;
}

void StateMachine::add_state(const std::string& state, const Location& loc) {
   assert(!abstract);
   int nextid = stateByName.size();
   std::pair<std::map<std::string, int>::iterator, bool> result =
      stateByName.insert(make_pair(state, nextid));
   if (!result.second) {
      throw Exception(loc, "state " + state + " has been multiply declared");
   }
   stateByNumber[nextid] = state;
}

void StateMachine::add_rule(StateMachineRulePtr rule) {
   assert(rule);
   if (rule->non_creating()) {
      rules.push_back(rule);
   }
   if (rule->creating()) {
      creating_rules.push_back(rule);
      global = false;
   }
}

void StateMachine::add_var(VarKind varkind, const std::string& varname,
      const Location& loc) {
   add_var(varkind, varname, NodePtr(nullptr), loc);
}

void StateMachine::add_close_handler(const StateSet& states, NodePtr block) {
   assert(block);
   Handler handler = {states, block};
   close_handlers.push_back(handler);
}

void StateMachine::add_local_function(const std::string& fname,
      NodePtr block) throw(Exception) {
   assert(block);
   std::map<std::string, NodePtr>::const_iterator vit = vars.find(fname);
   if (vit != vars.end()) {
      throw Exception(block->get_location(), "state machine function " +
	 fname + " is in conflict with an equally named variable");
   }
   std::pair<FunctionTable::iterator, bool> result =
      local_functions.insert(std::make_pair(fname, block));
   if (!result.second) {
      throw Exception(block->get_location(), "state machine function " +
	 fname + " has been multiply declared");
   }
}

void StateMachine::add_var(VarKind varkind, const std::string& varname,
      NodePtr init_expr, const Location& loc) {
   FunctionTable::const_iterator fit = local_functions.find(varname);
   if (fit != local_functions.end()) {
      throw Exception(loc, "state machine variable " +
	 varname + " is in conflict with an equally named function");
   }
   Variable var = {varkind, varname, init_expr};
   std::pair<std::map<std::string, NodePtr>::iterator, bool> result =
      vars.insert(make_pair(varname, init_expr));
   if (!result.second) {
      throw Exception(loc, "state machine variable " +
	 varname + " has been multiply declared");
   }
   switch (varkind) {
      case privateVar:
	 private_var_list.push_back(var);
	 break;
      case sharedVar:
	 shared_var_list.push_back(var);
	 break;
      default:
	 assert(false); std::abort();
   }
}

BindingsPtr StateMachine::get_shared_bindings() const {
   BindingsPtr b = std::make_shared<Bindings>(bindings);
   for (std::list<Variable>::const_iterator it =
	 shared_var_list.begin(); it != shared_var_list.end(); ++it) {
      AttributePtr at;
      if (it->init_expr) {
	 Expression expr(it->init_expr, b);
	 at = expr.get_result();
      }
      bool ok = b->define(it->name, at); assert(ok);
   }
   return std::make_shared<Bindings>(b);
}

BindingsPtr StateMachine::add_private_bindings(
      BindingsPtr shared_bindings) const {
   BindingsPtr b = std::make_shared<Bindings>(shared_bindings);
   for (std::list<Variable>::const_iterator it =
	 private_var_list.begin(); it != private_var_list.end(); ++it) {
      AttributePtr at;
      if (it->init_expr) {
	 Expression expr(it->init_expr, b);
	 at = expr.get_result();
      }
      bool ok = b->define(it->name, at); assert(ok);
   }
   update_function_bindings(b);
   return b;
}

void StateMachine::update_function_bindings(BindingsPtr local_bindings) const {
   for (FunctionTable::const_iterator it =
	 local_functions.begin(); it != local_functions.end(); ++it) {
      FunctionPtr f = std::make_shared<RegularFunction>(it->second,
	 local_bindings);
      AttributePtr fat = std::make_shared<Attribute>(f);
      if (!local_bindings->define(it->first, fat)) {
	 // update previous binding, if any
	 local_bindings->update(it->first, fat);
      }
   }
}

bool StateMachine::is_abstract() const {
   return abstract;
}

unsigned int StateMachine::get_id() const {
   assert(!abstract);
   return id;
}

std::string StateMachine::get_name() const {
   return name;
}

unsigned int StateMachine::get_nofstates() const {
   assert(!abstract);
   return stateByName.size();
}

unsigned int StateMachine::get_nofxstates() const {
   assert(!abstract);
   return stateByName.size() * stateByName.size();
}

const std::string& StateMachine::get_state_by_number(int index) const {
   assert(!abstract && index >= 0 && index < (int)stateByName.size());
   std::map<int, std::string>::const_iterator it = stateByNumber.find(index);
   assert(it != stateByNumber.end());
   return it->second;
}

int StateMachine::get_state_by_name(const std::string& name_param) const {
   assert(!abstract);
   std::map<std::string, int>::const_iterator it = stateByName.find(name_param);
   if (it == stateByName.end()) {
      return -1;
   } else {
      return it->second;
   }
}

bool StateMachine::is_global() const {
   return global;
}

StateMachine::Iterator StateMachine::get_rules_begin() const {
   return rules.begin();
}

StateMachine::Iterator StateMachine::get_rules_end() const {
   return rules.end();
}

StateMachine::Iterator StateMachine::get_creating_rules_begin() const {
   return creating_rules.begin();
}

StateMachine::Iterator StateMachine::get_creating_rules_end() const {
   return creating_rules.end();
}

void StateMachine::run_close_handlers(int state,
	 BindingsPtr local_bindings) const throw(Exception) {
   assert(state >= 0);
   local_bindings->define("current_state",
	 std::make_shared<Attribute>(get_state_by_number(state)));
   for (std::list<Handler>::const_iterator it = close_handlers.begin();
	 it != close_handlers.end(); ++it) {
      if (state >= (int)it->states.size() || it->states.test(state)) {
	 execute(it->block, local_bindings);
      }
   }
}

// ========== StateMachineRule =============================================
StateMachineRule::StateMachineRule(StateMachinePtr sm_param,
	 NodePtr tree_expr_param, const Rules& rules_param) :
      BasicRule(tree_expr_param, rules_param),
      sm(sm_param), creating_rule(false) {
   assert(sm);
}

StateMachineRule::StateMachineRule(StateMachinePtr sm_param,
	 const NodeTypeSet& nodetypes_param, const Rules& rules_param) :
      BasicRule(rules_param),
      sm(sm_param), nodetypes(nodetypes_param), creating_rule(false) {
   assert(sm);
}

StateMachineRule::StateMachineRule(StateMachinePtr sm_param,
	 NodePtr tree_expr_param, const NodeTypeSet& nodetypes_param,
	 const Rules& rules_param) :
      BasicRule(tree_expr_param, rules_param), sm(sm_param),
      nodetypes(nodetypes_param), creating_rule(false) {
   assert(sm);
}

void StateMachineRule::add_node_condition(NodePtr node_expr_param) {
   assert(!node_expr && node_expr_param);
   node_expr = node_expr_param;
}

void StateMachineRule::add_alternative(
      StateMachineRuleAlternativePtr alternative) {
   assert(alternative);
   if (alternative->action == StateMachineRuleAlternative::create) {
      assert(alternatives.size() == 0 && !creating_rule);
      creating_rule = true;
      block = alternative->get_block();
   } else {
      assert(!creating_rule);
      alternatives.push_back(alternative);
   }
}

StateMachinePtr StateMachineRule::get_sm() const {
   return sm;
}

bool StateMachineRule::tree_expr_defined() const {
   return (bool) tree_expr;
}

const NodeTypeSet& StateMachineRule::get_nodetypes() const {
   return nodetypes;
}

NodePtr StateMachineRule::get_node_condition() const {
   return node_expr;
}

StateMachineRule::AlternativesIterator
StateMachineRule::get_alternatives_begin() const {
   return alternatives.begin();
}

StateMachineRule::AlternativesIterator
StateMachineRule::get_alternatives_end() const {
   return alternatives.end();
}

bool StateMachineRule::creating() const {
   return creating_rule;
}

NodePtr StateMachineRule::get_block() const {
   return block;
}

bool StateMachineRule::non_creating() const {
   return !creating_rule;
}

// ========== StateMachineRuleAlternative ==================================

StateMachineRuleAlternative::StateMachineRuleAlternative(NodePtr block) :
      newstate(-1), action(null), block(block) {
   assert(block);
}

StateMachineRuleAlternative::StateMachineRuleAlternative(Action action) :
      newstate(-1), action(action) {
}

StateMachineRuleAlternative::StateMachineRuleAlternative(int newstate) :
      newstate(newstate), action(null) {
   assert(newstate >= 0);
}

StateMachineRuleAlternative::StateMachineRuleAlternative(Action action,
	 NodePtr block) :
      newstate(-1), action(action), block(block) {
   assert(block);
}

StateMachineRuleAlternative::StateMachineRuleAlternative(int newstate,
	 NodePtr block) :
      newstate(newstate), action(null), block(block) {
   assert(block && newstate >= 0);
}

void StateMachineRuleAlternative::set_labels(const LabelSet& labels_param) {
   labels = labels_param;
}

void StateMachineRuleAlternative::set_states(const StateSet& states_param) {
   states = states_param;
}

void StateMachineRuleAlternative::set_return_node(NodePtr node) {
   assert(action == cache);
   return_node = node;
}

// accessors
const LabelSet& StateMachineRuleAlternative::get_labels() const {
   return labels;
}

const StateSet& StateMachineRuleAlternative::get_states() const {
   return states;
}

StateMachineRuleAlternative::Action
StateMachineRuleAlternative::get_action() const {
   return action;
}

NodePtr StateMachineRuleAlternative::get_return_node() const {
   assert(action == cache);
   return return_node;
}

NodePtr StateMachineRuleAlternative::get_block() const {
   return block;
}

int StateMachineRuleAlternative::get_newstate() const {
   return newstate;
}

// ========== StateMachineTable ============================================

void StateMachineTable::add(StateMachinePtr sm) {
   if (sm->is_global()) {
      global.push_back(sm);
   } else {
      local.push_back(sm);
   }
}

void StateMachineTable::clear() {
   global.clear(); local.clear();
}

unsigned int StateMachineTable::nof_state_machines() const {
   return global.size() + local.size();
}

StateMachineTable::Iterator StateMachineTable::get_global_begin() const {
   return global.begin();
}

StateMachineTable::Iterator StateMachineTable::get_global_end() const {
   return global.end();
}

StateMachineTable::Iterator StateMachineTable::get_local_begin() const {
   return local.begin();
}

StateMachineTable::Iterator StateMachineTable::get_local_end() const {
   return local.end();
}

// ========== construction =================================================

static StateMachineRuleAlternativePtr create_alternative(StateMachinePtr sm,
      NodePtr smblock) {
   assert(smblock->get_op() == Op::sm_block);
   StateMachineRuleAlternative::Action action =
      StateMachineRuleAlternative::null;
   int newstate = -1;
   unsigned int nextop = 0;
   NodePtr return_node;
   if (smblock->get_operand(0)->get_op() == Op::sm_action) {
      NodePtr action_node = smblock->get_operand(0)->get_operand(0);
      if (action_node->is_leaf()) {
	 std::string statename = action_node->get_token().get_text();
	 newstate = sm->get_state_by_name(statename);
	 if (newstate < 0) {
	    throw Exception(action_node->get_location(),
	       "unknown state: " + statename);
	 }
      } else if (action_node->get_op() == Op::CREATE) {
	 action = StateMachineRuleAlternative::create;
      } else if (action_node->get_op() == Op::CLOSE) {
	 action = StateMachineRuleAlternative::close;
      } else if (action_node->get_op() == Op::CUT) {
	 action = StateMachineRuleAlternative::cut;
      } else if (action_node->get_op() == Op::RETRACT) {
	 action = StateMachineRuleAlternative::retract;
      } else if (action_node->get_op() == Op::CACHE) {
	 action = StateMachineRuleAlternative::cache;
	 return_node = action_node->get_operand(0);
      } else {
	 assert(false); std::abort();
      }
      ++nextop;
   }
   StateMachineRuleAlternativePtr alternative;
   if (nextop < smblock->size()) {
      NodePtr block = smblock->get_operand(nextop);
      if (action != StateMachineRuleAlternative::null) {
	 alternative = std::make_shared<StateMachineRuleAlternative>(action,
	    block);
      } else if (newstate >= 0) {
	 alternative = std::make_shared<StateMachineRuleAlternative>(newstate,
	    block);
      } else {
	 alternative = std::make_shared<StateMachineRuleAlternative>(block);
      }
   } else {
      if (action != StateMachineRuleAlternative::null) {
	 alternative = std::make_shared<StateMachineRuleAlternative>(action);
      } else {
	 alternative = std::make_shared<StateMachineRuleAlternative>(newstate);
      }
   }
   if (return_node) {
      alternative->set_return_node(return_node);
   }
   return alternative;
}

static void add_states(StateMachinePtr sm, NodePtr root) {
   if (!root->is_leaf() && root->get_op() == Op::states) {
      add_states(sm, root->get_operand(0));
      root = root->get_operand(1);
   }
   sm->add_state(root->get_token().get_text(), root->get_location());
}

static void add_vars(StateMachinePtr sm, NodePtr root) {
   if (root->get_op() == Op::sm_var_declarations) {
      add_vars(sm, root->get_operand(0));
      root = root->get_operand(1);
   }
   assert(root->get_op() == Op::sm_var_declaration);
   StateMachine::VarKind varkind;
   if (root->get_operand(0)->get_op() == Op::SHARED) {
      varkind = StateMachine::sharedVar;
   } else {
      varkind = StateMachine::privateVar;
   }
   std::string varname = root->get_operand(1)->get_token().get_text();
   if (root->size() == 2) {
      sm->add_var(varkind, varname, root->get_location());
   } else {
      sm->add_var(varkind, varname, root->get_operand(2), root->get_location());
   }
}

static void add_functions(StateMachinePtr sm, NodePtr root) throw(Exception) {
   if (root->get_op() == Op::sm_function_definitions) {
      add_functions(sm, root->get_operand(0));
      root = root->get_operand(1);
   }
   assert(root->get_op() == Op::function_definition);
   std::string fname = root->get_operand(0)->get_token().get_text();
   sm->add_local_function(fname, root->get_operand(1));
}

static void get_nodetypes(BindingsPtr bindings, NodePtr root,
      NodeTypeSet& nodetypes) {
   if (!root->is_leaf() && root->get_op() == Op::cfg_node_types) {
      get_nodetypes(bindings, root->get_operand(0), nodetypes);
      root = root->get_operand(1);
   }
   std::string nodetype = root->get_token().get_text();
   unsigned int index = node_type_by_name(bindings, nodetype);
   if (index >= nodetypes.size()) {
      nodetypes.resize(index + 1);
   }
   nodetypes.set(index);
}

static void get_node_expression(BindingsPtr bindings, NodePtr root,
      NodeTypeSet& nodetypes, NodePtr& expr) {
   assert(root->get_op() == Op::cfg_node_expression);
   if (root->size() == 2) {
      expr = root->get_operand(1);
   }
   root = root->get_operand(0);
   if (root->is_leaf() || root->get_op() == Op::cfg_node_types) {
      get_nodetypes(bindings, root, nodetypes);
   } else {
      nodetypes.resize(0);
   }
}

static void get_labelset(BindingsPtr bindings, NodePtr root, LabelSet& labels) {
   assert(root->get_op() == Op::cfg_edge_condition);
   if (root->size() == 2) {
      get_labelset(bindings, root->get_operand(0), labels);
      root = root->get_operand(1);
   }
   std::string label = root->get_operand(0)->get_token().get_text();
   unsigned int index = label_by_name(bindings, label);
   if (index >= labels.size()) {
      labels.resize(index + 1);
   }
   labels.set(index);
}

static void get_states(StateMachinePtr sm, NodePtr root, StateSet& states) {
   assert(root->get_op() == Op::sm_state_condition);
   if (root->size() == 2) {
      get_states(sm, root->get_operand(0), states);
      root = root->get_operand(1);
   } else {
      root = root->get_operand(0);
   }
   std::string statename = root->get_token().get_text();
   int index = sm->get_state_by_name(statename);
   if (index < 0) {
      throw Exception(root->get_location(),
	 "unknown state: " + statename);
   }
   states.set(index);
}

static void add_alternatives(StateMachinePtr sm, StateMachineRulePtr rule,
      BindingsPtr bindings, NodePtr root) {
   if (root->get_op() == Op::sm_alternatives) {
      add_alternatives(sm, rule, bindings, root->get_operand(0));
      root = root->get_operand(1);
   }
   assert(root->get_op() == Op::sm_alternative);
   NodePtr smblock = root->get_operand(root->size() - 1);
   StateMachineRuleAlternativePtr alternative = create_alternative(sm, smblock);
   unsigned int nextop = 0;
   if (root->get_operand(nextop)->get_op() == Op::cfg_edge_condition) {
      NodePtr edge_cond = root->get_operand(nextop++);
      LabelSet labels(nof_labels(bindings));
      get_labelset(bindings, edge_cond, labels);
      alternative->set_labels(labels);
   }
   if (nextop + 1 < root->size()) {
      NodePtr state_cond = root->get_operand(nextop++);
      StateSet states(sm->get_nofstates());
      get_states(sm, state_cond, states);
      alternative->set_states(states);
   }
   rule->add_alternative(alternative);
}

static void add_rules(StateMachinePtr sm, BindingsPtr bindings, NodePtr root) {
   if (root->get_op() == Op::sm_rules) {
      add_rules(sm, bindings, root->get_operand(0));
      root = root->get_operand(1);
   }
   assert(root->get_op() == Op::sm_rule);
   if (root->size() == 1) {
      // handler
      root = root->get_operand(0);
      assert(root->get_op() == Op::ON);
      assert(root->get_operand(0)->get_op() == Op::CLOSE);
      root = root->get_operand(1);
      assert(root->get_op() == Op::sm_handler);
      if (root->size() == 1) {
	 StateSet states;
	 sm->add_close_handler(states, root->get_operand(0));
      } else {
	 StateSet states(sm->get_nofstates());
	 get_states(sm, root->get_operand(0), states);
	 sm->add_close_handler(states, root->get_operand(1));
      }
   } else {
      // regular rule
      NodePtr smcond = root->get_operand(0);
      StateMachineRulePtr rule;
      NodePtr node_expr;
      if (smcond->size() == 2) {
	 NodePtr tree_expr = smcond->get_operand(0);
	 NodeTypeSet nodetypes(nof_node_types(bindings));
	 get_node_expression(bindings, smcond->get_operand(1),
	    nodetypes, node_expr);
	 rule = std::make_shared<StateMachineRule>(sm,
		  tree_expr, nodetypes, bindings->get_rules());
      } else {
	 if (smcond->get_operand(0)->get_op() == Op::cfg_node_expression) {
	    NodeTypeSet nodetypes(nof_node_types(bindings));
	    get_node_expression(bindings, smcond->get_operand(0),
	       nodetypes, node_expr);
	    rule = std::make_shared<StateMachineRule>(sm,
	       nodetypes, bindings->get_rules());
	 } else {
	    NodePtr tree_expr = smcond->get_operand(0);
	    rule = std::make_shared<StateMachineRule>(sm,
	       tree_expr, bindings->get_rules());
	 }
      }
      if (node_expr) {
	 rule->add_node_condition(node_expr);
      }
      if (root->get_operand(1)->get_op() == Op::sm_block) {
	 NodePtr smblock = root->get_operand(1);
	 rule->add_alternative(create_alternative(sm, smblock));
      } else {
	 add_alternatives(sm, rule, bindings, root->get_operand(1));
      }
      sm->add_rule(rule);
   }
}

StateMachinePtr construct_sm(BindingsPtr bindings, NodePtr root,
      unsigned int id, const AbstractStateMachineTable& asmt) throw(Exception) {
   assert(root && root->get_op() == Op::state_machine);
   assert(root->size() == 5 || root->size() == 6);
   int argi = 0;
   std::string name = root->get_operand(argi++)->get_token().get_text();
   std::string base_sm;
   if (root->size() == 6) {
      base_sm = root->get_operand(argi++)->get_token().get_text();
   }
   NodePtr states = root->get_operand(argi++);
   NodePtr vars = root->get_operand(argi++);
   NodePtr funcs = root->get_operand(argi++);
   NodePtr rules = root->get_operand(argi++);
   StateMachinePtr sm = std::make_shared<StateMachine>(bindings, name, id);
   if (base_sm != "") {
      AbstractStateMachineTable::const_iterator it = asmt.find(base_sm);
      if (it == asmt.end()) {
	 throw Exception(root->get_operand(1)->get_location(),
	    "no such abstract state machine");
      }
      sm->import_asm(it->second);
   }
   add_states(sm, states);
   assert(vars->get_op() == Op::sm_vars);
   if (vars->size() > 0) {
      add_vars(sm, vars->get_operand(0));
   }
   assert(funcs->get_op() == Op::sm_functions);
   if (funcs->size() > 0) {
      add_functions(sm, funcs->get_operand(0));
   }
   add_rules(sm, bindings, rules);
   return sm;
}

StateMachinePtr construct_asm(BindingsPtr bindings, NodePtr root,
      const AbstractStateMachineTable& asmt) throw(Exception) {
   assert(root && root->get_op() == Op::abstract_state_machine);
   assert(root->size() == 4 || root->size() == 5);
   int argi = 0;
   std::string name = root->get_operand(argi++)->get_token().get_text();
   std::string base_sm;
   if (root->size() == 5) {
      base_sm = root->get_operand(argi++)->get_token().get_text();
   }
   NodePtr vars = root->get_operand(argi++);
   NodePtr funcs = root->get_operand(argi++);
   NodePtr rules = root->get_operand(argi++);
   StateMachinePtr sm = std::make_shared<StateMachine>(bindings, name);
   if (base_sm != "") {
      AbstractStateMachineTable::const_iterator it = asmt.find(base_sm);
      if (it == asmt.end()) {
	 throw Exception(root->get_operand(1)->get_location(),
	    "no such abstract state machine");
      }
      sm->import_asm(it->second);
   }
   assert(vars->get_op() == Op::sm_vars);
   if (vars->size() > 0) {
      add_vars(sm, vars->get_operand(0));
   }
   assert(funcs->get_op() == Op::sm_functions);
   if (funcs->size() > 0) {
      add_functions(sm, funcs->get_operand(0));
   }
   add_rules(sm, bindings, rules);
   return sm;
}

} // namespace
