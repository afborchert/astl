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

#ifndef ASTL_TREELOC_H
#define ASTL_TREELOC_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>

namespace Astl {

   typedef std::shared_ptr<std::string> StringPtr;

   // necessary due to a reference cycle among header files
   class position;
   class location;

   class Position {
      public:
	 // constructors
	 Position();
	 Position(const position& pos);
	 Position(const Position& other);
	 Position(const std::string* filename_param,
	    std::size_t line_param, std::size_t column_param);

	 // accessors
	 std::size_t get_line() const;
	 std::size_t get_column() const;
	 bool is_filename_defined() const;
	 const std::string& get_filename() const;

	 // operators
	 typedef std::make_signed<std::size_t>::type Offset;
	 Position& operator+=(Offset incr);
	 Position& operator-=(Offset decr);
	 /*
	 Position operator+(Offset incr) const;
	 Position operator-(Offset decr) const;
	 */

      private:
	 std::size_t line;
	 std::size_t column;
	 bool filename_defined;
	 StringPtr filename;
   };

   class Location {
      public:
	 // constructors
	 Location();
	 Location(const location& loc);
	 Location(const Location& other);
	 Location(const Position& begin_param, const Position& end_param);

	 // accessors
	 const Position& get_begin() const;
	 const Position& get_end() const;

	 // operators
	 Location operator+(const Location& other);

      private:
	 Position begin, end;
   };

   std::ostream& operator<<(std::ostream& out, const Position& pos);
   std::ostream& operator<<(std::ostream& out, const Location& loc);

} // namespace Astl

#include <astl/location.hpp>

#endif
