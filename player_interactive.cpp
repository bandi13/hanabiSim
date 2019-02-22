#include "player_interactive.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t getrange(uint8_t lo, uint8_t hi){ // ask user for a number between lo and hi, inclusive. keep going until valid
	int response = hi+1;
	while(!(lo <= response && response <= hi)){
		uint8_t ret = scanf("%d", &response);
		if(ret <= 0) {
			fprintf(stdout, "Invalid response\n");
		}
	}
	return response;
}

action_t Player_interactive::turn(hanabi_game_t const &game){
	fprintf(stdout, "############################################################\n");
	fprintgame(stdout, game);
	fprintf(stdout, "Current hand:\n");
	fprinthand(stdout,&myHand,HANDSIZE[game.nplayers]);
	fprintf(stdout, "\n");
	fprintf(stdout, "Other hands:\n");
	for(uint8_t j = 1; j < game.nplayers; ++j){
		fprintf(stdout,"%d: ", j);
		fprinthand(stdout, others[j], HANDSIZE[game.nplayers]);
		fputc('\n',stdout);
	}
	fprintf(stdout, "Action (%d=discard,%d=play,%d=info): ",ACTION_DISCARD, ACTION_PLAY, ACTION_INFO);
	actionType_t type;
	type = (actionType_t) getrange(0,2);
	switch (type) {
		case ACTION_DISCARD:
			fprintf(stdout, "Discard which? ");
			return returnDiscard(getrange(0,MAX_HANDSIZE-1));
		case ACTION_PLAY:
			fprintf(stdout, "Play which? ");
			return returnPlay(getrange(0,MAX_HANDSIZE-1));
		case ACTION_INFO: {
			fprintf(stdout, "Tell who? ");
			uint8_t who = getrange(1,game.nplayers-1);
			fprintf(stdout, "Number (1) or color (2)? ");
			uint8_t type = getrange(1,2);
			fprintf(stdout, "What (1-5 number, %d=B,%d=G,%d=R,%d=W,%d=Y,%d=*)? ", CARD_BLUE, CARD_GREEN, CARD_RED, CARD_WHITE, CARD_YELLOW, CARD_RAINBOW);
			uint8_t idx = getrange(1, 1 == type ? 5 : 6);
			return returnInfo(who, type-1, idx);
		}
		default:
			assert(0);
	}
}