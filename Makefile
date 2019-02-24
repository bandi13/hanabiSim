all: bin/hanabi_interactive

bin/hanabi_interactive: obj/hanabi.o player_interactive.cpp
	g++ obj/hanabi.o player_interactive.cpp -g -Og -Wall -o bin/hanabi_interactive

obj/hanabi.o: hanabi.cpp
	g++ -c hanabi.cpp -g -Og -Wall -o obj/hanabi.o