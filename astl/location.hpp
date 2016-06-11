/*
   Copyright (C) 2009, 2010, 2016 Andreas Franz Borchert
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

#ifndef ASTL_LOCATION_H
#define ASTL_LOCATION_H

#include <astl/position.hh>
#include <astl/location.hh>
#include <astl/treeloc.hpp>

namespace Astl {

   /**
    * This function converts locations of the bison generated class
    * into general parser-independent locations.
    */
   inline Location make_loc(const location& loc) {
      return Location(
	 Position(
	    /* fix a bison bug where the initial position is w/o filename */
	    loc.begin.filename? loc.begin.filename: loc.end.filename,
	    loc.begin.line, loc.begin.column),
	 Position(loc.end.filename,
	    loc.end.line, loc.end.column));
   }

} // namespace Astl

#endif
