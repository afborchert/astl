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

#ifndef ASTL_BUILTIN_FUNCTIONS_H
#define ASTL_BUILTIN_FUNCTIONS_H

#include <map>
#include <string>
#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/types.hpp>

namespace Astl {

   class BuiltinFunctions {
      public:
	 bool add(const std::string& name, Builtin f);
	 void insert(BindingsPtr bindings) const;

      private:
	 typedef std::map<std::string, Builtin> Map;
	 Map functions;
   };

} // namespace Astl

#endif
