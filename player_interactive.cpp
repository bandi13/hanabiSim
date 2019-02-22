#include "player_interactive.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getrange(int lo, int hi){ // ask user for a number between lo and hi, inclusive. keep going until valid
	int response = hi+1;
	while(!(lo <= response && response <= hi)){
		int ret = scanf("%d", &response);
		if(ret <= 0) {
			fprintf(stdout, "Invalid response\n");
		}
	}
	return response;
}

action_t Player_interactive::turn(const hanabi_public_t *pub, char *action){
	fprintf(stdout, "############################################################\n");
	fprintpub(stdout, pub);
	fprintf(stdout, "Action (%d=discard,%d=play,%d=info): ",ACTION_DISCARD, ACTION_PLAY, ACTION_INFO);
	action_t acttype = (action_t) getrange(0,2);
	switch (acttype) {
		case ACTION_INFO: {
				fprintf(stdout, "Tell who? ");
				int who = getrange(1,pub->game->nplayers-1);
				fprintf(stdout, "Number (1) or color (2)? ");
				int type = getrange(1,2);
				fprintf(stdout, "What (1-5 number, %d=B,%d=G,%d=R,%d=W,%d=Y,%d=*)? ", CARD_BLUE, CARD_GREEN, CARD_RED, CARD_WHITE, CARD_YELLOW, CARD_RAINBOW);
				int idx = getrange(1, 1 == type ? 5 : 6);
				*action = ((idx << 4) | ((type-1) << 3) | who);
			}
			break;
		default:
			fprintf(stdout, "Which? ");
			*action = getrange(0,MAX_HANDSIZE-1);
			break;
	}
	return acttype;
}