# hanabi simulator
Created a framework in C++ for simulating various algorithms for the game Hanabi. All you need to do is implement the Player class with your own implementation to see what is the expected outcome of the game with your strategy.

This implementation is loosely based on https://gist.github.com/victorliu/8265574. I did not like the single file approach, nor the amount of data being copied. Player implementations should clearly be object oriented, hence choosing C++.

## Custom implementations
To make your own Player class, make a copy of player_random.cpp and name it something like player_myalg.cpp. Then add it to the Makefile as a separate build target. Then you will get a bin/hanabi_myalg executable.