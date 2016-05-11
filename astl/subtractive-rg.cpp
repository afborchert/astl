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
#include <limits>
#include <astl/subtractive-rg.hpp>

namespace Astl {

SubtractiveRG::SubtractiveRG() {
   set_seed(1003156459);
   rewind();
} // SubtractiveRG::SubtractiveRG

SubtractiveRG::SubtractiveRG(int seedval) {
   set_seed(seedval);
   rewind();
} // SubtractiveRG::SubtractiveRG

double SubtractiveRG::val() {
   if (index >= nof_values) {
      next_values();
   }
   index += 1;
   return value[index - 1];
} // SubtractiveRG::val

unsigned int SubtractiveRG::pick(unsigned int n) {
   assert(n > 0);
   if (n == 1) {
      // deterministic case, no need to invoke val()
      return 0;
   } else {
      return val() * n;
   }
} // SubtractiveRG::pick

void SubtractiveRG::rewind() {
   double val1(seed);
   double val2(0.731);

   for(int i(0); i < nof_values; i += 1) {
      int j (21 * i % nof_values);
      value[j] = val1;
      val1 = val2 - val1;
      if (val1 < 0) {
	 val1 += 1;
      }
      val2 = value[j];
   }
   // ``warm up'' generator
   for (int i(0); i < nof_values; i += 1) {
      next_values();
   }
} // SubtractiveRG::rewind

void SubtractiveRG::set_seed(int seedval) {
   std::numeric_limits<int> intlimits;
   if (seedval == intlimits.min()) {
      seedval = intlimits.max();
   } else if (seedval < 0) {
      seedval = - seedval;
   }
   if (seedval > 1) {
      seed = 1.0 / seedval;
   } else {
      seed = 0.172;
   }
   // seed now in [0..1)
} // SubtractiveRG::set_seed

void SubtractiveRG::next_values() {
   double val;
   int i(0);
   while (i < l) {
      val = value[i] - value[i+k-l];
      if (val < 0) {
	 val += 1;
      }
      value[i] = val;
      i += 1;
   }
   while (i < k) {
      val = value[i] - value[i-l];
      if (val < 0) {
	 val += 1;
      }
      value[i] = val;
      i += 1;
   }
   index = 0;
} // SubtractiveRG::next_values

} // namespace Astl
