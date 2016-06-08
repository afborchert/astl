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

#include <cassert>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <astl/designator.hpp>
#include <astl/execution.hpp>
#include <astl/expression.hpp>
#include <astl/operators.hpp>
#include <astl/parser.hpp>

namespace Astl {

#define EXEC(block) \
   if (recursive_execute(block, local_bindings, rval)) { \
      return true; \
   }

// returns true if we have to return
bool recursive_execute(NodePtr block, BindingsPtr bindings,
	 AttributePtr& rval)
      throw(Exception) {
   BindingsPtr local_bindings = std::make_shared<Bindings>(bindings);
   assert(block->get_op() == Op::block);
   for (std::size_t i = 0; i < block->size(); ++i) {
      NodePtr statement = block->get_operand(i);
      switch (statement->get_op().get_opcode()) {
	 case ASTL_OPERATOR_IF_STATEMENT:
	    {
	       Expression cond(statement->get_operand(0), local_bindings);
	       if (cond.convert_to_bool()) {
		  EXEC(statement->get_operand(1));
	       } else if (statement->size() >= 3 &&
		     statement->get_operand(2)->get_op() == Op::elsif_chain) {
		  NodePtr chain = statement->get_operand(2)->get_operand(0);
		  typedef std::list<std::pair<NodePtr, NodePtr> > ElsifChain;
		  ElsifChain members;
		  for(;;) {
		     if (chain->get_op() == Op::elsif_statement) {
			members.push_front(std::make_pair(
			   chain->get_operand(0), chain->get_operand(1)));
			break;
		     } else {
			NodePtr elsif = chain->get_operand(1);
			members.push_front(std::make_pair(
			   elsif->get_operand(0), elsif->get_operand(1)));
			chain = chain->get_operand(0);
		     }
		  }
		  bool executed = false;
		  for (ElsifChain::iterator it = members.begin();
			it != members.end(); ++it) {
		     Expression cond(it->first, local_bindings);
		     if (cond.convert_to_bool()) {
			EXEC(it->second);
			executed = true; break;
		     }
		  }
		  if (!executed && statement->size() == 4) {
		     EXEC(statement->get_operand(3));
		  }
	       } else if (statement->size() == 3) {
		  EXEC(statement->get_operand(2));
	       }
	    }
	    break;

	 case ASTL_OPERATOR_WHILE_STATEMENT:
	    for(;;) {
	       Expression cond(statement->get_operand(0), local_bindings);
	       if (!cond.convert_to_bool()) break;
	       EXEC(statement->get_operand(1));
	    }
	    break;

	 case ASTL_OPERATOR_FOREACH_STATEMENT:
	    if (statement->size() == 3) {
	       /* simple foreach statement for lists */
	       std::string varname =
		  statement->get_operand(0)->get_token().get_text();
	       NodePtr inner_block = statement->get_operand(2);
	       Expression list_expr(statement->get_operand(1), local_bindings);
	       AttributePtr list = list_expr.convert_to_list();
	       for (std::size_t i = 0; i < list->size(); ++i) {
		  AttributePtr indexVal = list->get_value(i);
		  BindingsPtr for_bindings =
		     std::make_shared<Bindings>(local_bindings);
		  bool ok = for_bindings->define(varname, indexVal); assert(ok);
		  if (recursive_execute(inner_block, for_bindings, rval)) {
		     return true;
		  }
	       }
	    } else {
	       /* foreach statement for dictionaries */
	       assert(statement->size() == 4);
	       std::string key_varname =
		  statement->get_operand(0)->get_token().get_text();
	       std::string value_varname =
		  statement->get_operand(1)->get_token().get_text();
	       NodePtr inner_block = statement->get_operand(3);
	       Expression dict_expr(statement->get_operand(2), local_bindings);
	       AttributePtr dict = dict_expr.get_result();
	       if (!dict || dict->get_type() != Attribute::dictionary) {
		  throw Exception(statement->get_operand(2)->get_location(),
			   "dictionary expected");
	       }
	       Attribute::DictionaryIterator it = dict->get_pairs_begin();
	       while (it != dict->get_pairs_end()) {
		  BindingsPtr for_bindings =
		     std::make_shared<Bindings>(local_bindings);
		  bool ok = for_bindings->define(key_varname,
		     std::make_shared<Attribute>(it->first));
		  ok = ok && for_bindings->define(value_varname, it->second);
		  assert(ok);
		  if (recursive_execute(inner_block, for_bindings, rval)) {
		     return true;
		  }
		  ++it;
	       }
	    }
	    break;

	 case ASTL_OPERATOR_VAR_STATEMENT:
	    {
	       std::string varname =
		  statement->get_operand(0)->get_token().get_text();
	       AttributePtr initValue = AttributePtr(nullptr);
	       if (statement->size() == 2) {
		  Expression init_expr(statement->get_operand(1), local_bindings);
		  initValue = init_expr.get_result();
	       }
	       if (!local_bindings->define(varname, initValue)) {
		  std::ostringstream os;
		  os << "multiply defined variable: " << varname;
		  throw Exception(statement->get_operand(0)->get_location(),
			   os.str());
	       }
	    }
	    break;

	 case ASTL_OPERATOR_RETURN_STATEMENT:
	    if (statement->size() == 1) {
	       Expression rexpr(statement->get_operand(0), local_bindings);
	       rval = rexpr.get_result();
	    }
	    return true;

	 case ASTL_OPERATOR_EXPRESSION:
	    {
	       // expression statement
	       Expression expr(statement->get_operand(0), local_bindings);
	    }
	    break;

	 case ASTL_OPERATOR_DELETE_STATEMENT:
	    {
	       Expression expr(statement->get_operand(0), local_bindings);
	       DesignatorPtr desat = expr.get_designator();
	       desat->delete_key(statement->get_location());
	    }
	    break;

	 default:
	    assert(false); std::abort();
      }
   }
   return false;
}

AttributePtr execute(NodePtr block, BindingsPtr bindings)
      throw(Exception) {
   AttributePtr return_value;
   if (recursive_execute(block, bindings, return_value)) {
      return return_value;
   } else {
      return AttributePtr(nullptr);
   }
}

void execute(NodePtr root, const RuleTable& rules) throw(Exception) {
   CandidateSet candidates(root, rules);
   execute(candidates);
}

void execute(NodePtr root, const RuleTable& rules,
      BindingsPtr bindings) throw(Exception) {
   CandidateSet candidates(root, rules, bindings);
   execute(candidates);
}

void execute(const CandidateSet& candidates) throw(Exception) {
   for (std::size_t i = 0; i < candidates.size(); ++i) {
      CandidatePtr candidate = candidates[i];
      RulePtr rule(candidate->get_rule());
      BindingsPtr bindings = candidate->get_bindings();
      std::string node_name(rule->get_name());
      if (node_name.size() > 0) {
	 if (!bindings->define(node_name,
	       std::make_shared<Attribute>(candidate->get_subtree()))) {
	    std::ostringstream os;
	    os << "multiply defined variable: " << node_name;
	    throw Exception(candidate->get_subtree()->get_location(), os.str());
	 }
      }
      execute(rule->get_rhs(), bindings);
   }
}

} // namespace Astl
