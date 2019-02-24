#include <stdlib.h> // for rand()
#include <string.h> // for memset()
#include <vector>
#include <memory>
#include "hanabi.hpp"
#include "player.hpp"

cardColor_t getColor(card_t card) { return (cardColor_t) ((card & CARD_COLOR_MASK) >> CARD_COLOR_OFFSET); }
char getColorChar(card_t card) {
	switch(getColor(card)) {
		case CARD_INVALID: return '?';
		case CARD_BLUE: return 'B';
		case CARD_GREEN: return 'G';
		case CARD_RED: return 'R';
		case CARD_WHITE: return 'W';
		case CARD_YELLOW: return 'Y';
		case CARD_RAINBOW: return '*';
		default: assert(0);
	}
	assert(0);
}
uint8_t getNumber(card_t card) { return (card & CARD_NUMBER_MASK) >> CARD_NUMBER_OFFSET; }
char getNumberChar(card_t card) {
	assert(getNumber(card) <= MAX_CARD_NUMBER);
	uint8_t num = getNumber(card);
	if(num == 0) return '?';
	return '0' + num;
}
card_t getCard(cardColor_t color, uint8_t number) { return (color << 4) + number; }

void fprintcard(FILE *fp, uint8_t card){
	fputc(getNumberChar(card), fp);
	fputc(card & CARD_NUMBER_KNOWN_MASK ? '\'' : ' ', fp);
	fputc(getColorChar(card), fp);
	fputc(card & CARD_COLOR_KNOWN_MASK ? '\'' : ' ', fp);
}
void fprinthand(FILE *fp, hanabi_hand_t const *hand, uint8_t handSize) {
	for(uint8_t i = 0; i < handSize; ++i) { fputc('\t',fp); fprintf(fp,"%d:",i); fprintcard(fp,hand->card[i]); }
}
/* print the public data structure */
void fprintgame(FILE *fp, hanabi_game_t const &game){
	fprintf(fp, "Piles:   B G R W Y *\n");
	fprintf(fp, "         %d %d %d %d %d %d\n",
		game.pile[CARD_BLUE],
		game.pile[CARD_GREEN],
		game.pile[CARD_RED],
		game.pile[CARD_WHITE],
		game.pile[CARD_YELLOW],
		game.pile[CARD_RAINBOW]
	);
	fprintf(fp, "Discard:\n");
	for(uint8_t i = 0; i < DECK_SIZE; ++i){
		if(game.discard[i] != 0) {
			fputc(' ',fp);
			fprintcard(fp,game.discard[i]);
		}
	}
	fprintf(fp, "\n");
	fprintf(fp, "Cards remaining in deck:      %d\n", game.ndeck);
	fprintf(fp, "Information tokens remaining: %d\n", game.ninfo);
	fprintf(fp, "Bomb tokens acquired:         %d\n", game.nbomb);
}

int rand_int(int n) {
	return (n * (long)rand()) / RAND_MAX;
}
void swapN(int n, card_t deck[DECK_SIZE]) {
	while(n--) {
		int i = rand_int(DECK_SIZE);
		int j = rand_int(DECK_SIZE);
		card_t tmp = deck[j];
		deck[j] = deck[i];
		deck[i] = tmp;
	}
}
void shuffle(card_t deck[DECK_SIZE]) {
	swapN(4*DECK_SIZE,deck);
}
void hanabi_game_init(hanabi_game_t &game, hanabi_hand_t hands[MAX_PLAYERS], std::vector<std::unique_ptr<Player>> &players, card_t deck[DECK_SIZE]) {
	static const uint8_t numcount[6] = {0,3,2,2,2,1};
	
	// initialize the deck
	memset(&game, 0, sizeof(hanabi_game_t));
	game.nplayers = players.size();
	game.active_player = rand() % game.nplayers;
	game.ninfo = MAX_INFO_TOKENS;
	game.nbomb = 0;
	game.ndeck = DECK_SIZE;
	game.nDiscard = 0;
	card_t *p = &deck[0];
	for(uint8_t color = 1; color < 7; ++color){
		for(uint8_t number = 1; number <= 5; ++number){
			for(uint8_t count = 0; count < numcount[number]; ++count){
				*p = getCard((cardColor_t) color,number);
				++p;
			}
		}
	}
	
	// shuffle
	shuffle(deck);
	
	#ifdef DEBUG 
		for(uint8_t i = 0; i < game.ndeck; ++i) { fprintcard(stdout, deck[i]); fputc('\t',stdout); } fputc('\n', stdout);
	#endif
	
	// deal
	memset(hands,0,MAX_PLAYERS*sizeof(hanabi_hand_t));
	for(uint8_t count = 0; count < HANDSIZE[game.nplayers]; ++count){
		for(uint8_t i = 0; i < game.nplayers; ++i){
			hands[i].card[count] = deck[DECK_SIZE - game.ndeck];
			game.ndeck--;
		}
	}

	#ifdef DEBUG 
		for(uint8_t i = 0; i < game.nplayers; ++i) { fprintf(stdout, "%d: ", i); fprinthand(stdout, &hands[i], HANDSIZE[game.nplayers]); fputc('\n', stdout); }
	#endif

	// Initialize Players to see other's hands
	for(uint8_t curPlayer = 0; curPlayer < game.nplayers; ++curPlayer) {
		uint8_t i = 0;
		for(; i < game.nplayers; ++i){
			players[curPlayer]->others[i] = &hands[(curPlayer + i) % game.nplayers];
		}
		for(; i < MAX_PLAYERS; ++i){
			players[curPlayer]->others[i] = NULL;
		}
	}

}

// advance the game by one turn
void hanabi_game_turn(hanabi_game_t &game, hanabi_hand_t hands[MAX_PLAYERS], std::vector<std::unique_ptr<Player>> &players, card_t deck[DECK_SIZE]){
	// Ask player to take action
	action_t action = players[game.active_player]->turn(game);
	switch (action.type) {
		case ACTION_DISCARD:
			assert(game.ninfo < 8);
			assert(0 <= action.value && action.value < HANDSIZE[game.nplayers]);
			game.ninfo++;
			game.discard[game.nDiscard++] = hands[game.active_player].card[action.value];
			for(uint8_t i = action.value; i+1 < HANDSIZE[game.nplayers]; ++i) {
				hands[game.active_player].card[i] = hands[game.active_player].card[i+1];
			}
			hands[game.active_player].card[HANDSIZE[game.nplayers]-1] = deck[DECK_SIZE-game.ndeck];
			game.ndeck--;
		break;
		case ACTION_PLAY: {
			assert(game.ninfo < 8);
			assert(0 <= action.value && action.value < HANDSIZE[game.nplayers]);
			card_t card = hands[game.active_player].card[action.value];
			if(game.pile[getColor(card)]+1 == getNumber(card)){
				game.pile[getColor(card)]++;
				if(getNumber(card) == 5) game.ninfo++;
			}else{
				game.discard[game.nDiscard++] = hands[game.active_player].card[action.value];
				game.nbomb++;
			}
			for(uint8_t i = action.value; i+1 < HANDSIZE[game.nplayers]; ++i) {
				hands[game.active_player].card[i] = hands[game.active_player].card[i+1];
			}
			hands[game.active_player].card[HANDSIZE[game.nplayers]-1] = deck[DECK_SIZE-game.ndeck];
			game.ndeck--;
		}
		break;
		case ACTION_INFO: {
			uint8_t who  = (action.value & INFO_WHO_MASK );
			bool isColor = (action.value & INFO_TYPE_MASK);
			uint8_t idx  = (action.value & INFO_IDX_MASK ) >> INFO_IDX_OFFSET;
			assert(game.ninfo > 0);
			assert(0 <= who && who < game.nplayers);
			assert(
				(isColor == false && 0 < idx && idx <= MAX_CARD_NUMBER) ||
				(isColor == true  && 0 < idx && idx <= MAX_COLORS)
			);
			// need to appropriately set the knowledge bits
			who = (who+game.active_player) % game.nplayers;
			// note: not enforcing the cant-say-no-cards-matching rule
			for(uint8_t i = 0; i < HANDSIZE[game.nplayers]; ++i){
				if(isColor) {
					if(getColor(hands[who].card[i]) == (cardColor_t) idx) {
						hands[who].card[i] |= CARD_COLOR_KNOWN_MASK;
					}
				}else{
					if(getNumber(hands[who].card[i]) == idx) {
						hands[who].card[i] |= CARD_NUMBER_KNOWN_MASK;
					}
				}
			}
			hanabi_hand_t newHand;
			for(uint8_t i = 0; i < HANDSIZE[game.nplayers]; ++i) {
				newHand.card[i]  = (hands[who].card[i] & CARD_NUMBER_KNOWN_MASK) ? (hands[who].card[i] & CARD_NUMBER_MASK) : 0;
				newHand.card[i] |= (hands[who].card[i] & CARD_COLOR_KNOWN_MASK) ? (hands[who].card[i] & CARD_COLOR_MASK) : 0;
			}
			players[who]->info(&newHand, action.value);
			game.ninfo--;
		}
		break;
		default:
			assert(0);
		break;
	}
	// Advance turn
	game.active_player = (game.active_player+1)%game.nplayers;
}

int main(int argc, char *argv[]){
	uint8_t nplayers = 3;
	int nRounds = 1;
	// note: we never "remove" cards from the deck array. We deal by just reading the next appropriate entry
	if(argc > 1){
		nplayers = atoi(argv[1]);
		if(nplayers < 3){ nplayers = 3; }
		else if(nplayers > 5){ nplayers = 5; }
		if(argc > 2) {
			nRounds = atoi(argv[2]);
		}
	}

	uint avgScore = 0;
	for(int j = 0; j < nRounds; ++j) {
		std::vector<std::unique_ptr<Player>> players;
		for(uint8_t i = 0; i < nplayers; ++i) players.push_back(std::make_unique<Player>());
		hanabi_game_t game;
		hanabi_hand_t hands[MAX_PLAYERS];
		card_t deck[DECK_SIZE];
		hanabi_game_init(game, hands, players, deck);
		
		// determine how many turns should occur
		uint8_t nturns = game.ndeck + game.nplayers;
		for(uint8_t turn = 0; turn < nturns; ++turn){
			hanabi_game_turn(game, hands, players, deck);
			if(game.nbomb == 3) break;
		}
	
		uint8_t score = 0;
		for(uint8_t i = 0; i < countof(game.pile); ++i) score += game.pile[i];
		#ifdef DEBUG
			fprintf(stdout,"Game over. Score = %d\n",score);
		#endif
		avgScore += score;
	}

	fprintf(stdout, "Average score = %d\n", avgScore / nRounds);
}
