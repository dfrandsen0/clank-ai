#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "details.h"
#include "player.h"
#include "game.h"

void specWatcher(player_t* player, char choice) {
    for(int i = 0; i < NUM_PLAYERS; i++) {
	if(gameObject->players[i]->id != player->id) {
	    gameObject->players[i]->clank += 1;
	    gameObject->players[i]->clankLeft -= 1;
	}
    }
}

void specSwagger(player_t* player, char choice) {
    player->swagger = 1;
}

void specArchaeologist(player_t* player, char choice) {
    if(player->monkey) {
	player->blue += 2;
    }
}

void specRebel(player_t* player, char choice) {
    if(player->companion) {
	drawCards(player, 1);
    } else {
	int i = 0;
	for(; i < MAX_HAND; i++) {
	    if(strstr(player->hand[i]->name, "Rebel")) {
		i++;
		break;
	    }
	}

	for(; i < MAX_HAND; i++) {
	    if(player->hand[i] == NULL) {
		break;
	    }

	    if(player->hand[i]->companion) {
		drawCards(player, 1);
	    }
	}
    }
}

void specTreasureHunter(player_t* player, char choice) {
    if(choice < 0) {
	choice = 0;
    } else if(choice > STORE_SIZE - 1) {
	choice = STORE_SIZE - 1;
    }

    if(*(gameObject->storeIndex) != NULL) {
	if(gameObject->store[choice]->danger) {
	    gameObject->extraCubes--;
	}

	gameObject->store[choice] = *(gameObject->storeIndex);
	gameObject->storeIndex++;

	if(gameObject->store[choice]->danger) {
	    gameObject->extraCubes++;
	}

	if(gameObject->store[choice]->arrive) {
	    for(int i = 0; i < NUM_PLAYERS; i++) {
		gameObject->players[i]->clank++;
		gameObject->players[i]->clankLeft--;
	    }
	}
    }
}

void specMasterBurglar(player_t* player, char choice) {
    gameObject->afterFuncs[gameObject->afterFuncsIndex] = &afterFuncMasterBurglar;
    gameObject->afterFuncsIndex++;
}

void specTattle(player_t* player, char choice) {
    for(int i = 0; i < NUM_PLAYERS; i++) {
	if(gameObject->players[i]->id != player->id) {
	    gameObject->players[i]->clank += 1;
	    gameObject->players[i]->clankLeft -= 1;
	}
    }
}

void specKoboldMerchant(player_t* player, char choice) {
    if(player->artifact) {
	player->blue += 2;
    }
}

void specSearch(player_t* player, char choice) {
    for(int i = 0; i < MAX_HAND; i++) {
	if(!strcmp(player->hand[i]->name, "Search")) {
	    break;
	}

	if(player->hand[i]->coins > 0) {
	    player->coins++;
	}
    }

    for(int i = 0; i < NUM_TOTAL_CARDS; i++) {
	if(allCards[i] != NULL && allCards[i]->coins > 0) {
	    allCards[i]->coins++;
	}
    }

    gameObject->afterFuncs[gameObject->afterFuncsIndex] = &afterFuncSearch;
    gameObject->afterFuncsIndex++;
}

void specInvokeroftheAncients(player_t* player, char choice) {
    player->teleports++;
}

void specGemCollector(player_t* player, char choice) {
    for(int i = 0; i < NUM_GEM_CARDS; i++) {
	gemCards[i]->price -= 2;
    }

    gameObject->afterFuncs[gameObject->afterFuncsIndex] = &afterFuncGemCollector;
    gameObject->afterFuncsIndex++;
}

void specWandofRecall(player_t* player, char choice) {
    if(player->artifact) {
	player->teleports++;
    }
}

void specTheMountainKing(player_t* player, char choice) {
    if(player->crown) {
	player->swords++;
	player->boots++;
    }
}

void specTheQueenofHearts(player_t* player, char choice) {
    if(player->crown) {
	player->lives++;

	if(player->lives > STARTING_LIVES) {
	    player->lives = STARTING_LIVES;
	} else {
	    player->clankLeft += 1;
	}
    }
}

void specWandofWind(player_t* player, char choice) {
    player->teleports++;
}

void afterFuncGemCollector(player_t* player) {
    for(int i = 0; i < NUM_GEM_CARDS; i++) {
	gemCards[i]->price += 2;
    }
}

void afterFuncMasterBurglar(player_t* player) {
    for(int i = 0; i < MAX_DECK; i++) {
	if(player->discard[i] == NULL) {
	    break;
	}

	if(!strcmp(player->discard[i]->name, "Burgle")) {
	    player->discard[i] = NULL;
	    player->discardIndex -= 1;

	    if(*(player->discardIndex) != NULL) { //if null, burgle was last card
		player->discard[i] = *(player->discardIndex);
		*(player->discardIndex) = NULL;
		return;
	    }
	}
    }

    for(int i = 0; i < MAX_HAND; i++) {
	if(player->hand[i] == NULL) {
	    break;
	}

	if(!strcmp(player->hand[i]->name, "Burgle")) {
	    player->hand[i] = NULL;
	    player->handIndex -= 1;

	    if(*(player->handIndex) != NULL) {
		player->hand[i] = *(player->handIndex);
		*(player->handIndex) = NULL;
		return;
	    }
	}
    }
}

void afterFuncSearch(player_t* player) {
    for(int i = 0; i < NUM_TOTAL_CARDS; i++) {
	if(allCards[i] != NULL && allCards[i]->coins > 0) {
	    allCards[i]->coins--;
	}
    }
}

void afterFuncSwagger(player_t* player) {
    player->swagger = 0;
}
