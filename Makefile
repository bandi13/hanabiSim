CXXFLAGS=-g -pg -Og -Wall
all: bin/hanabi_interactive

bin/hanabi_%: player_%.cpp obj/hanabi.o
	g++ obj/hanabi.o $(CXXFLAGS) -o $@ $<

obj/hanabi.o: hanabi.cpp
	g++ -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f obj/* bin/*