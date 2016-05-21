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

#include <cassert>
#include <astl/bindings.hpp>
#include <astl/exception.hpp>

namespace Astl {

Bindings::Bindings() : it_defined(false), rules(nullptr) {
}

Bindings::Bindings(const Rules* rulesp) : it_defined(false), rules(rulesp) {
}

Bindings::Bindings(BindingsPtr outer_scope) :
      it_defined(false), uplink(outer_scope), rules(outer_scope->rules) {
}

bool Bindings::define(const std::string& name, AttributePtr value) {
   std::pair<Map::iterator, bool> result = vars.insert(make_pair(name, value));
   if (result.second) {
      it = result.first; it_defined = true;
      constness[name] = false;
   }
   return result.second;
}

bool Bindings::update(const std::string& name, AttributePtr value) {
   Map::iterator vit = vars.find(name);
   if (vit == vars.end()) {
      if (uplink) {
	 return uplink->update(name, value);
      } else {
	 return false;
      }
   }
   assert(!constness[name]);
   vit->second = value;
   return true;
}

bool Bindings::merge(BindingsPtr bindings) {
   for (Map::const_iterator vit = bindings->vars.begin();
	 vit != bindings->vars.end(); ++vit) {
      if (!define(vit->first, vit->second)) {
	 return false;
      }
   }
   return true;
}

void Bindings::mk_const(const std::string& name) {
   assert(defined(name));
   constness[name] = true;
}

void Bindings::mk_all_const() {
   for (Map::const_iterator vit = vars.begin();
	 vit != vars.end(); ++vit) {
      constness[vit->first] = true;
   }
}

bool Bindings::is_const(const std::string& name) const {
   std::map<std::string, bool>::const_iterator cit = constness.find(name);
   if (cit == constness.end()) {
      assert(uplink);
      return uplink->is_const(name);
   }
   return cit->second;
}

bool Bindings::defined(const std::string& name) const {
   if (it_defined && it->first == name) {
      return true;
   }
   Map::const_iterator find_it = vars.find(name);
   if (find_it == vars.end()) {
      if (uplink) {
	 return uplink->defined(name);
      } else {
	 return false;
      }
   }
   it = find_it; it_defined = true;
   return true;
}

AttributePtr Bindings::get(const std::string& name) const {
   if (!it_defined || it->first != name) {
      Map::const_iterator find_it = vars.find(name);
      if (find_it == vars.end() && uplink) {
	 return uplink->get(name);
      }
      assert(find_it != vars.end());
      it = find_it; it_defined = true;
   }
   return it->second;
}

bool Bindings::rules_defined() const {
   return rules != nullptr;
}

const Rules& Bindings::get_rules() const {
   assert(rules);
   return *rules;
}

std::ostream& operator<<(std::ostream& out, BindingsPtr bindings) {
   out << "bindings {" << std::endl;
   for (Bindings::Map::const_iterator it = bindings->vars.begin();
	 it != bindings->vars.end(); ++it) {
      out << "   " << it->first << " -> " << it->second << std::endl;
   }
   if (bindings->uplink) {
      out << "   uplink: " << bindings->uplink;
   }
   out << "}" << std::endl;
   return out;
}

} // namespace Astl
