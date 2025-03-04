#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "card.h"
#include "player.h"
#include "game.h"
#include "config.h"
#include "board.h"

game_t* gameObject;
char aiOverallPath[12];

void setupGame(player_type* types) {
    initRand();

    createCards();
    createBoard();

    gameObject = createGame();
    fillStore(0);

    char path[12] = {0, 1, 2, 7, 12, 17, 12, 7, 2, 1, 0, 0};
    memcpy(aiOverallPath, path, 12);

    for(int i = 0; i < NUM_PLAYERS; i++) {
	gameObject->players[i] = createPlayer(types[i], i);
	drawCards(gameObject->players[i], 5);
    }
}

game_t* createGame() {
    game_t* newGame = (game_t*)malloc(sizeof(game_t));
    memset(newGame, 0, sizeof(game_t));
    newGame->deathLevel = -1;
    newGame->dragonLevel = START_DRAGON_LEVEL;
    newGame->dragonCubes = NUM_DRAGON_CUBES;
    newGame->extraCubes = 0;

    newGame->backpacks = STARTING_BACKPACKS;
    newGame->crowns = STARTING_CROWNS;
    newGame->nextCrownValue = START_CROWN_VALUE;
    newGame->keys = STARTING_KEYS;

    newGame->monkeys = STARTING_MONKEYS;

    newGame->exploreLeft = NUM_EXPLORE;
    newGame->mercenaryLeft = NUM_MERCENARY;
    newGame->secretTomesLeft = NUM_SECRET_TOMES;

    big_secret_type secrets[NUM_BIG_SECRETS] = {BS_BLUE, BS_BLUE, BS_CHALICE, BS_CHALICE, BS_CHALICE,
		   BS_CARDS, BS_CARDS, BS_HEARTS, BS_HEARTS, BS_COINS, BS_COINS};

    little_secret_type secrets2[NUM_LITTLE_SECRETS] =
		{LS_TRASH, LS_TRASH, LS_BLUE, LS_BLUE, LS_BLUE, LS_COINS,
		LS_COINS, LS_COINS, LS_HEART, LS_HEART, LS_HEART, LS_EGG,
		LS_EGG, LS_EGG, LS_SWORDS, LS_SWORDS, LS_BOOT, LS_BOOT};

    shuffleSecrets((int*)secrets, NUM_BIG_SECRETS);
    shuffleSecrets((int*)secrets2, NUM_LITTLE_SECRETS);

    newGame->bigSecrets = malloc(sizeof(big_secret_type) * NUM_BIG_SECRETS);
    newGame->littleSecrets = malloc(sizeof(little_secret_type) * NUM_LITTLE_SECRETS);

    memcpy(newGame->bigSecrets, secrets, sizeof(secrets));
    memcpy(newGame->littleSecrets, secrets2, sizeof(secrets2));

    newGame->players = malloc(sizeof(player_t*) * 2);

    prepareStore(&newGame);

    newGame->afterFuncs = malloc(sizeof(void*) * MAX_AFTER_FUNCS);
    for(int i = 0; i < MAX_AFTER_FUNCS; i++) {
	newGame->afterFuncs[i] = NULL;
    }

    return newGame;
}

void shuffleSecrets(int* secrets, int size) {
    int temp, first, second;
    for(int i = 0; i < SHUFFLE_POWER; i++) {
	first = second = rand() % size;
	while(second == first) {
	    second = rand() % size;
	}

	temp = secrets[first];
	secrets[first] = secrets[second];
	secrets[second] = temp;
    }
}

void prepareStore(game_t** game) {
    (*game)->storeDeck = malloc(sizeof(card_t*) * MAX_DECK);

    for(int i = 0; i < MAX_DECK; i++) {
	*((*game)->storeDeck + i) = NULL;
    }

    (*game)->storeIndex = (*game)->storeDeck;

    for(int i = 0; i < NUM_TOTAL_CARDS; i++) {
	if(allCards[i] == NULL) {
	    break;
	}

	if(allCards[i]->type == BASIC || allCards[i]->type == LIBRARY || allCards[i]->type == GOBLIN) {
	    continue;
	}

	for(int j = 0; j < allCards[i]->quantity; j++) {
	    *((*game)->storeIndex) = allCards[i];
	    ((*game)->storeIndex)++;
	}
    }

    shuffleDeck((*game)->storeDeck);
    shuffleDeck((*game)->storeDeck);

    (*game)->storeIndex = (*game)->storeDeck;

    (*game)->store = malloc(sizeof(card_t*) * STORE_SIZE);

    for(int i = 0; i < STORE_SIZE; i++) {
	*((*game)->store + i) = NULL;
    }
}

char fillStore(char attack) {
    char attackHappened = 0;
    for(int i = 0; i < STORE_SIZE; i++) {
	if(*(gameObject->storeIndex) == NULL) {
	    break;
	}

	if(gameObject->store[i] == NULL) {
	    gameObject->store[i] = *(gameObject->storeIndex);

	    if((attack) && (gameObject->store[i]->dragon)) {
		attackHappened = 1;
	    }

	    if((attack) && (gameObject->store[i]->arrive)) {
		for(int i = 0; i < NUM_PLAYERS; i++) {
		    gameObject->players[i]->clank += 1;
		    gameObject->players[i]->clankLeft -= 1;
		}
	    }

	    if(gameObject->store[i]->danger) {
		gameObject->extraCubes++;
	    }


	    gameObject->storeIndex++;
	}
    }

    return attackHappened;
}

char checkGame(player_t* current_player) {
    if(gameObject->deathLevel >= 4) {
	return 1;
    }

    char done = 1;
    for(int i = 0; i < NUM_PLAYERS; i++) {
	if(gameObject->players[i]->status != ALIVE) {
	    continue;
	}

	if(gameObject->players[i]->lives == 0) {
	    gameObject->players[i]->status = DEAD;
	} else if((gameObject->players[i]->artifact) &&
		  (gameObject->players[i]->location == 0)) {
	    if(gameObject->deathLevel == -1) {
		gameObject->players[i]->status = DEATH_TIMER;
		gameObject->deathLevel = 0;
	    } else {
		gameObject->players[i]->status = FINISHED;
	    }
	} else {
	    done = 0;
	}
    }

    if(done) {
	return done;
    }

    char deathAttack = 0;
    for(int i = 0; i < NUM_PLAYERS; i++) {
	if((gameObject->players[i]->status == DEATH_TIMER) &&
	   (gameObject->players[i]->id == current_player->id)) {
	    deathAttack = 1;
	}
    }

    if(fillStore(1) || deathAttack) {
	printf("\nDragon Attacks!\n");
	char clanks[NUM_PLAYERS + 1];
	for(int i = 0; i < NUM_PLAYERS; i++) {
	    gameObject->players[i]->clankInBag += gameObject->players[i]->clank;
	    clanks[i] = gameObject->players[i]->clankInBag;
	    gameObject->players[i]->clank = 0;
	}

	clanks[NUM_PLAYERS] = gameObject->dragonCubes;

	char cubesLeft = cubesToPull(gameObject->dragonLevel, gameObject->extraCubes);

	char playerHit;
	char sumClanks = sumArray(clanks, NUM_PLAYERS + 1);

	for(int i = 0; i < cubesLeft; i++) {
	    playerHit = 0;
	    char hit = rand() % sumClanks;
	    for(int j = 0; j < NUM_PLAYERS; j++) {
		if(hit < clanks[j]) {
		    clanks[j]--;
		    (gameObject->players[j]->lives)--;
		    playerHit = j + 1;
		    break;
		} else {
		    hit -= clanks[j];
		}
	    }

	    if(!playerHit) {
		clanks[NUM_PLAYERS] -= 1;
		printf("Dragon Hit\n");
	    } else {
		printf("Player %d Hit\n", playerHit - 1);
	    }

	    sumClanks--;
	}
    }

// may do one extra turn;
    for(int i = 0; i < NUM_PLAYERS; i++) {
	if(gameObject->players[i]->status != ALIVE) {
	    continue;
	}

	if(gameObject->players[i]->lives <= 0) {
	    gameObject->players[i]->status = DEAD;
	}
    }

    return done;
}

void printStore() {
    for(int i = 0; i < STORE_SIZE; i++) {
	if(gameObject->store[i] == NULL) {
	    printf("[Empty]\n");
	} else {
	    printf("[%d] ", i);
	    printCard(gameObject->store[i]);
	}
    }
}

void cleanUpGame() {
    cleanUpCards();

    for(int i = 0; i < NUM_PLAYERS; i++) {
	cleanUpPlayer(gameObject->players[i]);
    }

    cleanUpBoard();

    free(gameObject->storeDeck);
    free(gameObject->store);
    free(gameObject->bigSecrets);
    free(gameObject->littleSecrets);
    free(gameObject->players);
    free(gameObject->afterFuncs);
    free(gameObject);

}

player_t* whoGoesFirst() {
    return gameObject->players[rand() % NUM_PLAYERS];
}

void nextPlayer(player_t** current) {
    if((*current)->id == NUM_PLAYERS - 1) {
        *current = gameObject->players[0];
    } else {
        *current = gameObject->players[(*current)->id + 1];
    }
}

int cubesToPull(char level, char extra) {
    if(level < 2) {
	return 2 + extra;
    } else if(level < 4) {
	return 3 + extra;
    } else if(level < 6) {
	return 4 + extra;
    } else {
	return 5 + extra;
    }
}

char sumArray(char* set, char size) {
    char sum = 0;
    for(int i = 0; i < size; i++) {
	sum += set[i];
    }
    return sum;
}
