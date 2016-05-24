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

#ifndef ASTL_REGEX_H
#define ASTL_REGEX_H

#include <string>
#include <utility>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <astl/exception.hpp>

namespace Astl {

   class Regex {
      public:
	 Regex() : code(nullptr), match_data(nullptr) {
	 }

	 Regex(const Location& loc,
	    const std::string& pattern) throw(Exception) :
	       code(nullptr), match_data(nullptr) {
	    compile(loc, pattern);
	 }

	 ~Regex() {
	    if (code != nullptr) pcre2_code_free(code);
	    if (match_data != nullptr) pcre2_match_data_free(match_data);
	 }

	 void compile(const Location& loc,
	       const std::string& pattern) throw(Exception) {
	    // cleanup old state
	    if (code) {
	       pcre2_code* oldcode = nullptr;
	       std::swap(code, oldcode);
	       if (code != nullptr) pcre2_code_free(code);
	    }
	    if (match_data) {
	       pcre2_match_data* old_data = nullptr;
	       std::swap(match_data, old_data);
	       if (old_data != nullptr) pcre2_match_data_free(old_data);
	    }
	    // compile the pattern
	    std::size_t len = pattern.size();
	    const char* pattern_begin = pattern.c_str();
	    int errorcode = 0;
	    size_t error_offset = 0;
	    code = pcre2_compile((PCRE2_SPTR) pattern_begin, len,
	       PCRE2_ALT_BSUX | PCRE2_UCP | PCRE2_UTF,
	       &errorcode, &error_offset, /* context = */ nullptr);
	    if (!code) {
	       std::ostringstream os;
	       os << "error in regular expression m{" << pattern << "}";
	       if (error_offset > 0) {
		  os << " at offset " << error_offset;
	       }
	       if (errorcode) {
		  char buf[1024];
		  auto nbytes = pcre2_get_error_message(errorcode,
		     (PCRE2_UCHAR*) buf, sizeof buf - 1);
		  if (nbytes > 0) {
		     buf[nbytes] = 0;
		     os << ": " << buf;
		  }
	       }
	       throw Exception(loc, os.str());
	    }
	    match_data = pcre2_match_data_create_from_pattern(code, nullptr);
	    if (!match_data) {
	       throw Exception(loc, "out of memory while preparing regex");
	    }
	 }

	 AttributePtr match(const std::string& text) {
	    assert(code != nullptr && match_data != nullptr);
	    const char* text_begin = text.c_str();
	    size_t textlen = text.size();
	    int rc = pcre2_match(code,
	       (PCRE2_SPTR) text_begin, textlen,
	       /* start offset = */ 0, /* options = */ 0,
	       match_data,
	       /* match context = */ nullptr);
	    if (rc < 0) {
	       /* no matches or other errors */
	       return nullptr;
	    }
	    size_t* ovector = pcre2_get_ovector_pointer(match_data);
	    size_t count = pcre2_get_ovector_count(match_data);
	    Attribute::SubtokenVector subtokens(count);
	    for (size_t i = 0; i < count; ++i) {
	       const char* substring = text_begin + ovector[2*i];
	       size_t substring_len = ovector[2*i+1] - ovector[2*i];
	       subtokens[i] = std::string(substring, substring_len);
	    }
	    return std::make_shared<Attribute>(subtokens);
	 }

      private:
	 pcre2_code* code; /* result of pcre2_compile */
	 pcre2_match_data* match_data;
   };

} // namespace Astl

#endif
