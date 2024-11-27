CXX = gcc
CXXFLAGS = -Wall -Iinclude -lraylib 
TARGET = main
FILE = main.c

main: 
	$(CXX) $(CXXFLAGS) $(FILE) -g -o $@ 

#pixelDraw: main.cpp
#	g++ main.cpp -o main.o -Wall -Wextra -lraylib
