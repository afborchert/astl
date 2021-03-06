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

#ifndef ASTL_DEFAULT_BINDINGS_H
#define ASTL_DEFAULT_BINDINGS_H

#include <astl/exception.hpp>
#include <astl/types.hpp>

namespace Astl {

   BindingsPtr create_default_bindings();
   BindingsPtr create_default_bindings(NodePtr root);
   BindingsPtr create_default_bindings(NodePtr root, const Rules* rulesp);
   BindingsPtr create_default_bindings(NodePtr root,
      const Rules* rulesp, BindingsPtr extra_bindings);

} // namespace Astl

#endif
