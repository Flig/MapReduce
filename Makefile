all: main

%: %.cc
	g++ -std=c++11 $< -o $@


