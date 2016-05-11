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

#include <astl/arithmetic-ops.hpp>
#include <astl/operators.hpp>
#include <astl/integer.hpp>
#include <gmp.h>

namespace Astl {

AttributePtr arithmetic_binary_op(const Operator& op,
      AttributePtr leftAt, AttributePtr rightAt,
      const Location& loc) throw(Exception) {
   IntegerPtr left = leftAt? leftAt->convert_to_integer(loc): 
      IntegerPtr(new Integer((long)0));
   IntegerPtr right = rightAt? rightAt->convert_to_integer(loc): 
      IntegerPtr(new Integer((long)0));
   switch (op.get_opcode()) {
      case ASTL_OPERATOR_EQEQ_TK:
      case ASTL_OPERATOR_NE_TK:
      case ASTL_OPERATOR_LT_TK:
      case ASTL_OPERATOR_LE_TK:
      case ASTL_OPERATOR_GE_TK:
      case ASTL_OPERATOR_GT_TK:
	 // comparison operators
	 {
	    int cmp = mpz_cmp(left->get_value(), right->get_value());
	    bool cmp_result;
	    switch (op.get_opcode()) {
	       case ASTL_OPERATOR_EQEQ_TK: cmp_result = cmp == 0; break;
	       case ASTL_OPERATOR_NE_TK: cmp_result = cmp != 0; break;
	       case ASTL_OPERATOR_LT_TK: cmp_result = cmp < 0; break;
	       case ASTL_OPERATOR_LE_TK: cmp_result = cmp <= 0; break;
	       case ASTL_OPERATOR_GE_TK: cmp_result = cmp >= 0; break;
	       case ASTL_OPERATOR_GT_TK: cmp_result = cmp > 0; break;
	       default:
		  assert(0);
	    }
	    return AttributePtr(new Attribute(cmp_result));
	 }
      case ASTL_OPERATOR_PLUS_TK:
      case ASTL_OPERATOR_MINUS_TK:
      case ASTL_OPERATOR_DIV_TK:
      case ASTL_OPERATOR_MOD_TK:
      case ASTL_OPERATOR_STAR_TK:
      case ASTL_OPERATOR_POWER_TK:
	 /* numerical operators */
	 {
	    Integer result;
	    mpz_t& result_value(result.get_value());
	    mpz_t& left_value(left->get_value());
	    mpz_t& right_value(right->get_value());
	    switch (op.get_opcode()) {
	       case ASTL_OPERATOR_PLUS_TK:
		  mpz_add(result_value, left_value, right_value);
		  break;
	       case ASTL_OPERATOR_MINUS_TK:
		  mpz_sub(result_value, left_value, right_value);
		  break;
	       case ASTL_OPERATOR_DIV_TK:
		  if (mpz_sgn(right_value) == 0) {
		     throw Exception(loc, "division by zero");
		  }
		  mpz_fdiv_q(result_value, left_value, right_value);
		  break;
	       case ASTL_OPERATOR_MOD_TK:
		  if (mpz_sgn(right_value) == 0) {
		     throw Exception(loc, "division by zero");
		  }
		  mpz_fdiv_r(result_value, left_value, right_value);
		  break;
	       case ASTL_OPERATOR_STAR_TK:
		  mpz_mul(result_value, left_value, right_value);
		  break;
	       case ASTL_OPERATOR_POWER_TK:
		  if (mpz_fits_ulong_p(right_value)) {
		     unsigned long int exp = mpz_get_ui(right_value);
		     mpz_pow_ui(result_value, left_value, exp);
		  } else {
		     throw Exception(loc, "exponent too large or negative");
		  }
		  break;
	       default:
		  assert(false);
	    }
	    return AttributePtr(new Attribute(IntegerPtr(
	       new Integer(result_value))));
	 }
      default:
	 assert(false);
   }
}

} // namespace Astl
