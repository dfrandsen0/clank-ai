#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "card.h"
#include "ai.h"
#include "player.h"
#include "board.h"
#include "game.h"

int specEvaluateCardAI(player_t* player, card_t* card) {
    if(!strcmp(card->name, "Treasure Hunter")) {
	return rand() % 6;
    }

    return 0;
}

// [index, index, index ... -1]
void specFindMoveAI(player_t* player, char* moves) {

    char values[BOARD_TOTAL_NODES];
    bzero(values, BOARD_TOTAL_NODES);
    char tempValues[BOARD_TOTAL_NODES];

    initValues(player, tempValues);

    board_node_t* temp;
    char max;
    while(!updateValues(values, tempValues)) {
	for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	    if(player->artifact && i == 0) {
		continue;
	    }

	    if(!player->artifact && (boardNodes[i]->artifact || boardNodes[i]->monkeys)) {
		continue;
	    }

	    temp = boardNodes[i];
	    max = -128;
	    for(int j = 0; j < temp->connections; j++) {
		if(temp->edges[j]->locked && !(player->key || player->teleports)) {
		    continue;
		} else if(temp->edges[j]->cost < 0 && !player->teleports) {
		    continue;
		}

		if(values[temp->edges[j]->nextNodeIndex] > max) {
		    max = values[temp->edges[j]->nextNodeIndex];
		}
	    }

	    if(max != -128) {
		tempValues[i] = max - VALUE_REGRET;
	    }
	}
    }

    temp = boardNodes[player->location];
    char movesOffset = 0;
    char maxIndex;

    for(;;) {
	max = -128;
	maxIndex = -1;
	for(int i = 0; i < temp->connections; i++) {
	    if(temp->edges[i]->locked && !(player->key || player->teleports)) {
		continue;
	    } else if(temp->edges[i]->cost < 0 && !player->teleports) {
		continue;
	    }

	    // could account for ties here
	    if(values[temp->edges[i]->nextNodeIndex] > max) {
		max = values[temp->edges[i]->nextNodeIndex];
		maxIndex = i;
	    }
	}

	//could account for picking up artifact here

	if(temp->edges[maxIndex]->cost < 0) {
	    player->teleports--;
	} else if(temp->edges[maxIndex]->locked && !player->key && player->teleports) {
	    player->teleports--;
	} else if(temp->cave && (movesOffset > 0) && player->teleports) {
	    player->teleports--;
	} else if(temp->cave && (movesOffset > 0) && !player->teleports) {
	    break;
	} else if(temp->edges[maxIndex]->cost <= player->boots) {
	    player->boots -= temp->edges[maxIndex]->cost;
	} else if(player->teleports) {
	    player->teleports--;
	} else {
	    break;
	}

	moves[movesOffset] = temp->edges[maxIndex]->nextNodeIndex;
	movesOffset++;
	temp = boardNodes[temp->edges[maxIndex]->nextNodeIndex];
    }

    moves[movesOffset] = -1;

    printf("moves:");
    for(int i = 0; i < MAX_MOVES; i++) {
	printf("%d ", moves[i]);
    }
    printf("\n");

}

void initValues(player_t* player, char* values) {
    if(player->artifact) {
	values[0] = 100;
	for(int i = 1; i < BOARD_TOTAL_NODES; i++) {
	    values[i] = 0;
	}
    } else {
	for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	    if(boardNodes[i]->artifact) {
		values[i] = boardNodes[i]->artifact;
	    } else if(boardNodes[i]->monkeys) {
		values[i] = boardNodes[i]->monkeys*MONKEY_VALUE;
	    } else {
		values[i] = 0;
	    }
	}
    }
}

char updateValues(char* values, char* tempValues) {
    char done = 1;
    for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	if(tempValues[i] != values[i]) {
	    done = 0;
	    values[i] = tempValues[i];
	}
    }
    return done;
}

int specActInLocationAI(player_t* player) {
    board_node_t* node = boardNodes[player->location];

    if(node->bigSecret) {
        return ACT_BS;
    } else if(node->littleSecret) {
        return ACT_LS;
    } else if(node->monkeys) {
        return ACT_MONKEY;
    } else if(node->artifact) {
	// could account for other, potentially more valuable artifacts
	return ACT_ARTIFACT;
    } else if(node->store) {
	// could account for other items
	if(player->coins >= 7) {
	    return ACT_BUY_CROWN;
	}
    }

    return -1;
}

// [goblins, index, index, index.... -1]
void specActKillMonstersAI(player_t* player, char* monsters) {
    char swords = player->swords;
    char offset = 1;

    for(int i = 0; i < STORE_SIZE; i++) {
	// could account for specific killings
	if((gameObject->store[i]->type == MONSTER) && (gameObject->store[i]->swords <= swords)) {
	    swords -= gameObject->store[i]->swords;
	    monsters[offset] = i;
	    offset++;
	}
    }

    monsters[0] = swords / 2;

    monsters[offset] = -1;
}


// [index, index, index ... -1]
void specActBuyCardsAI(player_t* player, char* cards) {
    // could be reworked to favor certain conditions, or approximate value based on deck
    char maxPoints = 0;
    char maxIndex = -1;
    char blue = player->blue;
    char offset = 0;

    for(int i = 0; i < STORE_SIZE; i++) {
	if(gameObject->store[i] == NULL ||
	   gameObject->store[i]->type == MONSTER ||
	   gameObject->store[i]->price > blue ||
	  (gameObject->store[i]->deep && (player->location <= 15))) {
	    continue;
	}

	if(gameObject->store[i]->points > maxPoints) {
	    maxPoints = gameObject->store[i]->points;
	    maxIndex = i;
	}
    }

    if(maxIndex != -1) {
	cards[offset] = maxIndex;
	offset++;
	blue -= gameObject->store[maxIndex]->price;
    }

    if(!player->artifact && (player->location <= 15)) {
	tryToBuyMercenary(&offset, &blue, cards);
	tryToBuyExplore(&offset, &blue, cards);
    } else if(!player->artifact) {
	tryToBuyExplore(&offset, &blue, cards);
	tryToBuyExplore(&offset, &blue, cards);
	tryToBuyExplore(&offset, &blue, cards);
    } else {
	tryToBuyExplore(&offset, &blue, cards);
	tryToBuySecretTome(&offset, &blue, cards);
    }

    for(int i = 0; i < STORE_SIZE; i++) {
	if((i != maxIndex) &&
	   (gameObject->store[i] != NULL) &&
	   (gameObject->store[i]->type != MONSTER) &&
	   (blue >= gameObject->store[i]->price)) {
	    cards[offset] = i;
	    offset++;
	    blue -= gameObject->store[i]->price;
	}
    }

    cards[offset] = -1;
}


//temp functions
void tryToBuyMercenary(char* offset, char* blue, char* cards) {
    if((*blue >= libraryCards[1]->price) && gameObject->mercenaryLeft) {
	cards[*offset] = MERCENARY_ID;
	(*offset)++;
	*blue -= libraryCards[1]->price;
    }
}

void tryToBuyExplore(char* offset, char* blue, char* cards) {
    if((*blue >= libraryCards[0]->price) && gameObject->exploreLeft) {
	cards[*offset] = EXPLORE_ID;
	(*offset)++;
	*blue -= libraryCards[0]->price;
    }
}

void tryToBuySecretTome(char* offset, char* blue, char* cards) {
    if((*blue >= libraryCards[2]->price) && gameObject->secretTomesLeft) {
	cards[*offset] = SECRET_TOME_ID;
	(*offset)++;
	*blue -= libraryCards[2]->price;
    }
}

