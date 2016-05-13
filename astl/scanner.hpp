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

#ifndef ASTL_SCANNER_H
#define ASTL_SCANNER_H

#include <iostream>
#include <list>
#include <astl/parser.hpp>
#include <astl/location.hpp>

namespace Astl {

   typedef parser::semantic_type semantic_type;

   class Scanner {
      public:
	 Scanner(std::istream& in, const std::string& input_name);

	 // mutators
	 int get_token(semantic_type& yylval, location& yylloc);

      private:
	 std::istream& in;
	 std::string input_name;
	 unsigned char ch;
	 bool eof;
	 position oldpos, pos;
	 location tokenloc;
	 std::string* tokenstr;

	 struct TokenItem {
	    int token;
	    semantic_type yylval;
	    location yylloc;
	 };
	 std::list<TokenItem> token_buffer;

	 // private mutators
	 void nextch();
	 void error(char const* msg);
	 void scan_text();
	 void scan_regexp(char opening_delimiter, char closing_delimiter);
	 void scan_string_literal(semantic_type& yylval, int& token);
	 bool get_next_token(semantic_type& yylval,
	    location& yylloc, int& token);
	 void push_token(int token, semantic_type yylval, location yylloc);
   };

   inline int yylex(semantic_type* yylval, location* yylloc, Scanner& scanner) {
      return scanner.get_token(*yylval, *yylloc);
   }

   bool is_letter(char ch);
   bool is_digit(char ch);
   bool is_whitespace(char ch);

} // namespace Astl

#endif
