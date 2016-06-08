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
#include <iostream>
#include <memory>
#include <string>
#include <astl/treeloc.hpp>

namespace Astl {

// implementation of Position ===============================================

static StringPtr get_string(const std::string* sp) {
   static bool s_defined = false;
   static std::string last_s;
   static StringPtr last_ptr;
   if (sp) {
      if (!s_defined || *sp != last_s) {
	 last_ptr = std::make_shared<std::string>(*sp);
	 last_s = *sp;
	 s_defined = true;
      }
      return last_ptr;
   } else {
      return StringPtr(nullptr);
   }
}

// constructors -------------------------------------------------------------

Position::Position() : line(0), column(0), filename_defined(false) {};

Position::Position(const position& pos) :
      line(pos.line), column(pos.column),
      filename_defined(pos.filename != 0),
      filename(get_string(pos.filename)) {
}

Position::Position(const std::string* filename_param,
      std::size_t line_param, std::size_t column_param) :
      line(line_param), column(column_param),
      filename_defined(filename_param != 0),
      filename(get_string(filename_param)) {
}

Position::Position(const Position& other) :
      line(other.line), column(other.column),
      filename_defined(other.filename_defined),
      filename(other.filename) {
}

// accessors ----------------------------------------------------------------

std::size_t Position::get_line() const {
   return line;
}

std::size_t Position::get_column() const {
   return column;
}

bool Position::is_filename_defined() const {
   return filename_defined;
}

const std::string& Position::get_filename() const {
   assert(filename_defined);
   return *filename;
}

// operators ----------------------------------------------------------------

Position& Position::operator+=(Offset incr) {
   assert((Offset) column + incr >= 1);
   column = (Offset) column + incr;
   return *this;
}

Position& Position::operator-=(Offset decr) {
   assert((Offset) column - decr >= 1);
   column = (Offset) column - decr;
   return *this;
}

/*
Position Position::operator+(Offset incr) const {
   return *this + incr;
}

Position Position::operator-(Offset decr) const {
   return *this - decr;
}
*/

// implementation of Location ===============================================

// constructors -------------------------------------------------------------

Location::Location() {};

Location::Location(const location& loc) :
      begin(loc.begin), end(loc.end) {
}

Location::Location(const Position& begin_param, const Position& end_param) :
      begin(begin_param), end(end_param) {
}

Location::Location(const Location& other) :
      begin(other.begin), end(other.end) {
}

// accessors ----------------------------------------------------------------

const Position& Location::get_begin() const {
   return begin;
}

const Position& Location::get_end() const {
   return end;
}

// operators ----------------------------------------------------------------

Location Location::operator+(const Location& other) {
   Location loc(*this);
   loc.end = other.end;
   return loc;
}

// I/O operators ============================================================

/* both operators follow the conventions of position.hh and
   location.hh of the Bison implementation */

std::ostream& operator<<(std::ostream& out, const Position& pos) {
   if (pos.is_filename_defined()) {
      out << pos.get_filename() << ':';
   }
   return out << pos.get_line() << '.' << pos.get_column();
}

std::ostream& operator<<(std::ostream& out, const Location& loc) {
   const Position& first(loc.get_begin());
   Position last = loc.get_end();
   out << first;
   if (last.get_column() > 1) last -= 1;
   if (last.is_filename_defined() &&
	 (!first.is_filename_defined() ||
	    first.get_filename() != last.get_filename())) {
      out << '-' << last;
   } else if (first.get_line() != last.get_line()) {
      out << '-' << last.get_line() << '.' << last.get_column();
   } else if (first.get_column() != last.get_column()) {
      out << '-' << last.get_column();
   }
   return out;
}

} // namespace Astl
