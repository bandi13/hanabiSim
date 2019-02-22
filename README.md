# hanabiSim
Created a framework in C++ for simulating various algorithms for the game Hanabi. All you need to do is extend the Player class with your own implementation to see what is the expected outcome of the game with your strategy.

This implementation is loosely based on https://gist.github.com/victorliu/8265574. I did not like the single file approach, nor the amount of data being copied. Player implementations should clearly be object oriented, hence choosing C++.