CXX = clang++


CXXFLAGS = -Wall -Wextra -Weffc++ -Wfloat-equal -Wshadow\
	-Wpointer-arith -Wcast-align -Wstrict-overflow=5\
	-Wwrite-strings -Wswitch-default -Wswitch-enum -Wparentheses\
	-Woverloaded-virtual -pedantic -g


all: ivancool docs

.PHONY: docs
docs:
	@doxygen ./Doxyfile

ivancool: main.o parse.o lexer.yy.o
	$(CXX) $(CXXFLAGS)  main.o parse.o lexer.yy.o -o ivancool

 
main.o: main.cpp lexer.yy.hpp parse.hpp environment.o ast.o
 
parse.cpp: lemonfiles
 
parse.hpp: lemonfiles
 
.PHONY: lemonfiles
lemonfiles: parse.y
	lemon parse.y -s
	mv parse.c parse.cpp
	mv parse.h parse.hpp

ast.o: ast.h ast.cpp

environment.o: environment.h environment.cpp

lexer.o: lexer.yy.hpp lexer.yy.cpp
 
lexer.yy.cpp: flexfiles
 
lexer.yy.hpp: flexfiles
 
.PHONY: flexfiles
flexfiles: lexer.ll
	flex --outfile=lexer.yy.cpp --header-file=lexer.yy.hpp lexer.ll

.PHONY: clean
clean:
	rm -f *.o
	rm -f lexer.yy.cpp lexer.yy.hpp
	rm -f parse.cpp parse.hpp parse.out
	rm -f ivancool
	rm -r html latex man
