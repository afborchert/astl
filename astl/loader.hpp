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

#ifndef ASTL_LOADER_H
#define ASTL_LOADER_H

#include <list>
#include <set>
#include <string>
#include <astl/exception.hpp>
#include <astl/syntax-tree.hpp>

namespace Astl {

   class Loader {
      public:
	 Loader();

	 // accessors
	 bool loaded(const std::string& name) const;

	 // mutators
	 NodePtr load(std::string name);
	 void add_library(const std::string& libname);
	 void add_library_in_front(const std::string& libname);

      private:
	 std::set<std::string> loaded_libs; // set of loaded libraries
	 std::list<std::string> libpath;
   };

} // namespace Astl

#endif
