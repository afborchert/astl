/*
   Copyright (C) 2009 Andreas Franz Borchert
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

#ifndef SYNTAX_TREE_TOKEN_H
#define SYNTAX_TREE_TOKEN_H

#include <cassert>
#include <iostream>
#include <string>

namespace Astl {

   /**
    * A token is a tuple consisting of
    *  - a symbol (represented by an unsigned integer),
    *  - a literal representation, i.e. the sequence of characters
    *    which made up the token in the source, and
    *  - its content (which is derived from the string representation).
    * In most cases, the symbol number is derived from the corresponding
    * bison grammar, and the token content is identical to its
    * string representation. The string representation and the
    * corresponding contents differ, if some postprocessing has
    * been done by the lexical analysis.
    *
    * Examples:
    *  - In a programming language where case is not significant,
    *    identifiers can be normalized to lower case. In this case,
    *    the string representation provides the original mixed case
    *    identifier while all letters have been converted into lower
    *    case in its corresponding contents.
    *  - In case of a string constant "hello world", the
    *    string representation still contains the quotes but its
    *    contents provides the actual string constant without the
    *    quotes.
    * Tokens can consist of a symbol value only without any
    * associated strings. Alternatively, they can come without
    * a symbol value but provide a string represenation only.
    *
    * Tokens are considered equivalent
    *  - if they have both well-defined symbol numbers that are
    *    identical, or,
    *  - in case at least one of them has no well-defined symbol number,
    *    if their content is identical.
    */

   class Token {
      public:
	 // constructors
	 /**
	  * The default constructor creates a token without
	  * a well-defined symbol number and without a
	  * a well-defined string representation.
	  */
	 Token() : token(0), tokenval(), tokenlit() {
	 }

	 /**
	  * Construct a simple token with the given symbol value
	  * but with an empty string representation.
	  */
	 Token(unsigned int token_param) :
	       token(token_param), tokenval(""), tokenlit("") {
	    assert(token != 0);
	 }

	 /**
	  * Construct a token without a symbol value but the given
	  * contents which is also taken as its literal representation.
	  */
	 Token(const std::string& tokenval_param) :
	       token(0), tokenval(tokenval_param),
	       tokenlit(tokenval_param) {
	 }

	 /**
	  * Construct a token with the given symbol value and
	  * the given contents which is also taken as its literal
	  * representation.
	  */
	 Token(unsigned int token_param,
	       const std::string& tokenval_param) :
	       token(token_param), tokenval(tokenval_param),
	       tokenlit(tokenval_param) {
	    assert(token != 0);
	 }

	 /**
	  * Construct a token with the given symbol value and
	  * the given contents which is also taken as its literal
	  * represenation. Please note that the memory allocated
	  * behind the string pointer is deleted after it has been
	  * copied.
	  */
	 Token(unsigned int token_param,
	       std::string*& tokenval_param) :
	       token(token_param), tokenval(*tokenval_param),
	       tokenlit(*tokenval_param) {
	    assert(token != 0 && tokenval_param != 0);
	    delete tokenval_param; tokenval_param = 0;
	 }

	 /**
	  * Construct a token with the given symbol value,
	  * the given contents, and the given literal representation.
	  */
	 Token(unsigned int token_param,
		  const std::string& tokenval_param,
		  const std::string& tokenlit_param) :
	       token(token_param), tokenval(tokenval_param),
	       tokenlit(tokenlit_param) {
	    assert(token != 0);
	 }

	 /**
	  * Construct a token with the given symbol value,
	  * the given contents, and the given literal representation.
	  * Please note that both pointers are passed to delete
	  * once the string contents has been copied.
	  */
	 Token(unsigned int token_param,
	       std::string*& tokenval_param,
	       std::string*& tokenlit_param) :
	       token(token_param), tokenval(*tokenval_param),
	       tokenlit(*tokenlit_param) {
	    assert(token != 0 && tokenval_param != 0 && tokenlit_param != 0);
	    delete tokenval_param; tokenval_param = 0;
	    delete tokenlit_param; tokenlit_param = 0;
	 }

	 /**
	  * Copy constructor for tokens.
	  */
	 Token(const Token& other) :
	    token(other.token), tokenval(other.tokenval),
	    tokenlit(other.tokenlit) {
	 }

	 // accessors
	 /**
	  * Return the symbol value of a token. Please note
	  * that this operation is permitted only for tokens
	  * with well-defined symbol values.
	  */
	 unsigned int get_tokenval() const {
	    assert(token != 0);
	    return token;
	 }

	 /**
	  * Return the contents of token. This is empty in case
	  * it is not well-defined.
	  */
	 const std::string& get_text() const {
	    return tokenval;
	 }

	 /**
	  * Return the literal representation of the string. This
	  * is the empty string if it is not well-defined.
	  */
	 const std::string& get_literal() const {
	    return tokenlit;
	 }

	 /**
	  * Return true if two tokens are considered as equivalent
	  * (see the introduction).
	  */
	 bool operator==(const Token& other) const {
	    return (token && other.token && token == other.token) ||
	       ((token == 0 || other.token == 0) &&
		  (tokenval == other.tokenval));
	 }

	 /**
	  * Return true if two tokens are not considered as equivalent
	  * (see the introduction).
	  */
	 bool operator!=(const Token& other) const {
	    return !(*this == other);
	 }

	 // mutators
	 /**
	  * Assignment operator for tokens.
	  */
	 Token& operator=(const Token& other) {
	    token = other.token;
	    tokenval = other.tokenval;
	    return *this;
	 }

	 /**
	  * Update the literal representation of a token.
	  */
         void set_literal(std::string tokenlit_param) {
            tokenlit = tokenlit_param;
         }

	 /**
	  * Update the contents of a token.
	  */
         void set_text(std::string tokenval_param) {
            tokenval = tokenval_param;
         }

      private:
	 unsigned int token; // symbol from lexical analysis
	 std::string tokenval; // actual token string
	 std::string tokenlit; // literal form of token
   };

   /**
    * Output operator for tokens which prints its literal representation.
    */
   inline std::ostream& operator<<(std::ostream& out, const Token& token) {
      return out << token.get_literal();
   }

} // namespace

#endif
