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
#include <cstdlib>
#include <memory>
#include <sstream>
#include <astl/designator.hpp>
#include <astl/flow-graph.hpp>

namespace Astl {

Designator::Designator(AttributePtr at) :
      type(selectingDesignator), at(at), lvalue(false) {
   assert(at);
}

Designator::Designator(BindingsPtr bindings,
	 const std::string& varname) :
      type(simpleDesignator), bindings(bindings),
      varname(varname), lvalue(true) {
   assert(bindings->defined(varname));
}

void Designator::dereference(const Location& loc) {
   at = get_value(loc);
   type = selectingDesignator;
   varname = "";
   lvalue = false;
}

void Designator::add_index(AttributePtr indexAt,
      const Location& loc) {
   dereference(loc);
   if (!at) {
      throw Exception(loc, "cannot index null value");
   }
   Attribute::Type attype = at->get_type();
   unsigned long indexVal = indexAt?
      indexAt->convert_to_integer(loc)->get_unsigned_int(loc): 0;
   switch (attype) {
      case Attribute::list:
	 if (indexVal >= at->size()) {
	    throw Exception(loc, "index out of range");
	 }
	 index = indexVal;
	 lvalue = true;
	 break;

      case Attribute::match_result:
	 if (indexVal >= at->size()) {
	    throw Exception(loc, "index out of range");
	 }
	 at = at->get_value(indexVal);
	 lvalue = false;
	 break;

      case Attribute::tree: {
	    NodePtr node = at->get_node();
	    if (node->is_leaf()) {
	       throw Exception(loc, "a leaf node cannot be indexed");
	    }
	    if (indexVal >= node->size()) {
	       throw Exception(loc, "index out of range");
	    }
	    at = std::make_shared<Attribute>(node->get_operand(indexVal));
	 }
	 break;

      default:
	 throw Exception(loc, "is not a list");
   }
}

void Designator::add_key(const std::string& key_param,
      const Location& loc) {
   dereference(loc);
   if (!at) {
      throw Exception(loc, "null cannot be used as dictionary");
   }
   if (at->get_type() == Attribute::tree) {
      at = at->get_node()->get_attribute();
   } else if (at->get_type() == Attribute::flow_graph_node) {
      at = at->get_fgnode()->get_attribute();
   }
   if (at->get_type() != Attribute::dictionary) {
      throw Exception(loc, "is not a dictionary");
   }
   key = key_param; lvalue = true;
}

bool Designator::exists() const {
   switch (type) {
      case simpleDesignator:
	 return true;

      case selectingDesignator:
	 if (!lvalue) return true;
	 switch (at->get_type()) {
	    case Attribute::dictionary:
	       return at->is_defined(key);

	    case Attribute::list:
	       return index < at->size();

	    default:
	       assert(false); std::abort();
	 }

      default:
	 assert(false); std::abort();
   }
}

void Designator::assign(AttributePtr value,
      const Location& loc) {
   assert(lvalue);
   if (varname != "" && bindings->is_const(varname)) {
      throw Exception(loc, "constants must not be assigned to");
   }
   switch (type) {
      case simpleDesignator:
	 bindings->update(varname, value);
	 break;

      case selectingDesignator:
	 switch (at->get_type()) {
	    case Attribute::dictionary:
	       at->update(key, value);
	       break;

	    case Attribute::list:
	       at->update(index, value);
	       break;

	    default:
	       assert(false); std::abort();
	 }
	 break;
   }
}

void Designator::delete_key(const Location& loc) {
   assert(lvalue);
   if (varname != "" && bindings->is_const(varname)) {
      throw Exception(loc, "constants must not be changed");
   }
   switch (type) {
      case selectingDesignator:
	 switch (at->get_type()) {
	    case Attribute::dictionary:
	       at->delete_key(key);
	       break;

	    default:
	       throw Exception(loc, "reference to dictionary member expected");
	 }
	 break;

      default:
	 throw Exception(loc, "reference to dictionary member expected");
   }
}

AttributePtr Designator::get_value(const Location& loc) const {
   switch (type) {
      case simpleDesignator:
	 return bindings->get(varname);

      case selectingDesignator:
	 if (!lvalue) return at;
	 switch (at->get_type()) {
	    case Attribute::dictionary:
	       if (at->is_defined(key)) {
		  return at->get_value(key);
	       } else {
		  throw Exception(loc, "no such key");
	       }

	    case Attribute::list:
	       return at->get_value(index);

	    default:
	       assert(false); std::abort();
	 }

      default:
	 assert(false); std::abort();
   }
}

bool Designator::is_lvalue() const {
   return lvalue;
}

} // namespace Astl
