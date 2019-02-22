#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "hanabi.hpp"
#include "assert.h"

typedef enum { ACTION_DISCARD = 0, ACTION_PLAY = 1, ACTION_INFO = 2 } actionType_t;
typedef struct {
	actionType_t type;
	uint8_t value;
} action_t;
#define INFO_WHO_MASK    0x07
#define INFO_WHO_OFFSET  0
#define INFO_TYPE_MASK   0x08
#define INFO_TYPE_OFFSET 3
#define INFO_IDX_MASK    0xF0
#define INFO_IDX_OFFSET  4
typedef enum { INFO_TYPE_NUMBER = 0, INFO_TYPE_COLOR = 1 } actionInfo_t;

class Player {
	public:
		virtual action_t returnDiscard(uint8_t cardNum) final {
			action_t ret = {
				.type = ACTION_DISCARD,
				.value = cardNum
			};
			return ret;
		}
		virtual action_t returnPlay(uint8_t cardNum) final {
			action_t ret = {
				.type = ACTION_PLAY,
				.value = cardNum
			};
			return ret;
		}
		virtual action_t returnInfo(uint8_t who, bool isColor, uint8_t idx) final {
			assert(who <= MAX_PLAYERS);
			action_t ret = {
				.type = ACTION_INFO,
				.value = (uint8_t) ((idx << 4) | (((uint8_t)isColor) << 3) | who)
			};
			return ret;
		}
		// called when an action must be taken:
		//   return discard: action is the index of the card to discard
		//          play:    action is the index of the card to play
		//          info:    action is the information to say
		//                     low 3 bits: player index offset to say (0 is the active player, 1 is the next, etc. 0 is invalid)
		//                     next bit: 0=number, 1=color
		//                     next 4 bits: the number or color index to say
		virtual action_t turn(hanabi_game_t const *pub) = 0;
		// called when the player is told information
		// either the higher nibble (color), or lower nibble (number) is set in the info
		void info(hanabi_hand_t const *hand, uint8_t info) { myHand = *hand; }

		// Class variables

		// Hands of the other players set directly by framework
		hanabi_hand_t const *others[MAX_PLAYERS-1];
	protected:
		// My cards
		hanabi_hand_t myHand;
};

#endif