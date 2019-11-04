/*
   Copyright (C) 2009-2019 Andreas Franz Borchert
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
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <astl/generator.hpp>
#include <astl/loader.hpp>
#include <astl/location.hpp>
#include <astl/operators.hpp>
#include <astl/parser.hpp>
#include <astl/run.hpp>
#include <astl/scanner.hpp>
#include <astl/yytname.hpp>

using namespace std;
using namespace Astl;

class SyntaxTreeGeneratorForAstl: public SyntaxTreeGenerator {
   public:
      virtual NodePtr gen(int& argc, char**& argv) {
	 if (argc == 0) {
	    throw Exception("no source file given");
	 }
	 char* source_name = *argv++; --argc;
	 ifstream source(source_name);
	 if (!source) {
	    std::ostringstream os;
	    os << "unable to open " << source_name;
	    throw Exception(os.str());
	 }
	 Scanner scanner(source, source_name);
	 NodePtr root;
	 parser p(scanner, root);
	 if (p.parse() != 0) {
	    std::ostringstream os;
	    os << "parsing of " << source_name << " failed";
	    throw Exception(os.str());
	 }
	 return root;
      }
};

int main(int argc, char** argv) {
   try {
      SyntaxTreeGeneratorForAstl astgen;
      Loader loader;
      char* path = getenv("ASTL_ASTL_PATH");
      if (path) {
	 char* dir = path;
	 for (char* cp = path; *cp; ++cp) {
	    if (*cp == ':') {
	       if (dir) {
		  * cp = 0;
		  loader.add_library(dir);
		  dir = 0;
	       }
	    } else if (!dir) {
	       dir = cp;
	    }
	 }
	 if (dir) loader.add_library(dir);
      } else {
	 // default path
#ifdef ASTL_ASTL_LIBDIR
	 loader.add_library(ASTL_ASTL_LIBDIR);
#else
	 loader.add_library("/usr/local/share/astl/astl");
	 loader.add_library("/usr/share/astl/astl");
#endif
      }
      run(argc, argv, astgen, loader, Op::LPAREN);
   } catch (Exception& e) {
      cout << endl;
      cerr << e.what() << endl;
   } catch (std::exception& e) {
      cout << endl;
      cerr << e.what() << endl;
   }
}

/*

=head1 NAME

astl-astl -- run Astl scripts for Astl programs

=head1 SYNOPSIS

B<astl-astl> F<astl-script> F<astl-program> [I<args>]

=head1 DESCRIPTION

Astl scripts can be started explicitly using the B<astl-astl> interpreter
or implicitly using a shebang line in the first line of the script:

   #!/usr/bin/env astl-astl

The construct using F</usr/bin/env> attempts to find F<astl-astl>
anywhere in the user's path.

Once the abstract syntax tree is present, the Astl script is
loaded and executed following the execution order defined
in section 12.5 of the Report of the Astl Programming Language.

All arguments behind the text file are put into a list
and bound to the variable I<args> in the I<main> function.

=head1 ENVIRONMENT

The environment variable I<ASTL_ASTL_PATH> can be used to specify
a colon-separated list of directories where library modules
are looked for. By default, F</usr/local/share/astl/astl> and
F</usr/share/astl/astl> are used. The script itself can add more
directories to the library search path using the Astl
library clause (see section 12.1 in the I<Report of the
Astl Programming Language>).

=head1 AUTHOR

Andreas F. Borchert

=cut

*/
