/*
   Copyright (C) 2011 Andreas Franz Borchert
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

#include <algorithm>
#include <astl/set-ops.hpp>
#include <astl/operators.hpp>
#include <astl/integer.hpp>

namespace Astl {

AttributePtr set_binary_op(const Operator& op,
      AttributePtr leftAt, AttributePtr rightAt,
      const Location& loc) throw(Exception) {
   assert(leftAt && rightAt &&
      leftAt->get_type() == Attribute::dictionary &&
      rightAt->get_type() == Attribute::dictionary);
   Attribute::DictionaryIterator it1 = leftAt->get_pairs_begin();
   Attribute::DictionaryIterator it1_last = leftAt->get_pairs_end();
   Attribute::DictionaryIterator it2 = rightAt->get_pairs_begin();
   Attribute::DictionaryIterator it2_last = rightAt->get_pairs_end();
   AttributePtr result = AttributePtr(new Attribute(Attribute::dictionary));
   Attribute::DictionaryInserter ins = result->get_inserter();
   switch (op.get_opcode()) {
      case ASTL_OPERATOR_PLUS_TK:
	 /* set union */
	 std::set_union(it1, it1_last, it2, it2_last, ins, dict_less_than);
	 break;

      case ASTL_OPERATOR_MINUS_TK:
	 /* set difference */
	 std::set_difference(it1, it1_last, it2, it2_last, ins,
	    dict_less_than);
	 break;

      case ASTL_OPERATOR_POWER_TK:
	 /* symmetric difference */
	 std::set_symmetric_difference(it1, it1_last, it2, it2_last, ins,
	    dict_less_than);
	 break;

      case ASTL_OPERATOR_STAR_TK:
	 /* set intersection */
	 std::set_intersection(it1, it1_last, it2, it2_last, ins,
	    dict_less_than);
	 break;

      default:
	 assert(false);
   }
   return result;
}

} // namespace Astl
