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

#ifndef ASTL_CONFIG_H
#define ASTL_CONFIG_H

#ifdef __GNUC__

#ifndef GCC_VERSION
#define GCC_VERSION \
   (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#if GCC_VERSION >= 40600
#define HAVE_UNRESTRICTED_UNIONS 1
#endif

#endif

#endif

// #if HAVE_UNRESTRICTED_UNIONS
// #define USE_UNRESTRICTED_UNIONS
// #else
#define USE_FLAT_UNIONS
// #endif

#endif
