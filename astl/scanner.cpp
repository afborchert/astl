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
#include <memory>
#include <regex>
#include <astl/scanner.hpp>
#include <astl/error.hpp>
#include <astl/keywords.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/token.hpp>

namespace Astl {

const unsigned int TAB_STOP = 8;

bool is_letter(char ch) {
   return ((ch >= 'a') && (ch <= 'z')) ||
      ((ch >= 'A') && (ch <= 'Z')) || ch == '_';
}

bool is_digit(char ch) {
   return (ch >= '0') && (ch <= '9');
}

bool is_octal_digit(char ch) {
   return (ch >= '0') && (ch <= '7');
}

bool is_whitespace(char ch) {
   return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '\f';
}

// constructor ===============================================================

Scanner::Scanner(std::istream& in, const std::string& input_name) :
      in(in), input_name(input_name), ch(0),
      eof(false), tokenstr(nullptr) {
   pos.initialize(&this->input_name);
   nextch();
   if (!eof && ch == '#') {
      // skip #! line
      nextch();
      if (eof || ch != '!') {
	 error("#! line expected");
      }
      while (!eof && ch != '\n') {
	 nextch();
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
   yylval = NodePtr((Node*)0);
   for(;;) {
      if (eof) {
	 break;
      } else if (is_whitespace(ch)) {
	 nextch();
      } else {
	 break;
      }
   }
   tokenloc.begin = oldpos;
   if (is_letter(ch)) {
      char initial_letter = ch;
      tokenstr = new std::string();
      nextch();
      if (ch == '{' && (initial_letter == 'm' || initial_letter == 'q')) {
	 /* Perl-style literals m{regexp} and q{string} */
	 delete tokenstr; tokenstr = 0;
	 switch (initial_letter) {
	    case 'm':
	       scan_regexp('{', '}'); break;
	    case 'q':
	       scan_text(); break;
	    default:
	       assert(0);
	 }
	 return false; // fetch tokens from the token buffer
      } else {
	 while (is_letter(ch) || is_digit(ch)) {
	    nextch();
	 }
	 int keyword_token;
	 if (keyword_table.lookup(*tokenstr, keyword_token)) {
	    token = keyword_token;
	    /* a semantic value is no longer required for keywords */
	    delete tokenstr; tokenstr = 0;
	 } else {
	    token = parser::token::IDENT;
	    if (tokenstr) {
	       yylval = std::make_shared<Node>(make_loc(tokenloc),
		  Token(token, tokenstr));
	    }
	 }
      }
   } else if (is_digit(ch)) {
      tokenstr = new std::string();
      nextch();
      while (is_digit(ch)) {
	 nextch();
      }
      token = parser::token::CARDINAL_LITERAL;
      yylval = std::make_shared<Node>(make_loc(tokenloc),
	 Token(token, tokenstr));
      tokenstr = 0;
   } else {
      switch (ch) {
	 case 0:
	    /* eof */
	    break;
	 case '/':
	    nextch();
	    if (ch == '/') {
	       /* single-line comment */
	       while (!eof && ch != '\n') {
		  nextch();
	       }
	       if (eof) {
		  error("unexpected eof in single-line comment");
	       }
	    } else if (ch == '*') {
	       /* delimited comment */
	       nextch();
	       bool star = false;
	       while (!eof && (!star || ch != '/')) {
		  star = ch == '*';
		  nextch();
	       }
	       if (eof) {
		  error("unexpected eof in delimited comment");
	       } else {
		  nextch();
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
	    nextch();
	    if (ch == '>') {
	       nextch(); token = parser::token::ARROW;
	    } else if (ch == '-') {
	       nextch(); token = parser::token::MINUSMINUS;
	    } else if (ch == '=') {
	       nextch(); token = parser::token::MINUS_EQ;
	    } else {
	       token = parser::token::MINUS;
	    }
	    break;
	 case '<':
	    nextch();
	    if (ch == '=') {
	       nextch(); token = parser::token::LE;
	    } else {
	       token = parser::token::LT;
	    }
	    break;
	 case '>':
	    nextch();
	    if (ch == '=') {
	       nextch(); token = parser::token::GE;
	    } else {
	       token = parser::token::GT;
	    }
	    break;
	 case '!':
	    nextch();
	    if (ch == '=') {
	       nextch(); token = parser::token::NE;
	    } else {
	       token = parser::token::NOT;
	    }
	    break;
	 case '=':
	    nextch();
	    if (ch == '~') {
	       nextch(); token = parser::token::MATCHES;
	    } else if (ch == '=') {
	       nextch(); token = parser::token::EQEQ;
	    } else {
	       token = parser::token::EQ;
	    }
	    break;
	 case '&':
	    nextch();
	    if (ch == '&') {
	       nextch(); token = parser::token::AND;
	    } else if (ch == '=') {
	       nextch(); token = parser::token::AMP_EQ;
	    } else {
	       token = parser::token::AMPERSAND;
	    }
	    break;
	 case '+':
	    nextch();
	    if (ch == '=') {
	       nextch(); token = parser::token::PLUS_EQ;
	    } else if (ch == '+') {
	       nextch(); token = parser::token::PLUSPLUS;
	    } else {
	       token = parser::token::PLUS;
	    }
	    break;
	 case '|':
	    nextch();
	    if (ch == '|') {
	       nextch(); token = parser::token::OR;
	    } else {
	       error("invalid token");
	    }
	    break;
	 case '.':
	    nextch();
	    if (ch == '.') {
	       nextch();
	       if (ch == '.') {
		  nextch();
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
	    nextch(); token = parser::token::COLON; break;
	 case '(':
	    nextch(); token = parser::token::LPAREN; break;
	 case ')':
	    nextch(); token = parser::token::RPAREN; break;
	 case ';':
	    nextch(); token = parser::token::SEMICOLON; break;
	 case '*':
	    nextch(); token = parser::token::STAR; break;
	 case '^':
	    nextch(); token = parser::token::POWER; break;
	 case '?':
	    nextch(); token = parser::token::QMARK; break;
	 case ',':
	    nextch(); token = parser::token::COMMA; break;
	 case '{':
	    nextch(); token = parser::token::LBRACE; break;
	 case '}':
	    nextch(); token = parser::token::RBRACE; break;
	 case '[':
	    nextch(); token = parser::token::LBRACKET; break;
	 case ']':
	    nextch(); token = parser::token::RBRACKET; break;
	 default:
	    nextch();
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
   assert(tokenstr == 0);
   nextch(); // eat opening '{'
   bool whitespace = true; // so far we have seen whitespace only
   bool current_whitespace = true; // just whitespace seen in current line
   bool last_current_whitespace = true;
   unsigned int indent = 0; // of the first line
   unsigned int current_indent = 0; // of the current line
   std::string current_text("");
   bool newline = false; // add newline to current_text if sth follows
   unsigned int nestlevel = 0;
   while (!eof && (nestlevel > 0 || ch != '}')) {
      last_current_whitespace = current_whitespace;
      if (ch == '\n') {
	 current_whitespace = true;
	 current_indent = 0;
	 if (whitespace) indent = 0;
      } else if (is_whitespace(ch) && current_whitespace) {
	 int incr = 1;
	 if (ch == '\t') {
	    incr = TAB_STOP - current_indent % TAB_STOP;
	 }
	 current_indent += incr;
	 if (whitespace) indent += incr;
      } else {
	 current_whitespace = false;
	 whitespace = false;
      }
      if (ch == '{') {
	 ++nestlevel;
      } else if (ch == '}') {
	 --nestlevel;
      }
      if (!current_whitespace) {
	 if (ch == '$') {
	    if (current_text.size() > 0 || current_indent > indent || newline) {
	       if (newline) {
		  current_text += '\n'; newline = false;
	       }
	       if (last_current_whitespace || current_text.size() == 0) {
		  if (current_indent > indent) {
		     for (unsigned int i = 0;
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
	    nextch();
	    if (ch == '{') {
	       // support ${...} construct which permits an
	       // arbitrary expression within the braces
	       push_token(parser::token::LBRACE,
		  NodePtr(nullptr), tokenloc);
	       location startloc = tokenloc;
	       nextch();
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
	    } else if (ch == '.') {
	       // support $... construct
	       nextch();
	       if (ch != '.') {
		  error("'$...' expected"); continue;
	       }
	       nextch();
	       if (ch != '.') {
		  error("'$...' expected"); continue;
	       }
	       nextch();
	       push_token(parser::token::DOTS, NodePtr(nullptr), tokenloc);
	    } else {
	       tokenstr = new std::string();
	       if (!is_letter(ch)) {
		  error("invalid variable reference in text literal"); continue;
	       }
	       while (is_letter(ch) || is_digit(ch)) {
		  nextch();
	       }
	       int token = parser::token::VARIABLE;
	       push_token(token,
		  std::make_shared<Node>(make_loc(tokenloc),
		     Token(token, tokenstr)), tokenloc);
	       tokenloc.begin = oldpos;
	    }
	    continue;
	 } else if (ch == '\n') {
	    if (newline) current_text += '\n';
	    newline = true;
	 } else {
	    if (newline) {
	       current_text += '\n'; newline = false;
	    }
	    if (ch == '\\') {
	       nextch();
	    }
	    if (last_current_whitespace) {
	       if (current_indent > indent) {
		  for (unsigned int i = 0; i < current_indent - indent; ++i) {
		     current_text += ' ';
		  }
	       }
	    }
	    current_text += ch;
	 }
      } else if (!whitespace && ch == '\n') {
	 if (newline) current_text += '\n';
	 newline = true;
      }
      nextch();
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
      nextch();
   }
}

/* if opening_delimiter is non-null, scan_regexp() supports the
   nested use of the opening and closing delimiters
*/
void Scanner::scan_regexp(char opening_delimiter, char closing_delimiter) {
   assert(opening_delimiter != closing_delimiter);
   nextch();
   tokenstr = new std::string();
   int nestlevel = 0;
   while (!eof && (ch != closing_delimiter || nestlevel > 0)) {
      if (ch == '\\') {
	 nextch();
      } else if (ch == opening_delimiter) {
	 ++nestlevel;
      } else if (ch == closing_delimiter) {
	 --nestlevel;
      }
      nextch();
   }
   int token = parser::token::REGEXP_LITERAL;
   NodePtr node = std::make_shared<Node>(make_loc(tokenloc),
      Token(token, tokenstr));
   push_token(token, node, tokenloc);
   tokenstr = 0;
   if (ch == closing_delimiter) {
      nextch();
      /* check if the regex is accepted by our regex library */
      try {
	 std::regex re(node->get_token().get_text());
      } catch (const std::regex_error& e) {
	 std::string msg("invalid regular expression: ");
	 msg += e.what();
	 error(msg.c_str());
      } catch (const std::exception& e) {
	 std::string
	    msg("unexpected problem with regular expression: ");
	 msg += e.what();
	 error(msg.c_str());
      }
   } else {
      error("unexpected eof in regular expression");
   }
}

void Scanner::scan_string_literal(semantic_type& yylval, int& token) {
   std::string tokenval = "";
   tokenstr = new std::string();
   nextch();
   while (!eof && ch != '"') {
      if (ch == '\\') {
	 nextch();
	 if (is_octal_digit(ch)) {
	    unsigned int val = ch - '0';
	    nextch();
	    if (is_octal_digit(ch)) {
	       val = val * 8 + ch - '0';
	       nextch();
	       if (is_octal_digit(ch)) {
		  val = val * 8 + ch - '0';
		  nextch();
	       }
	    }
	    if (val >= 256) {
	       error("octal constant within string literal is out of range");
	    }
	    tokenval += (unsigned char) val;
	 } else {
	    switch (ch) {
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
	       default:
		  error("invalid \\-sequence in string literal");
		  break;
	    }
	    nextch();
	 }
      } else {
	 tokenval += ch;
	 nextch();
      }
   }
   if (ch == '"') {
      nextch();
   } else {
      error("unexpected eof in string literal");
   }
   token = parser::token::STRING_LITERAL;
   yylval = std::make_shared<Node>(make_loc(tokenloc),
      Token(token, tokenval, *tokenstr));
   tokenstr = 0;
}

void Scanner::nextch() {
   tokenloc.end = pos;
   oldpos = pos;
   if (eof) {
      ch = 0; return;
   }
   if (tokenstr) {
      *tokenstr += ch;
   }
   char c;
   if (!in.get(c)) {
      eof = true; ch = 0; return;
   }
   ch = c;
   if (ch == '\n') {
      pos.lines();
   } else if (ch == '\t') {
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
