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

#include <memory>
#include <astl/parenthesizer.hpp>

namespace Astl {

void parenthesize(NodePtr root, const OperatorTable& optab,
      const Operator& parentheses) {
   if (root->is_leaf()) return;
   Operator op = root->get_op();
   if (optab.included(op) && root->size() > 0) {
      OperatorTable::Associativity assoc = optab.get_associativity(op);
      OperatorTable::Rank rank = optab.get_rank(op);
      for (unsigned int i = 0; i < root->size(); ++i) {
	 NodePtr& subnode(root->get_operand(i));
	 if (!subnode->is_leaf() && subnode->size() > 0) {
	    NodePtr dnode(subnode);
	    Operator inner_op = subnode->get_op();
	    if (optab.included(inner_op)) {
	       OperatorTable::Associativity inner_assoc =
		  optab.get_associativity(inner_op);
	       OperatorTable::Rank inner_rank = optab.get_rank(inner_op);
	       assert(rank != inner_rank || assoc == inner_assoc);
	       if (rank > inner_rank ||
		     (rank == inner_rank &&
			(assoc == OperatorTable::nonassoc ||
			(assoc == OperatorTable::left && i > 0) ||
			(assoc == OperatorTable::right && i == 0)))) {
		  /* parentheses are required */
		  dnode = subnode =
		     std::make_shared<Node>(subnode->get_location(),
			parentheses, subnode);
	       }
	    }
	    parenthesize(dnode, optab, parentheses);
	 }
      }
   } else {
      for (unsigned int i = 0; i < root->size(); ++i) {
	 parenthesize(root->get_operand(i), optab, parentheses);
      }
   }
}

} // namespace Astl
