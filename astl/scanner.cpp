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
#include <utility>
#include <astl/error.hpp>
#include <astl/keywords.hpp>
#include <astl/regex.hpp>
#include <astl/scanner.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/token.hpp>
#include <astl/utf8.hpp>

namespace Astl {

const unsigned int TAB_STOP = 8;

// constructor ===============================================================

Scanner::Scanner(std::istream& in, const std::string& input_name) :
      in(in), input_name(input_name), ch(0), codepoint(0),
      eof_seen(false), eof(false), tokenstr(nullptr) {
   pos.initialize(&this->input_name);
   next_ch(); next_codepoint();
   if (!eof && codepoint == '#') {
      // skip #! line
      next_codepoint();
      if (eof || codepoint != '!') {
	 error("#! line expected");
      }
      while (!eof && codepoint != '\n') {
	 next_codepoint();
      }
   }
}

// mutator ===================================================================

int Scanner::get_token(semantic_type& yylval, location& yylloc) {
   int token;
   do {
      if (token_buffer.size() > 0) {
	 TokenItem t = token_buffer.front(); token_buffer.pop_front();
	 yylval = t.yylval; yylloc = t.yylloc;
	 return t.token;
      }
   } while (!get_next_token(yylval, yylloc, token));
   return token;
}

bool Scanner::get_next_token(semantic_type& yylval,
      location& yylloc, int& token) {
   token = 0;
   yylval = NodePtr(nullptr);
   for(;;) {
      if (eof) {
	 break;
      } else if (is_whitespace(codepoint)) {
	 next_codepoint();
      } else {
	 break;
      }
   }
   tokenloc.begin = oldpos;
   if (is_letter(codepoint)) {
      char32_t initial_letter = codepoint;
      tokenstr = std::make_unique<std::string>();
      next_codepoint();
      if (codepoint == '{' && (initial_letter == 'm' || initial_letter == 'q')) {
	 /* Perl-style literals m{regexp} and q{string} */
	 tokenstr = nullptr;
	 switch (initial_letter) {
	    case 'm':
	       scan_regexp('{', '}'); break;
	    case 'q':
	       scan_text(); break;
	    default:
	       assert(false); std::abort();
	 }
	 return false; // fetch tokens from the token buffer
      } else {
	 while (is_letter(codepoint) || is_digit(codepoint)) {
	    next_codepoint();
	 }
	 int keyword_token;
	 if (keyword_table.lookup(*tokenstr, keyword_token)) {
	    token = keyword_token;
	    /* a semantic value is no longer required for keywords */
	    tokenstr = nullptr;
	 } else {
	    token = parser::token::IDENT;
	    if (tokenstr != nullptr) {
	       yylval = std::make_shared<Node>(make_loc(tokenloc),
		  Token(token, std::move(tokenstr)));
	    }
	 }
      }
   } else if (is_digit(codepoint)) {
      tokenstr = std::make_unique<std::string>();
      next_codepoint();
      while (is_digit(codepoint)) {
	 next_codepoint();
      }
      token = parser::token::CARDINAL_LITERAL;
      yylval = std::make_shared<Node>(make_loc(tokenloc),
	 Token(token, std::move(tokenstr)));
   } else {
      switch (codepoint) {
	 case 0:
	    /* eof */
	    break;
	 case '/':
	    next_codepoint();
	    if (codepoint == '/') {
	       /* single-line comment */
	       while (!eof && codepoint != '\n') {
		  next_codepoint();
	       }
	       if (eof) {
		  error("unexpected eof in single-line comment");
	       }
	    } else if (codepoint == '*') {
	       /* delimited comment */
	       next_codepoint();
	       bool star = false;
	       while (!eof && (!star || codepoint != '/')) {
		  star = codepoint == '*';
		  next_codepoint();
	       }
	       if (eof) {
		  error("unexpected eof in delimited comment");
	       } else {
		  next_codepoint();
	       }
	    } else {
	       /* regular expression */
	       scan_regexp(0, '/');
	    }
	    return false; // fetch tokens from the token buffer
	 case '"':
	    scan_string_literal(yylval, token);
	    break;
	 /* compound delimiters */
	 case '-':
	    next_codepoint();
	    if (codepoint == '>') {
	       next_codepoint(); token = parser::token::ARROW;
	    } else if (codepoint == '-') {
	       next_codepoint(); token = parser::token::MINUSMINUS;
	    } else if (codepoint == '=') {
	       next_codepoint(); token = parser::token::MINUS_EQ;
	    } else {
	       token = parser::token::MINUS;
	    }
	    break;
	 case '<':
	    next_codepoint();
	    if (codepoint == '=') {
	       next_codepoint(); token = parser::token::LE;
	    } else {
	       token = parser::token::LT;
	    }
	    break;
	 case '>':
	    next_codepoint();
	    if (codepoint == '=') {
	       next_codepoint(); token = parser::token::GE;
	    } else {
	       token = parser::token::GT;
	    }
	    break;
	 case '!':
	    next_codepoint();
	    if (codepoint == '=') {
	       next_codepoint(); token = parser::token::NE;
	    } else {
	       token = parser::token::NOT;
	    }
	    break;
	 case '=':
	    next_codepoint();
	    if (codepoint == '~') {
	       next_codepoint(); token = parser::token::MATCHES;
	    } else if (codepoint == '=') {
	       next_codepoint(); token = parser::token::EQEQ;
	    } else {
	       token = parser::token::EQ;
	    }
	    break;
	 case '&':
	    next_codepoint();
	    if (codepoint == '&') {
	       next_codepoint(); token = parser::token::AND;
	    } else if (codepoint == '=') {
	       next_codepoint(); token = parser::token::AMP_EQ;
	    } else {
	       token = parser::token::AMPERSAND;
	    }
	    break;
	 case '+':
	    next_codepoint();
	    if (codepoint == '=') {
	       next_codepoint(); token = parser::token::PLUS_EQ;
	    } else if (codepoint == '+') {
	       next_codepoint(); token = parser::token::PLUSPLUS;
	    } else {
	       token = parser::token::PLUS;
	    }
	    break;
	 case '|':
	    next_codepoint();
	    if (codepoint == '|') {
	       next_codepoint(); token = parser::token::OR;
	    } else {
	       error("invalid token");
	    }
	    break;
	 case '.':
	    next_codepoint();
	    if (codepoint == '.') {
	       next_codepoint();
	       if (codepoint == '.') {
		  next_codepoint();
		  token = parser::token::DOTS;
	       } else {
		  error("invalid token");
	       }
	    } else {
	       token = parser::token::DOT;
	    }
	    break;
	 /* single-character delimiters */
	 case ':':
	    next_codepoint(); token = parser::token::COLON; break;
	 case '(':
	    next_codepoint(); token = parser::token::LPAREN; break;
	 case ')':
	    next_codepoint(); token = parser::token::RPAREN; break;
	 case ';':
	    next_codepoint(); token = parser::token::SEMICOLON; break;
	 case '*':
	    next_codepoint(); token = parser::token::STAR; break;
	 case '^':
	    next_codepoint(); token = parser::token::POWER; break;
	 case '?':
	    next_codepoint(); token = parser::token::QMARK; break;
	 case ',':
	    next_codepoint(); token = parser::token::COMMA; break;
	 case '{':
	    next_codepoint(); token = parser::token::LBRACE; break;
	 case '}':
	    next_codepoint(); token = parser::token::RBRACE; break;
	 case '[':
	    next_codepoint(); token = parser::token::LBRACKET; break;
	 case ']':
	    next_codepoint(); token = parser::token::RBRACKET; break;
	 default:
	    next_codepoint();
	    error("invalid token");
	    return false; // fetch tokens from the token buffer
      }
   }
   yylloc = tokenloc;
   return true;
}

// private methods ===========================================================

void Scanner::push_token(int token, semantic_type yylval, location yylloc) {
   TokenItem t = {token, yylval, yylloc};
   token_buffer.push_back(t);
}

void Scanner::scan_text() {
   assert(tokenstr == nullptr);
   next_codepoint(); // eat opening '{'
   bool whitespace = true; // so far we have seen whitespace only
   bool current_whitespace = true; // just whitespace seen in current line
   bool last_current_whitespace = true;
   std::size_t indent = 0; // of the first line
   std::size_t current_indent = 0; // of the current line
   std::string current_text("");
   bool newline = false; // add newline to current_text if sth follows
   std::size_t nestlevel = 0;
   while (!eof && (nestlevel > 0 || codepoint != '}')) {
      last_current_whitespace = current_whitespace;
      if (codepoint == '\n') {
	 current_whitespace = true;
	 current_indent = 0;
	 if (whitespace) indent = 0;
      } else if (is_whitespace(codepoint) && current_whitespace) {
	 int incr = 1;
	 if (codepoint == '\t') {
	    incr = TAB_STOP - current_indent % TAB_STOP;
	 }
	 current_indent += incr;
	 if (whitespace) indent += incr;
      } else {
	 current_whitespace = false;
	 whitespace = false;
      }
      if (codepoint == '{') {
	 ++nestlevel;
      } else if (codepoint == '}') {
	 --nestlevel;
      }
      if (!current_whitespace) {
	 if (codepoint == '$') {
	    if (current_text.size() > 0 || current_indent > indent || newline) {
	       if (newline) {
		  current_text += '\n'; newline = false;
	       }
	       if (last_current_whitespace || current_text.size() == 0) {
		  if (current_indent > indent) {
		     for (std::size_t i = 0;
			i < current_indent - indent; ++i) {
			current_text += ' ';
		     }
		  }
	       }
	       int token = parser::token::TEXT_LITERAL;
	       push_token(token,
		  std::make_shared<Node>(make_loc(tokenloc),
		     Token(token, current_text)), tokenloc); 
	       current_text = "";
	    }
	    tokenloc.begin = oldpos;
	    next_codepoint();
	    if (codepoint == '{') {
	       // support ${...} construct which permits an
	       // arbitrary expression within the braces
	       push_token(parser::token::LBRACE,
		  NodePtr(nullptr), tokenloc);
	       location startloc = tokenloc;
	       next_codepoint();
	       int token;
	       int nestlevel = 0;
	       semantic_type yylval; location yylloc;
	       for(;;) {
		  while (!get_next_token(yylval, yylloc, token))
		     ; // already added to the token buffer
		  if (!token ||
			(token == parser::token::RBRACE && nestlevel == 0)) {
		     break;
		  }
		  if (token == parser::token::LBRACE) {
		     ++nestlevel;
		  } else if (token == parser::token::RBRACE) {
		     --nestlevel;
		  }
		  push_token(token, yylval, yylloc);
	       }
	       if (token == parser::token::RBRACE) {
		  push_token(token, yylval, yylloc);
	       } else {
		  tokenloc = startloc;
		  error("opening '${' is not closed"); continue;
	       }
	    } else if (codepoint == '.') {
	       // support $... construct
	       next_codepoint();
	       if (codepoint != '.') {
		  error("'$...' expected"); continue;
	       }
	       next_codepoint();
	       if (codepoint != '.') {
		  error("'$...' expected"); continue;
	       }
	       next_codepoint();
	       push_token(parser::token::DOTS, NodePtr(nullptr), tokenloc);
	    } else {
	       tokenstr = std::make_unique<std::string>();
	       if (!is_letter(codepoint)) {
		  error("invalid variable reference in text literal"); continue;
	       }
	       while (is_letter(codepoint) || is_digit(codepoint)) {
		  next_codepoint();
	       }
	       int token = parser::token::VARIABLE;
	       push_token(token,
		  std::make_shared<Node>(make_loc(tokenloc),
		     Token(token, std::move(tokenstr))), tokenloc);
	       tokenloc.begin = oldpos;
	    }
	    continue;
	 } else if (codepoint == '\n') {
	    if (newline) current_text += '\n';
	    newline = true;
	 } else {
	    if (newline) {
	       current_text += '\n'; newline = false;
	    }
	    if (codepoint == '\\') {
	       next_codepoint();
	    }
	    if (last_current_whitespace) {
	       if (current_indent > indent) {
		  for (std::size_t i = 0; i < current_indent - indent; ++i) {
		     current_text += ' ';
		  }
	       }
	    }
	    current_text += codepoint;
	 }
      } else if (!whitespace && codepoint == '\n') {
	 if (newline) current_text += '\n';
	 newline = true;
      }
      next_codepoint();
   }
   if (current_text.size() > 0) {
      int token = parser::token::TEXT_LITERAL;
      push_token(token,
	 std::make_shared<Node>(make_loc(tokenloc),
	    Token(token, current_text)), tokenloc); 
   }
   if (eof) {
      error("eof encountered in text literal");
   } else {
      next_codepoint();
   }
}

/* if opening_delimiter is non-null, scan_regexp() supports the
   nested use of the opening and closing delimiters
*/
void Scanner::scan_regexp(char32_t opening_delimiter,
      char32_t closing_delimiter) {
   assert(opening_delimiter != closing_delimiter);
   next_codepoint();
   tokenstr = std::make_unique<std::string>();
   int nestlevel = 0;
   while (!eof && (codepoint != closing_delimiter || nestlevel > 0)) {
      if (codepoint == '\\') {
	 next_codepoint();
      } else if (codepoint == opening_delimiter) {
	 ++nestlevel;
      } else if (codepoint == closing_delimiter) {
	 --nestlevel;
      }
      next_codepoint();
   }
   int token = parser::token::REGEXP_LITERAL;
   NodePtr node = std::make_shared<Node>(make_loc(tokenloc),
      Token(token, std::move(tokenstr)));
   push_token(token, node, tokenloc);
   if (codepoint == closing_delimiter) {
      next_codepoint();
      /* check if the regex is accepted by our regex library */
      std::string pattern = node->get_token().get_text();
      Regex re(node->get_location(), pattern);
   } else {
      error("unexpected eof in regular expression");
   }
}

void Scanner::scan_string_literal(semantic_type& yylval, int& token) {
   std::string tokenval = "";
   tokenstr = std::make_unique<std::string>();
   next_codepoint();
   while (!eof && codepoint != '"') {
      if (codepoint == '\\') {
	 next_codepoint();
	 if (is_octal_digit(codepoint)) {
	    unsigned int val = codepoint - '0';
	    next_codepoint();
	    if (is_octal_digit(codepoint)) {
	       val = val * 8 + codepoint - '0';
	       next_codepoint();
	       if (is_octal_digit(codepoint)) {
		  val = val * 8 + codepoint - '0';
		  next_codepoint();
	       }
	    }
	    if (val >= 256) {
	       error("octal constant within string literal is out of range");
	    }
	    tokenval += static_cast<char32_t>(val);
	 } else {
	    switch (codepoint) {
	       case '\\':
		  tokenval += '\\';
		  break;
	       case '"':
		  tokenval += '"';
		  break;
	       case '\'':
		  tokenval += '\'';
		  break;
	       case '?':
		  tokenval += '?';
		  break;
	       case 'a':
		  tokenval += '\a';
		  break;
	       case 'b':
		  tokenval += '\b';
		  break;
	       case 'n':
		  tokenval += '\n';
		  break;
	       case 'r':
		  tokenval += '\r';
		  break;
	       case 't':
		  tokenval += '\t';
		  break;
	       case 'v':
		  tokenval += '\v';
		  break;
	       case 'u':
	       case 'U':
		  /* Unicode codepoint in hex notation */
		  {
		     unsigned int nof_digits = codepoint == 'u'? 4: 8;
		     char32_t val = 0;
		     while (nof_digits > 0) {
			next_codepoint();
			if (!is_hex_digit(codepoint)) {
			   error("hex digit expected");
			}
			val = val * 0x10 + eval_hex_digit(codepoint);
			--nof_digits;
		     }
		     if (!valid_codepoint(val)) {
			error("invalid Unicode codepoint");
		     }
		     tokenval += val;
		  }
		  break;
	       default:
		  error("invalid \\-sequence in string literal");
		  break;
	    }
	    next_codepoint();
	 }
      } else {
	 tokenval += codepoint;
	 next_codepoint();
      }
   }
   if (codepoint == '"') {
      next_codepoint();
   } else {
      error("unexpected eof in string literal");
   }
   token = parser::token::STRING_LITERAL;
   yylval = std::make_shared<Node>(make_loc(tokenloc),
      Token(token, tokenval, std::move(tokenstr)));
}

void Scanner::next_ch() {
   tokenloc.end = pos;
   oldpos = pos;
   if (eof_seen) {
      ch = 0; return;
   }
   char c;
   if (!in.get(c)) {
      eof_seen = true; ch = 0; return;
   }
   ch = c;
}

void Scanner::next_codepoint() {
   char32_t char_val = 0;
   char32_t expected = 0;
   tokenloc.begin = oldpos;
   if (eof_seen) {
      eof = true; codepoint = 0; return;
   }
   if (tokenstr != nullptr) {
      *tokenstr += codepoint;
   }
   for(;;) {
      if (expected) {
	 if ((ch >> 6) == 2) {
	    char_val = (char_val << 6) | (ch & ((1<<6)-1));
	    if (--expected == 0) {
	       codepoint = char_val; break;
	    }
	 } else {
	    error("invalid UTF-8 byte sequence");
	 }
      } else if ((ch & 0x80) == 0) {
	 codepoint = ch; break;
      } else if ((ch >> 5) == 6) {
	 /* lead character with one byte following */
	 char_val = ch & ((1<<5)-1); expected = 1;
      } else if ((ch >> 4) == 14) {
	 /* lead character with two bytes following */
	 char_val = ch & ((1<<4)-1); expected = 2;
      } else if ((ch >> 3) == 30) {
	 /* lead character with three bytes following */
	 char_val = ch & ((1<<3)-1); expected = 3;
      } else if ((ch >> 2) == 62) {
	 /* lead character with four bytes following */
	 char_val = ch & ((1<<2)-1); expected = 4;
      } else if ((ch >> 1) == 126) {
	 /* lead character with five bytes following */
	 char_val = ch & 1; expected = 5;
      } else {
	 error("invalid UTF-8 byte sequence");
      }
      next_ch();
   }
   next_ch();

   if (codepoint == '\n') {
      pos.lines();
   } else if (codepoint == '\t') {
      unsigned blanks = 8 - (pos.column - 1) % 8;
      pos.columns(blanks);
   } else {
      pos.columns();
   }
}

void Scanner::error(char const* msg) {
   yyerror(&tokenloc, msg);
}

} // namespace Astl
