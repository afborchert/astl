/*
   Copyright (C) 2019 Andreas F. Borchert
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

#ifndef ASTL_STREAM_HPP
#define ASTL_STREAM_HPP

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <astl/exception.hpp>
#include <astl/types.hpp>

namespace Astl {

/* Stream can be instantiated as OutputStream using T = std::ostream
   or as InputStream using T = std::istream;
   unfortunately we cannot have an iostream-based polymorphic stream */
template<typename T>
class Stream {
   public:
      Stream() : working(false), foreign(nullptr) {
      }

      /* associate stream object with an already existing stream
	 that is not owned by this class */
      Stream(T& s, std::string name) : working(true), name(name), foreign(&s) {
      }

      /* pass ownership of an opened stream to an object of this class */
      Stream(std::unique_ptr<T> s, std::string name) :
	    working(s), name(name), owned(std::move(s)), foreign(nullptr) {
      }

      T& get() {
	 if (!working) {
	    throw Exception("accessing uninitialized stream");
	 }
	 if (foreign) {
	    return *foreign;
	 } else {
	    return *owned;
	 }
      }

      const std::string& get_name() {
	 return name;
      }

      bool good() {
	 return working && get().good();
      }

   private:
      bool working;
      /* name associated with this stream, usually the filename */
      std::string name;
      /* stream that is owned by us */
      std::unique_ptr<T> owned;
      /* stream which is not our resource */
      T* foreign;
};

} // namespace Astl

#endif
