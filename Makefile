CXXFLAGS=-g -Og -Wall
all: bin/hanabi_interactive

bin/hanabi_%: player_%.cpp obj/hanabi.o
	@mkdir -p $(@D)
	g++ obj/hanabi.o $(CXXFLAGS) -o $@ $<

obj/hanabi.o: hanabi.cpp
	@mkdir -p $(@D)
	g++ -c $(CXXFLAGS) -o $@ $<

clean:
	rm -rf obj bin