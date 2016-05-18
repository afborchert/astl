/*
   Copyright (C) 2009, 2016 Andreas F. Borchert
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

#ifndef ASTL_OPERATOR_H
#define ASTL_OPERATOR_H

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <astl/token.hpp>

namespace Astl {

   class Operator {
      public:
	 // constructors
	 Operator() : opcode(0), opname(nullptr), opstring() {}
	 Operator(unsigned int opcode, const char* opname) :
	       opcode(opcode), opname(opname) {
	    assert(opname != nullptr); assert(opcode != 0);
	 }
	 Operator(const std::string& opname) :
	       opcode(0), opname(nullptr), opstring(opname) {
	    assert(!opname.empty());
	 }
	 Operator(const Operator& other) :
	       opcode(other.opcode), opname(other.opname),
	       opstring(other.opstring) {
	 }

	 // accessors
	 unsigned int get_opcode() const {
	    return opcode;
	 }
	 const char* get_name() const {
	    if (opname) {
	       return opname;
	    } else {
	       return opstring.c_str();
	    }
	 }
	 bool operator==(const Operator& other) const {
	    if (opcode > 0 && other.opcode > 0) {
	       return opcode == other.opcode;
	    } else {
	       return std::strcmp(get_name(), other.get_name()) == 0;
	    }
	 }
	 bool operator==(const Token& other) const {
	    if (opname) {
	       return std::strcmp(opname, other.get_text().c_str()) == 0;
	    } else {
	       return opstring == other.get_text();
	    }
	 }
	 bool operator==(const std::string& other) const {
	    if (opname) {
	       return std::strcmp(opname, other.c_str()) == 0;
	    } else {
	       return opstring == other;
	    }
	 }
	 bool operator!=(const Operator& other) const {
	    return !(*this == other);
	 }
	 bool operator!=(const Token& other) const {
	    return !(*this == other);
	 }
	 bool operator!=(const std::string& other) const {
	    return !(*this == other);
	 }

	 // mutators
	 Operator& operator=(const Operator& other) {
	    opcode = other.opcode;
	    opname = other.opname;
	    opstring = other.opstring;
	    return *this;
	 }

      private:
	 unsigned int opcode;
	 const char* opname;
	 std::string opstring;
   };

   inline std::ostream& operator<<(std::ostream& out, const Operator& op) {
      return out << op.get_name();
   }

} // namespace Astl

#endif
