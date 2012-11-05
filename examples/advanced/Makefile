CC=gcc
# CXX=clang++ -ferror-limit=1 -Wno-logical-op-parentheses 
CXX=g++
RM=rm -f
CXXFLAGS=-I ~/src/boost/svn/release  -Wall -Wextra -pedantic -Wno-long-long -Wno-unused-parameter -Wno-unused -Wno-parentheses -D_GLIBCXX_DEBUG -g -O0
# LDFLAGS=-g $(shell root-config --ldflags)
# LDLIBS=$(shell root-config --libs)

SRCS = $(wildcard *.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))
EXES=$(subst .cpp,,$(SRCS))

all: $(EXES)
	for x in $(EXES) ; do echo '#####' $$x && ./$$x ; done
none:	
	./any
	./variant
	./erasure
	./tokenize
	./parse

clean:
	$(RM) $(EXES)

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CXXFLAGS) -fsyntax-only $(CHK_SOURCES)

any: any.cpp
	$(CXX) $(CXXFLAGS) any.cpp -o any

variant: variant.cpp
	$(CXX) $(CXXFLAGS) variant.cpp -o variant

erasure: erasure.cpp
	$(CXX) $(CXXFLAGS) erasure.cpp -o erasure

shared: shared.cpp
	$(CXX) $(CXXFLAGS) shared.cpp -o shared

tokenize: tokenize.cpp
	$(CXX) $(CXXFLAGS) tokenize.cpp -o tokenize

parse: parse.cpp
	$(CXX) $(CXXFLAGS) parse.cpp -o parse

