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
#include <astl/syntax-tree.hpp>
#include <astl/rule-table.hpp>
#include <astl/operators.hpp>
#include <astl/opset.hpp>

namespace Astl {

RuleTable::RuleTable() : current_rank(0) {
}

void RuleTable::traverse(NodePtr node,
      const Operator& ruleop, const Rules& rules) {
   if (node->is_leaf()) return;
   if (node->get_op() == ruleop) {
      /* put this rule into one of the tables */
      assert(node->size() >= 2);
      const NodePtr& tree_expr = node->get_operand(0);
      const NodePtr& rhs = node->get_operand(1);
      std::string rulename;
      if (node->size() >= 3) {
	 rulename = node->get_operand(2)->get_token().get_text();
      }
      RulePtr rule(new Rule(tree_expr, rhs, rulename, rules));

      Arity arity = rule->get_arity();
      Rule::Type rtype = rule->get_type();

      /* insert rule for all operators in its operator set */
      OperatorSetPtr opset = rule->get_opset();
      for (OperatorSet::Iterator opit = opset->begin();
	    opit != opset->end(); ++opit) {
	 std::string opname = *opit;
	 key_pair key(opname, arity);
	 map_type::iterator it = table[rtype].find(key);
	 if (it == table[rtype].end()) {
	    /* create an empty map and insert it */
	    std::pair<map_type::iterator, bool> result =
	       table[rtype].insert(pair(key, submap_type()));
	    assert(result.second);
	    it = result.first;
	 }
	 it->second.insert(value_pair(++current_rank, rule));
      }
   } else {
      /* recursive traverse */
      for (unsigned int i = 0; i < node->size(); ++i) {
	 traverse(node->get_operand(i), ruleop, rules);
      }
   }
}

void RuleTable::scan(NodePtr root, const Operator& ruleop, const Rules& rules) {
   traverse(root, ruleop, rules);
}

RuleTable::iterator RuleTable::find_prefix(const Operator& op,
      Arity arity, iterator& end) const {
   return find(op, arity, Rule::prefix, end);
}

RuleTable::iterator RuleTable::find_postfix(const Operator& op,
      Arity arity, iterator& end) const {
   return find(op, arity, Rule::postfix, end);
}

RuleTable::iterator RuleTable::find(const Operator& op, Arity arity,
	    Rule::Type rtype, iterator& end) const {
   std::string opname(op.get_name());
   map_type::const_iterator it = table[rtype].find(key_pair(opname, arity));
   if (it == table[rtype].end()) {
      return end = submap_type().end();
   } else {
      end = it->second.end();
      return it->second.begin();
   }
}

RuleTable::print_iterator RuleTable::reversed_find(const Operator& op,
      Arity arity, print_iterator& end) const {
   std::string opname(op.get_name());
   map_type::const_iterator it =
      table[Rule::prefix].find(key_pair(opname, arity));
   if (it == table[Rule::prefix].end()) {
      return end = submap_type().rend();
   } else {
      end = it->second.rend();
      return it->second.rbegin();
   }
}

unsigned int RuleTable::size() const {
   return current_rank;
}

} // namespace Astl
