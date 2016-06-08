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
#include <sstream>
#include <astl/exception.hpp>
#include <astl/operator-table.hpp>
#include <astl/operators.hpp>
#include <astl/opset.hpp>
#include <astl/rules.hpp>

namespace Astl {

// constructors =============================================================

OperatorTable::OperatorTable() : last_opname(nullptr) {};
OperatorTable::OperatorTable(NodePtr root, const Rules& rules) :
      last_opname(nullptr) {
   scan(root, rules);
}

// mutators =================================================================

void OperatorTable::scan(NodePtr root, const Rules& rules) throw(Exception) {
   assert(!root->is_leaf());
   for (Rank rank = 0; rank < root->size(); ++rank) {
      NodePtr operators = root->get_operand(rank);
      assert(!operators->is_leaf());
      Associativity assoc;
      if (operators->get_op() == Op::LEFT) {
	 assoc = left;
      } else if (operators->get_op() == Op::RIGHT) {
	 assoc = right;
      } else if (operators->get_op() == Op::NONASSOC) {
	 assoc = nonassoc;
      } else {
	 assert(false); std::abort();
      }
      for (std::size_t opindex = 0; opindex < operators->size(); ++opindex) {
	 NodePtr opnode = operators->get_operand(opindex);
	 OperatorSet opset(opnode, rules);
	 for (OperatorSet::Iterator opit = opset.begin();
	       opit != opset.end(); ++opit) {
	    Entry entry = {*opit, rank, assoc};
	    auto result = tab.insert(value_type(entry.op, entry));
	    if (!result.second) {
	       std::ostringstream os;
	       os << "multiply listed operator: \"" << entry.op << "\"";
	       throw Exception(opnode->get_location(), os.str());
	    }
	 }
      }
   }
}

// accessors ================================================================

bool OperatorTable::access(const Operator& op) const {
   if (op.get_name() != last_opname) {
      last_opname = op.get_name();
      it = tab.find(last_opname);
   }
   return it != tab.end();
}

bool OperatorTable::included(const Operator& op) const {
   return access(op);
}

OperatorTable::Rank OperatorTable::get_rank(const Operator& op) const {
   bool ok = access(op); assert(ok);
   return it->second.rank;
}

OperatorTable::Associativity
OperatorTable::get_associativity(const Operator& op) const {
   bool ok = access(op); assert(ok);
   return it->second.associativity;
}

} // namespace Astl
