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

#include <cstdlib>
#include <astl/list-ops.hpp>
#include <astl/operators.hpp>

namespace Astl {

AttributePtr list_binary_op(const Operator& op,
      AttributePtr leftAt, AttributePtr rightAt,
      const Location& loc) throw(Exception) {
   if (!leftAt) {
      leftAt = AttributePtr(new Attribute(Attribute::list));
   }
   if (!rightAt) {
      rightAt = AttributePtr(new Attribute(Attribute::list));
   }
   switch (op.get_opcode()) {
      case ASTL_OPERATOR_AMPERSAND_TK:
	 // list concatenation
	 {
	    AttributePtr resultAt =
	       AttributePtr(new Attribute(Attribute::list));
	    if (leftAt->get_type() != Attribute::list) {
	       resultAt->push_back(leftAt);
	    } else {
	       for (unsigned int i = 0; i < leftAt->size(); ++i) {
		  resultAt->push_back(leftAt->get_value(i));
	       }
	    }
	    if (rightAt->get_type() != Attribute::list) {
	       resultAt->push_back(rightAt);
	    } else {
	       for (unsigned int i = 0; i < rightAt->size(); ++i) {
		  resultAt->push_back(rightAt->get_value(i));
	       }
	    }
	    return resultAt;
	 }

      default:
	 assert(false); std::abort();
   }
}

} // namespace Astl
