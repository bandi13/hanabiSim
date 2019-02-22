#ifndef PLAYER_INTERACTIVE_HPP
#define PLAYER_INTERACTIVE_HPP

#include "player.hpp"

class Player_interactive : public Player {
	public:
		action_t turn(const hanabi_public_t *pub) override;
};

#endif