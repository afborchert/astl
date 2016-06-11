/*
   Copyright (C) 2016 Andreas Franz Borchert
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

#ifndef ASTL_UTF8_H
#define ASTL_UTF8_H

#include <cassert>
#include <cstdlib>
#include <iterator>
#include <iostream>
#include <string>
#include <type_traits>

namespace Astl {

constexpr bool valid_codepoint(char32_t codepoint) {
   return codepoint < 0x80000000 &&
	  /* exclude surrogates */
          (codepoint < 0xd800 || codepoint > 0xdfff);
}

template<typename Iterator>
inline void convert_to_utf8(Iterator out, char32_t codepoint) {
   constexpr char32_t sixbits = (1<<6)-1;
   unsigned int bytes_left = 0;
   char ch = 0;
   if (codepoint < 0x80) {
      ch = codepoint; bytes_left = 0;
   } else if (codepoint < 0x800) {
      ch = 0xc0 | (codepoint >> 6); bytes_left = 1;
   } else if (codepoint < 0x10000) {
      ch = 0xe0 | (codepoint >> 12); bytes_left = 2;
   } else if (codepoint < 0x200000) {
      ch = 0xf0 | (codepoint >> 18); bytes_left = 3;
   } else if (codepoint < 0x04000000) {
      ch = 0xf8 | (codepoint >> 24); bytes_left = 4;
   } else if (codepoint < 0x80000000) {
      ch = 0xfc | (codepoint >> 30); bytes_left = 5;
   } else {
      /* invalid codepoint */
      assert(false); std::abort();
   }
   *out++ = ch;
   while (bytes_left > 0) {
      ch = 0x80 | ((codepoint >> (bytes_left - 1) * 6) & sixbits);
      *out++ = ch; --bytes_left;
   }
}

/* add a codepoint to an UTF-8 encoded string */
inline std::string& operator+=(std::string& s, char32_t codepoint) {
   convert_to_utf8(std::back_inserter(s), codepoint);
   return s;
}

inline void add_codepoint(std::string& s, char32_t codepoint) {
   convert_to_utf8(std::back_inserter(s), codepoint);
}

/* print codepoint to an UTF-8 encoded output stream */
inline std::ostream& operator<<(std::ostream& out, char32_t codepoint) {
   convert_to_utf8(std::ostreambuf_iterator<char>(out), codepoint);
   return out;
}

/* bidirectional char32_t iterator on base of char iterators
   where the char sequence is utf8 encoded */
template <typename Iterator>
class CodepointIterator :
	 public std::iterator<
	    std::bidirectional_iterator_tag,
	    char32_t,
	    std::ptrdiff_t,
	    const char32_t*,
	    const char32_t&
	 > {
   public:
      CodepointIterator() : nil(true), codepoint(0) {
      }
      CodepointIterator(Iterator begin, Iterator end) :
	    nil(false), codepoint(0), current(begin), next(begin), end(end) {
	 advance();
      }
      CodepointIterator(const CodepointIterator& other) :
	 nil(other.nil), codepoint(other.codepoint),
	 current(other.current), next(other.next), end(other.end) {
      }
      bool operator==(const CodepointIterator& other) const {
	 if (is_end() && other.is_end()) return true;
	 if (is_end() || other.is_end()) return false;
	 return current == other.current;
      }
      bool operator!=(const CodepointIterator& other) const {
	 return !(*this == other);
      }
      const char32_t& operator*() const {
	 return codepoint;
      }
      CodepointIterator& operator++() {
	 advance();
	 return *this;
      }
      CodepointIterator operator++(int) {
	 CodepointIterator clone(*this);
	 advance();
	 return clone;
      }
      CodepointIterator& operator--() {
	 retreat();
	 return *this;
      }
      CodepointIterator operator--(int) {
	 CodepointIterator clone(*this);
	 retreat();
	 return clone;
      }

   private:
      bool is_end() const {
	 return nil || current == end;
      }
      void advance() {
	 if (is_end()) return;
	 current = next;
	 unsigned int expected = 0;
	 char32_t char_val = 0;
	 while (next != end) {
	    unsigned char ch = *next++;
	    if (expected) {
	       if ((ch >> 6) == 2) {
		  char_val = (char_val << 6) | (ch & ((1<<6)-1));
		  if (--expected == 0) {
		     codepoint = char_val; break;
		  }
	       } else {
		  assert(false); std::abort();
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
	       assert(false); std::abort();
	    }
	 }
	 assert(expected == 0);
      }

      void retreat() {
	 Iterator it = current;
	 unsigned char ch;
	 do {
	    --it;
	    ch = *it;
	 } while (ch >= 0x80 && (ch >> 6) == 2);
	 current = next = it;
	 advance();
	 current = next = it;
      }

      bool nil;
      char32_t codepoint;
      Iterator current;
      Iterator next;
      Iterator end;
};

template <typename Iterator>
class CodepointRange {
   public:
      CodepointRange(Iterator begin, Iterator end) :
	 begin_it(begin), end_it(end) {
      }
      CodepointIterator<Iterator> begin() const {
	 return CodepointIterator<Iterator>(begin_it, end_it);
      }
      CodepointIterator<Iterator> end() const {
	 return CodepointIterator<Iterator>(end_it, end_it);
      }
   private:
      Iterator begin_it;
      Iterator end_it;
};

template<typename T>
typename std::enable_if<
   std::is_same<typename std::iterator_traits<T>::value_type, char>::value,
   CodepointRange<T>
>
codepoint_range(T begin, T end) {
   return CodepointRange<T>(begin, end);
}
template<typename T>
CodepointRange<typename T::const_iterator>
codepoint_range(const T& s) {
   return CodepointRange<typename T::const_iterator>(s.begin(), s.end());
}

} // namespace Astl

#endif
