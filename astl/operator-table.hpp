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

#ifndef ASTL_OPERATOR_TABLE_H
#define ASTL_OPERATOR_TABLE_H

#include <cstdlib>
#include <map>
#include <string>
#include <astl/exception.hpp>
#include <astl/operator.hpp>
#include <astl/types.hpp>

namespace Astl {

   class OperatorTable {
      public:
	 typedef std::size_t Rank;
	 typedef enum {left, right, nonassoc} Associativity;
	 // constructors
	 OperatorTable();
	 OperatorTable(NodePtr root, const Rules& rules);

	 // mutators
	 void scan(NodePtr root, const Rules& rules) throw(Exception);

	 // accessors
	 bool included(const Operator& op) const;
	 Rank get_rank(const Operator& op) const;
	 Associativity get_associativity(const Operator& op) const;

      private:
	 struct Entry {
	    std::string op;
	    Rank rank;
	    Associativity associativity;
	 };
	 typedef std::map<std::string, Entry>::value_type value_type;
	 typedef std::map<std::string, Entry>::const_iterator iterator;
	 bool access(const Operator& op) const;
	 mutable const char* last_opname;
	 mutable iterator it;
	 std::map<std::string, Entry> tab;
   };

} // namespace Astl

#endif
