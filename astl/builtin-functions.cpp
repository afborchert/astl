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

#include <cassert>
#include <memory>
#include <astl/builtin-functions.hpp>

namespace Astl {

bool BuiltinFunctions::add(const std::string& name, Builtin f) {
   std::pair<Map::iterator, bool> result = functions.insert(make_pair(name, f));
   return result.second;
}

void BuiltinFunctions::insert(BindingsPtr bindings) const {
   for (Map::const_iterator it = functions.begin();
	 it != functions.end(); ++it) {
      bool ok = bindings->define(it->first,
	 std::make_shared<Attribute>(FunctionPtr(
	    std::make_shared<BuiltinFunction>(it->second, bindings)))
      );
      assert(ok);
   }
}

} // namespace Astl
