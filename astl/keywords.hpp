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

#ifndef ASTL_KEYWORDS_HPP
#define ASTL_KEYWORDS_HPP

#include <map>
#include <string>

namespace Astl {

   class KeywordTable {
      public:
	 // constructor
	 KeywordTable();

	 // accessor
	 bool lookup(const std::string& ident, int& token) const;

      private:
	 std::map<std::string, int> tab;
   };

   extern KeywordTable keyword_table;

} // namespace Astl

#endif
