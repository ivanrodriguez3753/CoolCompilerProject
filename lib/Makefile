CXX = clang++
CXXFLAGS = -Wall -Wextra -Weffc++ -Wfloat-equal -Wshadow\
	-Wpointer-arith -Wcast-align -Wstrict-overflow=5\
	-Wwrite-strings -Wswitch-default -Wswitch-enum -Wparentheses\
	-Woverloaded-virtual -Wl -pedantic -g -std=c++11

OBJS = ast.o environment.o lexer.yy.o ParserDriver.o parse.o

.PHONY: all libivanlib.a
all: libivanlib.a

libTarget:
	echo "HIII COMMAND LINE"
libivanlib.a: $(OBJS)
	ar ru $@ $^
	rm -rf ../build
	mkdir ../build
	mv libivanlib.a ../build/libivanlib.a
	#rm parse.cpp parse.hpp parse.out #comment this line out to read the code produced by lemon
	#rm lexer.yy.cpp lexer.yy.hpp #comment this line out to read the code produced by flex


ParserDriver.o: lexer.yy.hpp
lexer.ll: parse.hpp

parse.cpp parse.hpp: lemonfiles
.PHONY: lemonfiles
lemonfiles: parse.y
	lemon parse.y -s
	mv parse.c parse.cpp
	mv parse.h parse.hpp

lexer.yy.cpp lexer.yy.hpp: flexfiles
.PHONY: flexfiles
flexfiles: lexer.ll
	flex --outfile=lexer.yy.cpp --header-file=lexer.yy.hpp lexer.ll

.PHONY: clean
clean:
	rm -f *.o
	rm -f lexer.yy.cpp lexer.yy.hpp
	rm -f parse.cpp parse.hpp parse.out
	rm -f parse.c parse.h
	rm -r ../build/libivanlib.a
