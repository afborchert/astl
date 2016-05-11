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

#include <astl/scanner.hpp>
#include <astl/parser.hpp>
#include <astl/keywords.hpp>

namespace Astl {

static struct {
   char const* keyword;
   int token;
} keywords[] = {
   {"abstract", parser::token::ABSTRACT},
   {"and", parser::token::AND},
   {"as", parser::token::AS},
   {"at", parser::token::AT},
   {"attribution", parser::token::ATTRIBUTION},
   {"cache", parser::token::CACHE},
   {"create", parser::token::CREATE},
   {"close", parser::token::CLOSE},
   {"cut", parser::token::CUT},
   {"delete", parser::token::DELETE},
   {"div", parser::token::DIV},
   {"else", parser::token::ELSE},
   {"elsif", parser::token::ELSIF},
   {"exists", parser::token::EXISTS},
   {"foreach", parser::token::FOREACH},
   {"if", parser::token::IF},
   {"import", parser::token::IMPORT},
   {"in", parser::token::IN},
   {"inplace", parser::token::INPLACE},
   {"left", parser::token::LEFT},
   {"library", parser::token::LIBRARY},
   {"machine", parser::token::MACHINE},
   {"mod", parser::token::MOD},
   {"nonassoc", parser::token::NONASSOC},
   {"null", parser::token::NULL_T},
   {"on", parser::token::ON},
   {"operators", parser::token::OPERATORS},
   {"opset", parser::token::OPSET},
   {"or", parser::token::OR},
   {"post", parser::token::POST},
   {"pre", parser::token::PRE},
   {"print", parser::token::PRINT},
   {"private", parser::token::PRIVATE},
   {"retract", parser::token::RETRACT},
   {"right", parser::token::RIGHT},
   {"return", parser::token::RETURN},
   {"rules", parser::token::RULES},
   {"shared", parser::token::SHARED},
   {"state", parser::token::STATE},
   {"sub", parser::token::SUB},
   {"transformation", parser::token::TRANSFORMATION},
   {"var", parser::token::VAR},
   {"where", parser::token::WHERE},
   {"when", parser::token::WHEN},
   {"while", parser::token::WHILE},
   {"x", parser::token::X},
};

KeywordTable::KeywordTable() {
   for (int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); ++i) {
      tab[keywords[i].keyword] = keywords[i].token;
   }
}

bool KeywordTable::lookup(const std::string& ident, int& token) const {
   std::map<std::string, int>::const_iterator it = tab.find(ident);
   if (it == tab.end()) {
      return false;
   } else {
      token = it->second;
      return true;
   }
}

KeywordTable keyword_table;

} // namespace Astl
