CXXFLAGS=-Wall #-g -Og -DDEBUG
all: bin/hanabi_interactive bin/hanabi_random

bin/hanabi_%: player_%.cpp player.hpp obj/hanabi.o
	@mkdir -p $(@D)
	g++ obj/hanabi.o $(CXXFLAGS) -o $@ $<

obj/hanabi.o: hanabi.cpp hanabi.hpp
	@mkdir -p $(@D)
	g++ -c $(CXXFLAGS) -o $@ $<

clean:
	rm -rf obj bin