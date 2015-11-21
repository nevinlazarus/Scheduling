skipList: Graph.hpp user.cpp
	g++ -Wall -Werror -std=c++11 -O2 -pthread -o Graph Graph.cpp user.cpp
