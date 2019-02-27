#include "player.hpp"

uint8_t getrange(uint8_t lo, uint8_t hi){ // ask user for a number between lo and hi, inclusive. keep going until valid
	return rand_int(hi - lo) + lo;
}

class Player_example : public Player {
	public:
	action_t turn(hanabi_game_t const &game) override {
		actionType_t type = (game.ninfo == 8) ? ACTION_INFO : (actionType_t) getrange(0,2);
		switch (type) {
			case ACTION_DISCARD:
				return returnDiscard(getrange(0,MAX_HANDSIZE-1));
			case ACTION_PLAY:
				return returnPlay(getrange(0,MAX_HANDSIZE-1));
			case ACTION_INFO: {
				uint8_t who = getrange(1,game.nplayers-1);
				uint8_t type = getrange(1,2);
				uint8_t idx = getrange(1, 1 == type ? 5 : 6);
				return returnInfo(who, type-1, idx);
			}
			default:
				assert(0);
		}
		oldHand = myHand;
	}
	protected:
		hanabi_hand_t oldHand;
};

std::unique_ptr<Player> getPlayer() { return std::make_unique<Player_example>(); }