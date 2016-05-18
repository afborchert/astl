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

#ifndef ASTL_SELECTIVE_PRINTER_H
#define ASTL_SELECTIVE_PRINTER_H

#include <iostream>
#include <string>
#include <astl/exception.hpp>
#include <astl/operator.hpp>
#include <astl/printer.hpp>
#include <astl/rule-table.hpp>
#include <astl/syntax-tree.hpp>

namespace Astl {

   bool selective_print(std::ostream& out,
      const std::string& begin_text,
      const std::string& join_text,
      const std::string& end_text,
      const NodePtr root, Operator& op,
      const RuleTable& rules) throw(Exception);

} // namespace Astl

#endif
