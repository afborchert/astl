/*
   Copyright (C) 2009, 2010 Andreas Franz Borchert
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

#ifndef ASTL_STATE_MACHINE_H
#define ASTL_STATE_MACHINE_H

#include <list>
#include <map>
#include <memory>
#include <string>
#include <boost/dynamic_bitset.hpp>
#include <astl/exception.hpp>
#include <astl/flow-graph.hpp>
#include <astl/rule.hpp>
#include <astl/treeloc.hpp>
#include <astl/types.hpp>

namespace Astl {

   typedef boost::dynamic_bitset<std::size_t> StateSet;

   class StateMachine;
   typedef std::shared_ptr<StateMachine> StateMachinePtr;

   class StateMachineRuleAlternative {
      public:
	 typedef enum {null, cache, create, close, cut, retract} Action;
	 // constructors
	 StateMachineRuleAlternative(NodePtr block);
	 StateMachineRuleAlternative(Action action);
	 StateMachineRuleAlternative(int newstate);
	 StateMachineRuleAlternative(Action action, NodePtr block);
	 StateMachineRuleAlternative(int newstate, NodePtr block);
	 // mutators
	 void set_labels(const LabelSet& labels_param);
	 void set_states(const StateSet& states_param);
	 void set_return_node(NodePtr node); // if action == cache
	 // accessors
	 const LabelSet& get_labels() const;
	 const StateSet& get_states() const;
	 Action get_action() const;
	 NodePtr get_return_node() const; // if action == cache
	 NodePtr get_block() const;
	 int get_newstate() const;
      private:
	 friend class StateMachine;
	 friend class StateMachineRule;
	 // additional conditions of this alternative
	 LabelSet labels;
	 StateSet states;
	 // actions & state changes if a rule matches
	 int newstate; // -1 if this rule has now new state
	 Action action;
	 NodePtr block; // may be 0
	 NodePtr return_node; // != 0 if action == cache
   };
   typedef std::shared_ptr<StateMachineRuleAlternative>
      StateMachineRuleAlternativePtr;

   class StateMachineRule: public BasicRule {
      public:
	 typedef std::list<StateMachineRuleAlternativePtr>::const_iterator
	    AlternativesIterator;
	 // constructors
	 StateMachineRule(StateMachinePtr sm, NodePtr tree_expr_param,
	    const Rules& rules);
	 StateMachineRule(StateMachinePtr sm,
	    const NodeTypeSet& nodetypes_param, const Rules& rules);
	 StateMachineRule(StateMachinePtr sm, NodePtr tree_expr_param,
	    const NodeTypeSet& nodetypes_param, const Rules& rules);
	 // mutators
	 void add_node_condition(NodePtr node_expr_param);
	 void add_alternative(StateMachineRuleAlternativePtr alternative);
	 // accessors
	 StateMachinePtr get_sm() const;
	 bool tree_expr_defined() const;
	 const NodeTypeSet& get_nodetypes() const;
	 NodePtr get_node_condition() const;
	 // non-creating alternatives
	 bool non_creating() const;
	 AlternativesIterator get_alternatives_begin() const;
	 AlternativesIterator get_alternatives_end() const;
	 // creating rule
	 bool creating() const;
	 NodePtr get_block() const;
      private:
	 friend class StateMachine;
	 StateMachinePtr sm;
	 // cfg_node_expression
	 NodeTypeSet nodetypes;
	 NodePtr node_expr; // condition of cfg_node_expression, may be 0
	 // list of alternatives
	 std::list<StateMachineRuleAlternativePtr> alternatives;
	 // creating rule:
	 bool creating_rule;
	 NodePtr block; // may be 0
   };
   typedef std::shared_ptr<StateMachineRule> StateMachineRulePtr;

   class StateMachine {
      public:
	 typedef std::list<StateMachineRulePtr>::const_iterator Iterator;
	 // constructor
	 StateMachine(BindingsPtr bindings,
	    const std::string& name, std::size_t id);
	 StateMachine(BindingsPtr bindings,
	    const std::string& name); // abstract state machine
	 // mutators
	 void import_asm(StateMachinePtr sm);
	 void add_state(const std::string& state,
	    const Location& loc) throw(Exception);
	 void add_rule(StateMachineRulePtr rule);
	 typedef enum {privateVar, sharedVar} VarKind;
	 void add_var(VarKind varkind, const std::string& varname,
	    const Location& loc);
	 void add_var(VarKind varkind, const std::string& varname,
	    NodePtr init_expr, const Location& loc);
	 void add_close_handler(const StateSet& states, NodePtr handler);
	 void add_local_function(const std::string& name,
	    NodePtr block) throw(Exception);
	 // accessors
	 bool is_abstract() const;
	 std::size_t get_id() const;
	 BindingsPtr get_shared_bindings() const;
	 BindingsPtr add_private_bindings(BindingsPtr shared_bindings) const;
	 void update_function_bindings(BindingsPtr local_bindings) const;
	 std::string get_name() const;
	 std::size_t get_nofstates() const;
	 std::size_t get_nofxstates() const;
	 const std::string& get_state_by_number(int index) const;
	 int get_state_by_name(const std::string& name) const;
	 bool is_global() const;
	 Iterator get_rules_begin() const;
	 Iterator get_rules_end() const;
	 Iterator get_creating_rules_begin() const;
	 Iterator get_creating_rules_end() const;
	 void run_close_handlers(int state, BindingsPtr local_bindings) const
	    throw(Exception);
      private:
	 bool abstract;
	 bool global;
	 std::string name;
	 // variables
	 std::map<std::string, NodePtr> vars;
	 typedef struct {
	    VarKind kind;
	    std::string name;
	    NodePtr init_expr;
	 } Variable;
	 std::list<Variable> shared_var_list;
	 std::list<Variable> private_var_list;
	 BindingsPtr bindings;
	 // list of rules
	 std::list<StateMachineRulePtr> rules;
	 std::list<StateMachineRulePtr> creating_rules;
	 // list of handlers
	 typedef struct {
	    StateSet states;
	    NodePtr block;
	 } Handler;
	 std::list<Handler> close_handlers;
	 // list of local functions
	 typedef std::map<std::string, NodePtr> FunctionTable;
	 FunctionTable local_functions;
	 // non-abstract state machines:
	 std::size_t id;
	 std::map<int, std::string> stateByNumber;
	 std::map<std::string, int> stateByName;
   };

   typedef std::map<std::string, StateMachinePtr> AbstractStateMachineTable;
   StateMachinePtr construct_sm(BindingsPtr bindings, NodePtr root,
      std::size_t id, const AbstractStateMachineTable& asmt) throw(Exception);
   // construct abstract state machine
   StateMachinePtr construct_asm(BindingsPtr bindings, NodePtr root,
      const AbstractStateMachineTable& asmt) throw(Exception);

   class StateMachineTable {
      public:
	 typedef std::list<StateMachinePtr>::const_iterator Iterator;
	 // mutators
	 void add(StateMachinePtr sm);
	 void clear();
	 // accessors
	 std::size_t nof_state_machines() const;
	 Iterator get_global_begin() const;
	 Iterator get_global_end() const;
	 Iterator get_local_begin() const;
	 Iterator get_local_end() const;
      private:
	 std::list<StateMachinePtr> global;
	 std::list<StateMachinePtr> local;
   };

} // namespace Astl

#endif
