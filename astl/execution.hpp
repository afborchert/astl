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

#ifndef ASTL_EXECUTION_H
#define ASTL_EXECUTION_H

#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/candidate-set.hpp>
#include <astl/exception.hpp>
#include <astl/rule-table.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/tree-expressions.hpp>

namespace Astl {

   AttributePtr execute(NodePtr statements, BindingsPtr bindings)
      throw(Exception);
   void execute(NodePtr root, const RuleTable& rules) throw(Exception);
   void execute(NodePtr root, const RuleTable& rules,
      BindingsPtr bindings) throw(Exception);
   void execute(const CandidateSet& candidates) throw(Exception);

} // namespace Astl

#endif
