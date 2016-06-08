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
#include <astl/exception.hpp>
#include <astl/operator.hpp>
#include <astl/operators.hpp>
#include <astl/rules.hpp>

namespace Astl {

// constructors =============================================================

Rules::Rules(Loader& loader_param) :
   loader(loader_param),
   print_rules_ok(false), operator_rules_ok(false),
   transformation_rules_ok(false), attribution_rules_ok(false) {
}

Rules::Rules(NodePtr root, Loader& loader_param) throw(Exception) :
      loader(loader_param),
      print_rules_ok(false), operator_rules_ok(false),
      transformation_rules_ok(false), attribution_rules_ok(false) {
   scan(root);
}

// mutators =================================================================

void Rules::add_to_named_rules(NamedRulesTable& nrtab,
      const std::string& name, NodePtr root, const Operator& ruleop) {
   NamedRulesTable::iterator it = nrtab.find(name);
   if (it != nrtab.end()) {
      it->second->scan(root, ruleop, *this);
   } else {
      RuleTablePtr rt = std::make_shared<RuleTable>();
      rt->scan(root, ruleop, *this);
      std::pair<NamedRulesTable::iterator, bool> result =
	 nrtab.insert(make_pair(name, rt));
      assert(result.second);
   }
}

void Rules::scan(NodePtr root) throw(Exception) {
   assert(!root->is_leaf());
   assert(root->get_op() == Op::unit && root->size() == 2);

   // process clauses
   NodePtr clauses = root->get_operand(0);
   for (std::size_t i = 0; i < clauses->size(); ++i) {
      NodePtr clause = clauses->get_operand(i);
      assert(!clause->is_leaf());
      Operator op = clause->get_op();
      if (op == Op::library_clause) {
	 std::string libdir = clause->get_operand(0)->get_token().get_text();
	 loader.add_library_in_front(libdir);
      } else if (op == Op::import_clause) {
	 std::string name = clause->get_operand(0)->get_token().get_text();
	 if (!loader.loaded(name)) {
	    scan(loader.load(name));
	 }
      } else if (op == Op::operator_set_clause) {
	 std::string name = clause->get_operand(0)->get_token().get_text();
	 auto opset = std::make_shared<OperatorSet>(clause->get_operand(1),
	    *this);
	 std::pair<OperatorSetTable::iterator, bool> result =
	    opsets.insert(make_pair(name, opset));
	 if (!result.second) {
	    throw Exception(clause->get_location(),
	       "operator set multiply defined");
	 }
      } else {
	 assert(false); std::abort();
      }
   }

   // process rule sets
   NodePtr rule_list = root->get_operand(1);
   assert(!rule_list->is_leaf());
   assert(rule_list->get_op() == Op::rules);
   for (std::size_t i = 0; i < rule_list->size(); ++i) {
      NodePtr rules = rule_list->get_operand(i);
      assert(!rules->is_leaf());
      Operator op = rules->get_op();
      if (op == Op::print_rules) {
	 print_rt.scan(rules, Op::print_rule, *this);
	 print_rules_ok = true;
      } else if (op == Op::operator_rules) {
	 if (operator_rules_ok) {
	    throw Exception(rules->get_location(),
	       "operator rules multiply defined");
	 }
	 optab.scan(rules, *this);
	 operator_rules_ok = true;
      } else if (op == Op::transformation_rules) {
	 trans_rt.scan(rules, Op::transformation, *this);
	 transformation_rules_ok = true;
      } else if (op == Op::named_transformation_rules) {
	 std::string name = rules->get_operand(0)->get_token().get_text();
	 add_to_named_rules(named_transformation_rules, name,
	    rules->get_operand(1), Op::transformation);
      } else if (op == Op::named_inplace_transformation_rules) {
	 std::string name = rules->get_operand(0)->get_token().get_text();
	 add_to_named_rules(named_inplace_transformation_rules, name,
	    rules->get_operand(1), Op::transformation);
      } else if (op == Op::attribution_rules) {
	 attribution_rt.scan(rules, Op::attribution, *this);
	 attribution_rules_ok = true;
      } else if (op == Op::named_attribution_rules) {
	 std::string name = rules->get_operand(0)->get_token().get_text();
	 add_to_named_rules(named_attribution_rules, name,
	    rules->get_operand(1), Op::attribution);
      } else if (op == Op::named_print_rules) {
	 std::string name = rules->get_operand(0)->get_token().get_text();
	 add_to_named_rules(named_print_rules, name,
	    rules->get_operand(1), Op::print_rule);
      } else if (op == Op::function_definition) {
	 std::string name = rules->get_operand(0)->get_token().get_text();
	 std::pair<FunctionTable::iterator, bool> result =
	    ftab.insert(make_pair(name, rules));
	 if (!result.second) {
	    throw Exception(rules->get_location(),
	       "function " + name + " multiply defined");
	 }
      } else if (op == Op::state_machine) {
	 // postpone parsing as we have no bindings yet
	 smnodes.push_back(rules);
      } else if (op == Op::abstract_state_machine) {
	 // postpone parsing as we have no bindings yet
	 asmnodes.push_back(rules);
      } else {
	 assert(false); std::abort();
      }
   }
}

// accessors ================================================================

OperatorSetPtr Rules::get_opset(const std::string& name) const {
   OperatorSetTable::const_iterator it = opsets.find(name);
   if (it == opsets.end()) {
      return OperatorSetPtr(nullptr);
   } else {
      return it->second;
   }
}

bool Rules::print_rules_defined() const {
   return print_rules_ok;
}

const RuleTable& Rules::get_print_rule_table() const {
   assert(print_rules_ok);
   return print_rt;
}

bool Rules::operator_rules_defined() const {
   return operator_rules_ok;
}

const OperatorTable& Rules::get_operator_table() const {
   assert(operator_rules_ok);
   return optab;
}

bool Rules::transformation_rules_defined() const {
   return transformation_rules_ok;
}

const RuleTable& Rules::get_transformation_rule_table() const {
   assert(transformation_rules_ok);
   return trans_rt;
}

bool Rules::attribution_rules_defined() const {
   return attribution_rules_ok;
}

const RuleTable& Rules::get_attribution_rule_table() const {
   assert(attribution_rules_ok);
   return attribution_rt;
}

const NamedRulesTable& Rules::get_named_attribution_rule_tables() const {
   return named_attribution_rules;
}

const NamedRulesTable& Rules::get_named_print_rule_tables() const {
   return named_print_rules;
}

const NamedRulesTable& Rules::get_named_transformation_rule_tables() const {
   return named_transformation_rules;
}

const NamedRulesTable& Rules::get_named_inplace_transformation_rule_tables()
      const {
   return named_inplace_transformation_rules;
}

const FunctionTable& Rules::get_function_table() const {
   return ftab;
}

NodePtr Rules::get_function(const std::string& fname) const {
   FunctionTable::const_iterator it = ftab.find(fname);
   if (it == ftab.end()) return NodePtr(nullptr);
   return it->second;
}

const StateMachineTable& Rules::get_sm_table(BindingsPtr bindings_param) const {
   if (bindings_param != bindings) {
      bindings = bindings_param;
      AbstractStateMachineTable asmt;
      for (std::list<NodePtr>::const_iterator it = asmnodes.begin();
	    it != asmnodes.end(); ++it) {
	 StateMachinePtr sm = construct_asm(bindings, *it, asmt);
	 asmt[sm->get_name()] = sm;
	 smtab.add(sm);
      }

      std::size_t id = 0;
      smtab.clear();
      for (std::list<NodePtr>::const_iterator it = smnodes.begin();
	    it != smnodes.end(); ++it) {
	 StateMachinePtr sm = construct_sm(bindings, *it, id++, asmt);
	 smtab.add(sm);
      }
   }
   return smtab;
}

} // namespace Astl
