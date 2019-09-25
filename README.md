# astl
Astl programming language

## Synopsis

This package provides the parser and the core library for the
Astl programming language. Astl is a scripting language that
is based on abstract syntax trees, hence the name. A
[language reference manual of Astl](../blob/refman/astl-refman.pdf)
is included.

The core library in itself just lays the foundation which
can be extended for a particular language where you want to
process abstract syntax trees using the comfort of a specialized
programming language. One such example is
[astl-c](https://github.com/afborchert/astl-c) for the C
programming language.

## Downloading and building

If you want to clone it, you should do this recursively:

```
git clone --recursive https://github.com/afborchert/astl.git
```

To build it, you need _bison_ 3.x, a recent _g++_ with
C++14 support, GMP, and _pcre2_ libraries:

```
(cd astl/astl && make)
```
