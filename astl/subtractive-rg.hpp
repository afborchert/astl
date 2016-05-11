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

/**
   Pseudo random number generator based on the subtractive method from
   Knuth, Seminumerical Algorithms, 3.2.2 and 3.6;
   this RG ist used instead of rand() by the standard library
   to have reproducable sequences on different platforms
*/

#ifndef ASTL_SUBTRACTIVE_RG_H
#define ASTL_SUBTRACTIVE_RG_H

#include <astl/prg.hpp>

namespace Astl {

   class SubtractiveRG : public PseudoRandomGenerator {
      public:
	 // constructors

	 /*
	  * Description:   Default constructor that selects a constant
	  *                default for its seed value.
	  */
	 SubtractiveRG();

	 /*
	  * Description:   Constructs a pseudo random generator whose
	  *                sequence depends on the given seed value.
	  */
	 SubtractiveRG(int seedval);

	 // mutators

	 /*
	  * Name:          rewind
	  * Description:   Re-initializes the pseudo random generator to
	  *                start with the same sequence of numbers
	  *                from its begin.
	  */
	 void rewind();

	 /*
	  * Name:          val
	  * Description:   Returns the next number of the sequence
	  *                which is uniformely distributed over [0..1)
	  * Post:          The returned value is within [0..1)
	  */
	 virtual double val();

	 /*
	  * Name:          pick
	  * Description:   Takes the next value from the sequence (using val)
	  *                and maps it into the range [0..n)
	  * Pre:           n > 0
	  * Post:          The returned value is within [0..n)
	  */
	 virtual unsigned int pick(unsigned int n);

      private:
	 /*
	  * Name:          set_seed
	  * Description:   Computes the initial state for the given seed value.
	  */
	 void set_seed(int seedval);

	 /*
	  * Name:          next_values
	  * Description:   Computes the next buffer of values and stores it
	  *                into value[]
	  */
	 void next_values();

	 /*
	  * Parameters of the algorithm which must fulfill certain
	  * conditions. See Knuth's description for details.
	  */
	 static const int l = 24;
	 static const int k = 55;
	 static const int nof_values = k;

	 /*
	  * Current buffer with pseudo random numbers,
	  * value[index] .. value[nof_values-1] have not been returned yet
	  */
	 double value[nof_values];
	 int index;

	 /*
	  * Remember the seed value for rewind()
	  */
	 double seed;
   }; // class SubtractiveRG

} // namespace Astl

#endif
