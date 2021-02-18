CXX = g++-10
CXXFLAGS = -Wall -Wextra -Weffc++ -Wfloat-equal -Wshadow\
	-Wpointer-arith -Wcast-align -Wstrict-overflow=5\
	-Wwrite-strings -Wswitch-default -Wswitch-enum -Wparentheses\
	-Woverloaded-virtual -pedantic -g
BUILD_DIR = ../build

.PHONY: all ivancool #main.o
all: ivancool #main.o

#main.o: main.cpp
	#$(CXX) $(CXXFLAGS) -c main.cpp -o main.o
#	g++-10 main.cpp ../build/libivanlib.a -o ivancool
#ivancool: main.o
#	pwd
	#$(CXX) $(CXXFLAGS) main.o libivanlib.a -o ivancool
ivancool: main.cpp
	$(CXX) main.cpp -L../build -livanlib -o ../build/ivancool