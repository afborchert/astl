/*
   Copyright (C) 2009-2010 Andreas Franz Borchert
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

#ifndef ASTL_OPSET_H
#define ASTL_OPSET_H

#include <memory>
#include <set>
#include <string>
#include <astl/exception.hpp>
#include <astl/operator.hpp>
#include <astl/types.hpp>

namespace Astl {

   class OperatorSet {
      public:
	 typedef std::set<std::string>::const_iterator Iterator;

	 // constructor
	 OperatorSet(NodePtr opset_expr, const Rules& rules);

	 // accessors
	 unsigned int get_card() const;
	 bool includes(const std::string& opname) const;
	 bool includes(const Operator& op) const;
	 Iterator begin() const;
	 Iterator end() const;

      private:
	 std::set<std::string> opset;
	 void traverse(NodePtr opset_expr, const Rules& rules);
	 void add(NodePtr node, const Rules& rules);
   };
   typedef std::shared_ptr<OperatorSet> OperatorSetPtr;

} // namespace Astl

#endif
