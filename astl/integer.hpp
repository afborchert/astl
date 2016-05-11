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

#ifndef ASTL_INTEGER_H
#define ASTL_INTEGER_H

#include <gmp.h>
#include <string>
#include <memory>
#include <astl/treeloc.hpp>
#include <astl/exception.hpp>

namespace Astl {

   class Integer {
      public:
	 // constructors
	 Integer() {
	    mpz_init(value);
	 }
	 Integer(const Integer& other) {
	    mpz_init(value);
	    mpz_set(value, other.value);
	 }
	 Integer(int ival) {
	    mpz_init(value);
	    mpz_set_si(value, ival);
	 }
	 Integer(unsigned int ival) {
	    mpz_init(value);
	    mpz_set_ui(value, ival);
	 }
	 Integer(long int ival) {
	    mpz_init(value);
	    mpz_set_si(value, ival);
	 }
	 Integer(unsigned long int ival) {
	    mpz_init(value);
	    mpz_set_ui(value, ival);
	 }
	 Integer(const mpz_t& val) {
	    mpz_init(value);
	    mpz_set(value, val);
	 }
	 Integer(const char* s, const Location& loc) throw(Exception) {
	    mpz_init(value);
	    if (mpz_set_str(value, s, 10) < 0) {
	       throw Exception(loc, "conversion to integer failed");
	    }
	 }
	 Integer(std::string& s, const Location& loc) throw(Exception) {
	    mpz_init(value);
	    if (mpz_set_str(value, s.c_str(), 10) < 0) {
	       throw Exception(loc, "conversion to integer failed");
	    }
	 }
	 // destructor
	 ~Integer() {
	    mpz_clear(value);
	 }
	 // mutators
	 Integer& operator=(const Integer& other) {
	    mpz_set(value, other.value);
	    return *this;
	 }
	 Integer& operator+=(const Integer& other) {
	    mpz_add(value, value, other.value);
	    return *this;
	 }
	 Integer& operator-=(const Integer& other) {
	    mpz_sub(value, value, other.value);
	    return *this;
	 }
	 Integer& operator*=(const Integer& other) {
	    mpz_mul(value, value, other.value);
	    return *this;
	 }
	 Integer& div(const Integer& other) {
	    mpz_fdiv_q(value, value, other.value);
	    return *this;
	 }
	 Integer& mod(const Integer& other) {
	    mpz_fdiv_r(value, value, other.value);
	    return *this;
	 }
	 Integer& power(const Integer& other,
	       const Location& loc) throw(Exception) {
	    if (mpz_fits_ulong_p(other.value)) {
	       unsigned long int exp = mpz_get_ui(other.value);
	       mpz_pow_ui(value, value, exp);
	    } else {
	       throw Exception(loc, "exponent too large or negative");
	    }
	    return *this;
	 }
	 Integer& neg() {
	    mpz_neg(value, value);
	    return *this;
	 }
	 Integer& abs() {
	    mpz_abs(value, value);
	    return *this;
	 }
	 mpz_t& get_value() {
	    return value;
	 }
	 // accessors
	 unsigned long int get_unsigned_int(const Location& loc) const
	       throw(Exception) {
	    if (mpz_fits_ulong_p(value)) {
	       return mpz_get_ui(value);
	    } else {
	       throw Exception(loc, "integer is too large or negative");
	    }
	 }
	 std::string to_string() const {
	    char* result_string = mpz_get_str(0, 10, value);
	    std::string rval(result_string);
	    delete result_string;
	    return rval;
	 }
	 bool to_bool() const {
	    return mpz_sgn(value) != 0;
	 }
      private:
	 mpz_t value;
   };
   typedef std::shared_ptr<Integer> IntegerPtr;

} // namespace Astl

#endif
