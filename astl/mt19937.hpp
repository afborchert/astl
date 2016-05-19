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

#ifndef ASTL_MT19937_H
#define ASTL_MT19937_H

#include <cassert>
#include <random>
#include <utility>
#include <astl/prg.hpp>

namespace Astl {

   class mt19937 : public PseudoRandomGenerator {
      public:
	 mt19937() : uniform_double(0.0, 1.0) {
	 }

	 template<typename T>
	 mt19937(T&& seedval) {
	    mt.seed(std::forward<T>(seedval));
	 }

	 virtual double val() {
	    return uniform_double(mt);
	 }

	 virtual unsigned int pick(unsigned int n) {
	    assert(n > 0);
	    std::uniform_int_distribution<unsigned int> dist(0, n-1);
	    return dist(mt);
	 }

      private:
	 std::mt19937 mt;
	 std::uniform_real_distribution<double> uniform_double;
   }; // class mt19937

} // namespace Astl

#endif
