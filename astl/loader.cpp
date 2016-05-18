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
#include <fstream>
#include <iostream>
#include <astl/loader.hpp>
#include <astl/parser.hpp>
#include <astl/scanner.hpp>

namespace Astl {

Loader::Loader() {
   libpath.push_back(".");
}

bool Loader::loaded(const std::string& name) const {
   return loaded_libs.find(name) != loaded_libs.end();
}

NodePtr Loader::load(const std::string& name) throw(Exception) {
   std::ifstream in;
   std::string path;
   if (name[0] == '/') {
      in.open(name.c_str());
      path = name;
   } else {
      for (std::list<std::string>::const_iterator it = libpath.begin();
	    it != libpath.end(); ++it) {
	 path = *it + "/" + name;
	 in.clear(); in.open(path.c_str());
	 if (!in) {
	    path += ".ast";
	    in.clear();
	    in.open(path.c_str());
	 }
	 if (in) break;
      }
   }
   if (in) {
      Scanner scanner(in, path);
      NodePtr root;
      parser p(scanner, root);
      if (p.parse() != 0) {
	 throw Exception("unable to parse '" + path + "'");
      }
      loaded_libs.insert(name);
      return root;
   }
   throw Exception("unable to load Astl source '" + name + "'");
}

void Loader::add_library(const std::string& libname) {
   assert(libname.length() > 0);
   libpath.push_back(libname);
}

void Loader::add_library_in_front(const std::string& libname) {
   assert(libname.length() > 0);
   libpath.push_front(libname);
}

} // namespace Astl
