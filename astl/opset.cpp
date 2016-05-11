/*
   Copyright (C) 2009-2010 Andreas Franz Borchert
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
#include <astl/exception.hpp>
#include <astl/rules.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/operators.hpp>

namespace Astl {

OperatorSet::OperatorSet(NodePtr opset_expr,
      const Rules& rules) throw(Exception) {
   traverse(opset_expr, rules);
}

void OperatorSet::add(NodePtr node, const Rules& rules) throw(Exception) {
   if (node->is_leaf()) {
      /* identifier refering to an operator set clause */
      std::string name = node->get_token().get_text();
      OperatorSetPtr set = rules.get_opset(name);
      if (set) {
	 for (OperatorSet::Iterator it = set->begin();
		  it != set->end(); ++it) {
	    opset.insert(*it);
	 }
      } else {
	 throw Exception(node->get_location(),
	    "undeclared operator set: " + name);
      }
   } else {
      /* string literal */
      assert(node->get_op() == Op::string_literal);
      node = node->get_operand(0);
      std::string opname = node->get_token().get_text();
      opset.insert(opname);
   }
}

void OperatorSet::traverse(NodePtr opset_expr,
      const Rules& rules) throw(Exception) {
   NodePtr node = opset_expr;
   std::string opname;
   while (!node->is_leaf() && node->size() == 2) {
      assert(node->get_op() == Op::operators);
      add(node->get_operand(1), rules);
      node = node->get_operand(0);
   }
   add(node, rules);
}

unsigned int OperatorSet::get_card() const {
   return opset.size();
}

bool OperatorSet::includes(const std::string& opname) const {
   Iterator it = opset.find(opname);
   return it != opset.end();
}

bool OperatorSet::includes(const Operator& op) const {
   std::string opname(op.get_name());
   Iterator it = opset.find(opname);
   return it != opset.end();
}

OperatorSet::Iterator OperatorSet::begin() const {
   return opset.begin();
}

OperatorSet::Iterator OperatorSet::end() const {
   return opset.end();
}

} // namespace Astl
