#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "hanabi.hpp"
#include "assert.h"

typedef enum { ACTION_DISCARD = 0, ACTION_PLAY = 1, ACTION_INFO = 2 } action_t;
#define INFO_WHO_MASK    0x07
#define INFO_WHO_OFFSET  0
#define INFO_TYPE_MASK   0x08
#define INFO_TYPE_OFFSET 3
#define INFO_IDX_MASK    0xF0
#define INFO_IDX_OFFSET  4
typedef enum { INFO_TYPE_NUMBER = 0, INFO_TYPE_COLOR = 1 } actionInfo_t;

class Player {
	public:
		// called when an action must be taken:
		//   return discard: action is the index of the card to discard
		//          play:    action is the index of the card to play
		//          info:    action is the information to say
		//                     low 3 bits: player index offset to say (0 is the active player, 1 is the next, etc. 0 is invalid)
		//                     next bit: 0=number, 1=color
		//                     next 4 bits: the number or color index to say
		virtual action_t turn(const hanabi_public_t *pub, char *actionValue) = 0;
		// called when the player is told information
		// either the higher nibble (color), or lower nibble (number) is set in the info
		void info(const hanabi_hand_t *hand, char info) { return; }
};

#endif