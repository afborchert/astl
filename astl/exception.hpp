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

#ifndef ASTL_EXCEPTION_H
#define ASTL_EXCEPTION_H

#include <exception>
#include <sstream>
#include <string>
#include <astl/treeloc.hpp>

namespace Astl {

   class Exception: public std::exception {
      private:
	 Location loc;
	 bool loc_defined;
	 std::string msg;
	 mutable std::string msgbuf; // temporarily used by what()

      public:
	 // constructors
	 Exception() : loc_defined(false) {}
	 Exception(const location& loc) : loc(loc), loc_defined(true) {}
	 Exception(const Location& loc) : loc(loc), loc_defined(true) {}
	 Exception(const std::string msg) : loc_defined(false), msg(msg) {}
	 Exception(const char* msg) : loc_defined(false), msg(msg) {}
	 Exception(const location& loc, const std::string& msg) :
	       loc(loc), loc_defined(true), msg(msg) {}
	 Exception(const Location& loc, const std::string& msg) :
	       loc(loc), loc_defined(true), msg(msg) {}
	 Exception(const location& loc, const char* msg) :
	       loc(loc), loc_defined(true), msg(msg) {}
	 Exception(const Location& loc, const char* msg) :
	       loc(loc), loc_defined(true), msg(msg) {}
	 Exception(const Exception& other) :
	       loc(other.loc), loc_defined(other.loc_defined), msg(other.msg) {
	 }

	 // destructor
	 virtual ~Exception() throw() {}

	 // mutator
	 virtual Exception& operator=(const Exception& other) {
	    loc_defined = other.loc_defined;
	    if (loc_defined) {
	       loc = other.loc;
	    }
	    msg = other.msg;
	    return *this;
	 }

	 // accessor
	 virtual const char* what() const throw() {
	    std::ostringstream os;
	    if (loc_defined) os << loc;
	    if (loc_defined && msg.size() > 0) os << ": ";
	    if (msg.size() > 0) os << msg;
	    msgbuf = os.str();
	    return msgbuf.c_str();
	 }
   }; // class Exception

} // namespace Astl

#endif
