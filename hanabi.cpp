#include "hanabi.hpp"
#include "player.hpp"
#include "player_interactive.hpp"
#include <stdlib.h> // for rand()
#include <string.h> // for memset()
#include <assert.h>
#include <vector>
#include <memory>

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
void fprinthand(FILE *fp, const hanabi_hand_t &hand, uint8_t handSize) {
	for(uint8_t i = 0; i < handSize; ++i) { fputc('\t',fp); fprintf(fp,"%d:",i); fprintcard(fp,hand.card[i]); }
}
/* print the public data structure */
void fprintpub(FILE *fp, const hanabi_public_t *pub){
	fprintf(fp, "Piles:   B G R W Y *\n");
	fprintf(fp, "         %d %d %d %d %d %d\n",
		pub->game->pile[CARD_BLUE],
		pub->game->pile[CARD_GREEN],
		pub->game->pile[CARD_RED],
		pub->game->pile[CARD_WHITE],
		pub->game->pile[CARD_YELLOW],
		pub->game->pile[CARD_RAINBOW]
	);
	fprintf(fp, "Discard:\n");
	for(uint8_t i = 0; i < DECK_SIZE; ++i){
		if(pub->game->discard[i] != 0) {
			fputc(' ',fp);
			fprintcard(fp,pub->game->discard[i]);
		}
	}
	fprintf(fp, "\n");
	fprintf(fp, "Cards remaining in deck:      %d\n", pub->game->ndeck);
	fprintf(fp, "Information tokens remaining: %d\n", pub->game->ninfo);
	fprintf(fp, "Bomb tokens acquired:         %d\n", pub->game->nbomb);
	fprintf(fp, "Current hand:\n");
	fprinthand(fp,pub->mine,HANDSIZE[pub->game->nplayers]);
	fprintf(fp, "\n");
	fprintf(fp, "Other hands:\n");
	for(uint8_t j = 1; j < pub->game->nplayers; ++j){
		fprintf(fp,"%d: ", j);
		fprinthand(fp,*pub->others[j], HANDSIZE[pub->game->nplayers]);
		fputc('\n',fp);
	}
}

/*static int rand_int(int n){
	int limit = RAND_MAX - RAND_MAX % n;
	int rnd;
	do{ rnd = rand(); }while(rnd >= limit);
	return rnd % n;
}*/
static int rand_int(int n) {
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
void hanabi_game_init(int nplayers, hanabi_game_t *game, hanabi_hand_t hands[MAX_PLAYERS], card_t deck[DECK_SIZE]) {
	static const uint8_t numcount[6] = {0,3,2,2,2,1};
	
	// initialize the deck
	memset(game, 0, sizeof(hanabi_game_t));
	game->nplayers = nplayers;
	game->active_player = rand() % nplayers;
	game->ninfo = MAX_INFO_TOKENS;
	game->nbomb = 0;
	game->ndeck = DECK_SIZE;
	game->nDiscard = 0;
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
	
	for(uint8_t i = 0; i < game->ndeck; ++i) { fprintcard(stdout, deck[i]); fputc('\t',stdout); } fputc('\n', stdout);
	
	// deal
	memset(hands,0,MAX_PLAYERS*sizeof(hanabi_hand_t));
	for(uint8_t count = 0; count < HANDSIZE[game->nplayers]; ++count){
		for(uint8_t i = 0; i < game->nplayers; ++i){
			hands[i].card[count] = deck[DECK_SIZE - game->ndeck];
			game->ndeck--;
		}
	}

	for(uint8_t i = 0; i < game->nplayers; ++i) { fprintf(stdout, "%d: ", i); fprinthand(stdout, hands[i], game->nplayers); fputc('\n', stdout); }
}

// construct the public information from the current game state
// currently just copies everything
void game_make_pub(const hanabi_game_t *game, hanabi_hand_t hands[MAX_PLAYERS], hanabi_public_t *pub){
	pub->game = game;
	for(uint8_t i = 0; i < HANDSIZE[game->nplayers]; ++i) {
		pub->mine.card[i]  = (hands[game->active_player].card[i] & CARD_NUMBER_KNOWN_MASK) ? (hands[game->active_player].card[i] & CARD_NUMBER_MASK) : 0;
		pub->mine.card[i] |= (hands[game->active_player].card[i] & CARD_COLOR_KNOWN_MASK) ? (hands[game->active_player].card[i] & CARD_COLOR_MASK) : 0;
	}
	for(uint8_t i = 0; i < game->nplayers; ++i){
		uint8_t w = (game->active_player + i) % game->nplayers;
		pub->others[i] = &hands[w];
	}
	for(uint8_t i = game->nplayers; i < MAX_PLAYERS; ++i){
		pub->others[i] = NULL;
	}
}

// advance the game by one turn
void hanabi_game_turn(hanabi_game_t *game, hanabi_hand_t hands[MAX_PLAYERS], std::vector<std::unique_ptr<Player>> &players, card_t deck[DECK_SIZE]){
	hanabi_public_t pub;
	
	// generate the public information
	game_make_pub(game, hands, &pub);
	
	// Ask player to take action
	action_t action = players[game->active_player]->turn(&pub);
	switch (action.type) {
		case ACTION_DISCARD:
			assert(game->ninfo < 8);
			assert(0 <= action.value && action.value < HANDSIZE[game->nplayers]);
			game->ninfo++;
			game->discard[game->nDiscard++] = hands[game->active_player].card[action.value];
			for(uint8_t i = action.value; i+1 < HANDSIZE[game->nplayers]; ++i) {
				hands[game->active_player].card[i] = hands[game->active_player].card[i+1];
			}
			hands[game->active_player].card[HANDSIZE[game->nplayers]-1] = deck[DECK_SIZE-game->ndeck];
			game->ndeck--;
		break;
		case ACTION_PLAY: {
			assert(game->ninfo < 8);
			assert(0 <= action.value && action.value < HANDSIZE[game->nplayers]);
			card_t card = hands[game->active_player].card[action.value];
			if(game->pile[getColor(card)]+1 == getNumber(card)){
				game->pile[getColor(card)]++;
			}else{
				game->discard[game->nDiscard++] = hands[game->active_player].card[action.value];
				game->nbomb++;
			}
			for(uint8_t i = action.value; i+1 < HANDSIZE[game->nplayers]; ++i) {
				hands[game->active_player].card[i] = hands[game->active_player].card[i+1];
			}
			hands[game->active_player].card[HANDSIZE[game->nplayers]-1] = deck[DECK_SIZE-game->ndeck];
			game->ndeck--;
		}
		break;
		case ACTION_INFO: {
			uint8_t who  = (action.value & INFO_WHO_MASK );
			bool isColor = (action.value & INFO_TYPE_MASK);
			uint8_t idx  = (action.value & INFO_IDX_MASK ) >> INFO_IDX_OFFSET;
			assert(game->ninfo > 0);
			assert(0 <= who && who < game->nplayers);
			assert(
				(isColor == false && 0 < idx && idx <= MAX_CARD_NUMBER) ||
				(isColor == true  && 0 < idx && idx <= MAX_COLORS)
			);
			// need to appropriately set the knowledge bits
			who = (who+game->active_player) % game->nplayers;
			// note: not enforcing the cant-say-no-cards-matching rule
			for(uint8_t i = 0; i < HANDSIZE[game->nplayers]; ++i){
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
			players[who]->info(&hands[who], action.value);
			game->ninfo--;
		}
		break;
		default:
			assert(0);
		break;
	}
	// Advance turn
	game->active_player = (game->active_player+1)%game->nplayers;
}

int main(int argc, char *argv[]){
	uint8_t nplayers = 3;
	// note: we never "remove" cards from the deck array. We deal by just reading the next appropriate entry
	card_t deck[DECK_SIZE];
	hanabi_hand_t hands[MAX_PLAYERS];
	if(argc > 1){
		nplayers = atoi(argv[1]);
		if(nplayers < 3){ nplayers = 3; }
		else if(nplayers > 5){ nplayers = 5; }
	}
	std::vector<std::unique_ptr<Player>> players;
	for(uint8_t i = 0; i < nplayers; ++i) players.push_back(std::make_unique<Player_interactive>());
	
	hanabi_game_t game;
	hanabi_game_init(nplayers, &game, hands, deck);
	
	// determine how many turns should occur
	uint8_t nturns = game.ndeck + game.nplayers;
	for(uint8_t turn = 0; turn < nturns; ++turn){
		hanabi_game_turn(&game, hands, players, deck);
	}
}
