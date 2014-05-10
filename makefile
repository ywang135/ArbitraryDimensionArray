all:
	g++ -std=c++11 -g -Wall -Wextra -pedantic Array.hpp test.cpp
clean:
	rm -R a.out *.dSYM *.gch