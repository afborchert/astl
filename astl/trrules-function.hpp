/*
   Copyright (C) 2010 Andreas Franz Borchert
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

#ifndef ASTL_TRRULES_FUNCTION_H
#define ASTL_TRRULES_FUNCTION_H

#include <astl/types.hpp>
#include <astl/function.hpp>
#include <astl/rule-table.hpp>

namespace Astl {

   class TransformationRuleSetFunction: public Function {
      public:
	 TransformationRuleSetFunction(RuleTablePtr rt_param,
	    BindingsPtr bindings_param);
	 virtual AttributePtr eval(AttributePtr args) const throw(Exception);
      private:
	 RuleTablePtr rt;
   };

   class InplaceTransformationRuleSetFunction: public Function {
      public:
	 InplaceTransformationRuleSetFunction(RuleTablePtr rt_param,
	    BindingsPtr bindings_param);
	 virtual AttributePtr eval(AttributePtr args) const throw(Exception);
      private:
	 RuleTablePtr rt;
   };

} // namespace Astl

#endif
