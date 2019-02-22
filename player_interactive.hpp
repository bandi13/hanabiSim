#ifndef PLAYER_INTERACTIVE_HPP
#define PLAYER_INTERACTIVE_HPP

#include "player.hpp"

class Player_interactive : public Player {
	public:
		action_t turn(hanabi_game_t const *pub) override;
};

#endif