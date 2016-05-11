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

#ifndef ASTL_RULES_H
#define ASTL_RULES_H

#include <iostream>
#include <map>
#include <list>
#include <astl/syntax-tree.hpp>
#include <astl/rule-table.hpp>
#include <astl/operator-table.hpp>
#include <astl/exception.hpp>
#include <astl/loader.hpp>
#include <astl/state-machine.hpp>

namespace Astl {

   typedef std::map<std::string, NodePtr> FunctionTable;
   typedef std::map<std::string, OperatorSetPtr> OperatorSetTable;
   typedef std::map<std::string, RuleTablePtr> NamedRulesTable;

   class Rules {
      public:
	 Rules(Loader& loader_param);
	 Rules(NodePtr root, Loader& loader_param) throw(Exception);

	 // mutator
	 void scan(NodePtr root) throw(Exception);

	 // accessors
	 OperatorSetPtr get_opset(const std::string& name) const;

	 bool print_rules_defined() const;
	 const RuleTable& get_print_rule_table() const;
	 const NamedRulesTable& get_named_print_rule_tables() const;

	 bool operator_rules_defined() const;
	 const OperatorTable& get_operator_table() const;

	 bool transformation_rules_defined() const;
	 const RuleTable& get_transformation_rule_table() const;
	 const NamedRulesTable& get_named_transformation_rule_tables() const;
	 const NamedRulesTable& get_named_inplace_transformation_rule_tables()
	    const;

	 bool attribution_rules_defined() const;
	 const RuleTable& get_attribution_rule_table() const;
	 const NamedRulesTable& get_named_attribution_rule_tables() const;

	 const FunctionTable& get_function_table() const;
	 NodePtr get_function(const std::string& fname) const;

	 const StateMachineTable& get_sm_table(BindingsPtr bindings_param)
	    const;

      private:
	 Loader& loader;

	 OperatorSetTable opsets;

	 bool print_rules_ok;
	 RuleTable print_rt;
	 NamedRulesTable named_print_rules;

	 bool operator_rules_ok;
	 OperatorTable optab;

	 bool transformation_rules_ok;
	 RuleTable trans_rt;
	 NamedRulesTable named_transformation_rules;
	 NamedRulesTable named_inplace_transformation_rules;

	 bool attribution_rules_ok;
	 RuleTable attribution_rt;
	 NamedRulesTable named_attribution_rules;

	 FunctionTable ftab;

	 mutable BindingsPtr bindings;
	 mutable StateMachineTable smtab;
	 std::list<NodePtr> smnodes; // state machines
	 std::list<NodePtr> asmnodes; // abstract state machines

	 void add_to_named_rules(NamedRulesTable& nrtab,
	    const std::string& name, NodePtr root, const Operator& ruleop);
   };

} // namespace Astl

#endif
