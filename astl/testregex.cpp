/*
   Copyright (C) 2016 Andreas F. Borchert
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <astl/attribute.hpp>
#include <astl/exception.hpp>
#include <astl/location.hpp>
#include <astl/regex.hpp>

using namespace Astl;

int main(int argc, char** argv) {
   if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << " pattern" << std::endl;
      exit(1);
   }
   std::string pattern(argv[1]);

   Regex regex;
   Location loc;
   try {
      regex.compile(loc, pattern);
   } catch(Exception& exception) {
      std::cerr << exception.what() << std::endl;
      exit(1);
   }
   
   std::string line;
   while (std::getline(std::cin, line)) {
      AttributePtr res = regex.match(line);
      if (res == nullptr) continue;
      std::cout << res << std::endl;
   }
}
