CXX = g++-10
CXXFLAGS = -Wall -Wextra -Weffc++ -Wfloat-equal -Wshadow\
	-Wpointer-arith -Wcast-align -Wstrict-overflow=5\
	-Wwrite-strings -Wswitch-default -Wswitch-enum -Wparentheses\
	-Woverloaded-virtual -pedantic -g -std=c++11
BUILD_DIR = ../build

.PHONY: all ivancool
all: ivancool

ivancool: main.cpp
	$(CXX) main.cpp -I../lib -L../build -livanlib -o ../build/ivancool
