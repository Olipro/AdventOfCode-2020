# Advent of Code 2020

This is a repository containing my solutions to AoC 2020.

All of them are in C++20 and are built as individual executables.
Only the standard library is used so it should theoretically build on
any platform with a modern-enough compiler.

One exception is GCC for one of the days due to an incorrect implementation
of std::reduce in libstdc++. This will also impact clang unless your toolchain
is setup to default to libc++ or you explicitly build with -stdlib=libc++

## Compiling

CMake is easy enough, but you could alternatively build manually using
something along the lines of:

`clang++ -std=c++20 -O3 -I. -o day1 day1/main.cpp` - Obviously this would
build day 1. Substitute numbers as you wish if you don't want to use
CMake.

## Usage

All programs take at least one argument. The first argument is always
intended to be a file. I always **strip the trailing newline** from any
input files (should they have it) - I cannot guarantee correct parsing
behaviour if you do not.

