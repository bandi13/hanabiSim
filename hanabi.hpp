#ifndef HANABI_HPP
#define HANABI_HPP

#include "stdio.h"
#include "stdint.h"
#include "assert.h"

#define countof(arr) sizeof(arr) / sizeof(arr[0])

// Generate a random number with [0,n]
int rand_int(int n);

// number of cards per player based on number of players
static const uint8_t HANDSIZE[] = {0,0,0,5,4,4};
#define MAX_PLAYERS (countof(HANDSIZE)-1)
#define MAX_HANDSIZE 5
#define MAX_INFO_TOKENS 8

/*
Colors: (alphabetical order, rainbow at end)
BGRWY*

Numbers:
12345

Cards:
(30 distinct,
 60 total counting
multiplicity)
                     index:
1B 2B 3B 4B 5B       0  1  2  3  4
1G 2G 3G 4G 5G       5  6  7  8  9
1R 2R 3R 4R 5R       10 11 12 13 14
1W 2W 3W 4W 5W       15 16 17 18 19
1Y 2Y 3Y 4Y 5Y       20 21 22 23 24
1* 2* 3* 4* 5*       25 26 27 28 29


Game locations:
6 stacks for build piles
30 stacks for discard piles
N hands of H slots for hands
*/
#define DECK_SIZE 60

#define CARD_COLOR_MASK 0x70
#define CARD_COLOR_OFFSET 4
#define CARD_COLOR_KNOWN_MASK 0x80
#define CARD_NUMBER_MASK 0x07
#define CARD_NUMBER_KNOWN_MASK 0x08
#define CARD_NUMBER_OFFSET 0
typedef enum {
	CARD_INVALID = 0,
	CARD_BLUE    = 1,
	CARD_GREEN   = 2,
	CARD_RED     = 3,
	CARD_WHITE   = 4,
	CARD_YELLOW  = 5,
	CARD_RAINBOW = 6
} cardColor_t;

typedef uint8_t card_t;
#define MAX_COLORS 6
#define MAX_CARD_NUMBER 5

cardColor_t getColor(card_t card);
char getColorChar(card_t card);
uint8_t getNumber(card_t card);
char getNumberChar(card_t card);
card_t getCard(cardColor_t color, uint8_t number);

typedef struct hanabi_game {
	uint8_t nplayers; // number of players
	uint8_t active_player; // whose turn it is
	uint8_t ninfo, nbomb, ndeck; // number of information tokens, bomb tokens, and cards remaining in deck
	// cards in completion piles, value is current top card number (default 0)
	card_t pile[MAX_COLORS+1];
	// cards in discard in the order they are discarded
	card_t discard[DECK_SIZE];
	// number of discarded cards
	uint8_t nDiscard;
} hanabi_game_t;

typedef struct hanabi_hand {
	card_t card[MAX_HANDSIZE];
} hanabi_hand_t;

// Utilities to print state of the game
void fprintcard(FILE *fp, int card);
void fprinthand(FILE *fp, hanabi_hand_t const *hand, uint8_t handSize);
void fprintgame(FILE *fp, hanabi_game_t const &game);

#endif