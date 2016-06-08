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

#ifndef ASTL_DESIGNATOR_H
#define ASTL_DESIGNATOR_H

#include <memory>
#include <string>
#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/exception.hpp>
#include <astl/treeloc.hpp>

namespace Astl {

   class Designator {
      public:
	 Designator(AttributePtr at);
	 Designator(BindingsPtr bindings,
	    const std::string& varname);

	 // mutators
	 void add_index(AttributePtr indexAt,
	    const Location& loc) throw(Exception);
	 void add_key(const std::string& key_param,
	    const Location& loc) throw(Exception);
	 void assign(AttributePtr value, const Location& loc) throw(Exception);
	 void delete_key(const Location& loc) throw(Exception);

	 // accessors
	 AttributePtr get_value(const Location& loc) const throw(Exception);
	 bool is_lvalue() const;
	 bool exists() const;

      private:
	 enum {simpleDesignator, selectingDesignator} type;
	 BindingsPtr bindings;
	 std::string varname;
	 AttributePtr at; // selector
	 bool lvalue; // is it a lvalue?
	 std::string key; // if at->get_type() == dictionary
	 std::size_t index; // if type == list
	 void dereference(const Location& loc);
   };
   typedef std::shared_ptr<Designator> DesignatorPtr;

} // namespace Astl

#endif
