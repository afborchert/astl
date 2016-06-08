/*
   Copyright (C) 2016 Andreas Franz Borchert
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

#ifndef ASTL_ARITY_H
#define ASTL_ARITY_H

#include <cstdlib>
#include <iostream>

namespace Astl {

   struct Arity {
      Arity() : fixed(false), arity(0) {
      }
      Arity(std::size_t arity) : fixed(true), arity(arity) {
      }
      Arity(const Arity& other) : fixed(other.fixed), arity(other.arity) {
      }
      bool operator<(const Arity& other) const {
	 if (fixed != other.fixed) {
	    return fixed > other.fixed;
	 } else {
	    return arity < other.arity;
	 }
      }
      bool fixed;
      std::size_t arity;
   };

   inline std::ostream& operator<<(std::ostream& out, const Arity& arity) {
      if (arity.fixed) {
	 out << arity.arity;
      } else {
	 out << "variable";
      }
      return out;
   }

} // namespace Astl

#endif
