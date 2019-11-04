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

#ifndef ASTL_BINDINGS_H
#define ASTL_BINDINGS_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <astl/attribute.hpp>
#include <astl/types.hpp>

namespace Astl {

   class Bindings;
   typedef std::shared_ptr<Bindings> BindingsPtr;

   class Bindings: public std::enable_shared_from_this<Bindings> {
      public:
	 Bindings();
	 Bindings(const Rules* rulesp);
	 Bindings(BindingsPtr outer_scope);
	 bool define(const std::string& name, AttributePtr value);
	 bool update(const std::string& name, AttributePtr value);
	 bool merge(BindingsPtr bindings);
	 void mk_const(const std::string& name);
	 void mk_all_const();
	 bool defined(const std::string& name) const;
	 bool is_const(const std::string& name) const;
	 AttributePtr get(const std::string& name) const;
	 bool rules_defined() const;
	 const Rules& get_rules() const;

      private:
	 friend std::ostream& operator<<(std::ostream& out,
	    BindingsPtr bindings);
	 typedef std::map<std::string, AttributePtr> Map;
	 Map vars;
	 mutable Map::const_iterator it;
	 mutable bool it_defined;
	 BindingsPtr uplink;
	 std::map<std::string, bool> constness;
	 const Rules* rules;
   };

   std::ostream& operator<<(std::ostream& out, BindingsPtr bindings);

} // namespace Astl

#endif
