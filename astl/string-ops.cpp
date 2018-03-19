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
#include <string>
#include <astl/attribute.hpp>
#include <astl/exception.hpp>
#include <astl/operator.hpp>
#include <astl/operators.hpp>
#include <astl/string-ops.hpp>

namespace Astl {

AttributePtr string_binary_op(const Operator& op,
      AttributePtr leftAt, AttributePtr rightAt, const Location& loc) {
   std::string left = leftAt? leftAt->convert_to_string(): "";
   std::string right;
   if (op.get_opcode() != ASTL_OPERATOR_X_TK) {
      /* convert right operand into a string */
      right = rightAt? rightAt->convert_to_string(): "";
   }
   switch (op.get_opcode()) {
      case ASTL_OPERATOR_AMPERSAND_TK:
	 // string concatenation
	 return std::make_shared<Attribute>(left + right);

      case ASTL_OPERATOR_X_TK:
	 // repetition operator
	 {
	    /* convert right operand into an integer */
	    unsigned int times = rightAt?
	       rightAt->get_integer()->get_unsigned_int(loc): 0;
	    std::string res;
	    res = "";
	    while (times-- > 0) {
	       res += left;
	    }
	    return std::make_shared<Attribute>(res);
	 }

      case ASTL_OPERATOR_EQEQ_TK:
      case ASTL_OPERATOR_NE_TK:
	 return std::make_shared<Attribute>(
	    (left == right) == (op.get_opcode() == ASTL_OPERATOR_EQEQ_TK));

      case ASTL_OPERATOR_LT_TK:
	 return std::make_shared<Attribute>(
	    (left < right) == (op.get_opcode() == ASTL_OPERATOR_EQEQ_TK));
      case ASTL_OPERATOR_LE_TK:
	 return std::make_shared<Attribute>(
	    (left <= right) == (op.get_opcode() == ASTL_OPERATOR_EQEQ_TK));
      case ASTL_OPERATOR_GE_TK:
	 return std::make_shared<Attribute>(
	    (left >= right) == (op.get_opcode() == ASTL_OPERATOR_EQEQ_TK));
      case ASTL_OPERATOR_GT_TK:
	 return std::make_shared<Attribute>(
	    (left > right) == (op.get_opcode() == ASTL_OPERATOR_EQEQ_TK));
      default:
	 assert(false); std::abort();
   }
}

} // namespace Astl
