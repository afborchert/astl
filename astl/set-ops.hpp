/*
   Copyright (C) 2011 Andreas Franz Borchert
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

#ifndef ASTL_SET_OPS_H
#define ASTL_SET_OPS_H

#include <astl/attribute.hpp>
#include <astl/exception.hpp>
#include <astl/operator.hpp>
#include <astl/operators.hpp>
#include <astl/treeloc.hpp>

namespace Astl {

   /**
    * Perform an set binary operator op on the operands
    * leftAt and rightAt and return its result.
    */
   AttributePtr set_binary_op(const Operator& op,
      AttributePtr leftAt, AttributePtr rightAt, const Location& loc);

} // namespace Astl

#endif
