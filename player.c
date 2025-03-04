#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "player.h"
#include "card.h"
#include "turn.h"
#include "board.h"
#include "human.h"
#include "ai.h"

player_t* createPlayer(player_type type, char id) {
    player_t* newPlayer = malloc(sizeof(player_t));
    memset(newPlayer, 0, sizeof(player_t));

    newPlayer->id = id;
    newPlayer->type = type;
    newPlayer->status = ALIVE;

    if(type == HUMAN) {
	newPlayer->evaluateCard = &specEvaluateCardHuman;
	newPlayer->findMove = &specFindMoveHuman;
	newPlayer->actInLocation = &specActInLocationHuman;
	newPlayer->actKillMonsters = &specActKillMonstersHuman;
	newPlayer->actBuyCards = &specActBuyCardsHuman;
    } else {
	newPlayer->evaluateCard = &specEvaluateCardAI;
	newPlayer->findMove = &specFindMoveAI;
	newPlayer->actInLocation = &specActInLocationAI;
	newPlayer->actKillMonsters = &specActKillMonstersAI;
	newPlayer->actBuyCards = &specActBuyCardsAI;
    }


    newPlayer->lives = STARTING_LIVES;
    newPlayer->clankLeft = NUM_PLAYER_CUBES;

    newPlayer->deck = (card_t**)malloc(sizeof(card_t*) * MAX_DECK);
    newPlayer->discard = (card_t**)malloc(sizeof(card_t*) * MAX_DECK);

    for(int i = 0; i < MAX_DECK; i++) {
	*(newPlayer->deck + i) = NULL;
	*(newPlayer->discard + i) = NULL;
    }

    newPlayer->hand = (card_t**)malloc(sizeof(card_t*) * MAX_HAND);

    for(int i = 0; i < MAX_HAND; i++) {
	*(newPlayer->hand + i) = NULL;
    }

    newPlayer->deckIndex = newPlayer->deck;
    newPlayer->discardIndex = newPlayer->discard;
    newPlayer->handIndex = newPlayer->hand;

    addStartingCards(newPlayer);

    return newPlayer;
}

void cleanUpPlayer(player_t* player) {
    free(player->deck);
    free(player->discard);
    free(player->hand);
    free(player);
}

void printPlayer(player_t* player) {
    printf("blue: %d\n", player->blue);
    printf("boots: %d\n", player->boots);
    printf("swords: %d\n", player->swords);
    printf("coins: %d\n", player->coins);
    printf("clank: %d\n", player->clank);
    printf("lives: %d\n", player->lives);
    printf("clankLeft: %d\n", player->clankLeft);
    printf("points: %d\n", player->points);
    printf("companion: %d\n", player->companion);
    printf("heartBottles: %d\n", player->heartBottles);
    printf("swordBottles: %d\n", player->swordBottles);
    printf("bootBottles: %d\n", player->bootBottles);
    printf("key: %d\n", player->key);
    printf("backpacks: %d\n", player->backpacks);
    printf("crown: %d\n", player->crown);
    printf("artifact: %d\n", player->artifact);
    printf("monkey: %d\n", player->monkey);
    printf("chalice: %d\n", player->chalice);
    printf("egg: %d\n", player->egg);
    printf("location: %d\n", player->location);

    printf("totalCardsPlayed: %ld\n", player->handIndex - player->hand);

}

void addStartingCards(player_t* player) {
    int count = 0;
    for(int card = 0; card < NUM_BASIC_CARDS; card++) {
	for(int num = 0; num < basicCards[card]->quantity; num++) {
	    player->deck[count] = basicCards[card];
	    count++;
	}
    }

    shuffleDeck(player->deck);
}

void printDeck(card_t** deck) {
    for(int i = 0; i < MAX_DECK; i++) {
        if(deck[i] == NULL) {
            continue;
        }

        printCard(deck[i]);
    }
}

void printHand(player_t* player) {
    for(int i = 0; i < MAX_HAND; i++) {
	if(player->hand[i] == NULL) {
	    break;
	}

	printCard(player->hand[i]);
    }
}

void shuffleDeck(card_t** deck) {
    if(*deck == NULL) {
	return;
    }

    int endOfDeck = 0;

    while(deck[endOfDeck] != NULL) {
	endOfDeck++;
    }

    card_t* temp;
    int first, second;
    for(int i = 0; i < SHUFFLE_POWER; i++) {
	first = second = rand() % endOfDeck;
	while(second == first) {
	    second = rand() % endOfDeck;
	}

	temp = deck[first];
	deck[first] = deck[second];
	deck[second] = temp;
    }
}

void recycleDeck(player_t* player) {
    card_t** tempDeck = player->deck;

    player->deck = player->discard;
    player->discard = tempDeck;

    shuffleDeck(player->deck);
    player->deckIndex = player->deck;
    player->discardIndex = player->discard;
}

void drawCards(player_t* player, int num) {
    for(int i = 0; i < num; i++) {
	if(*(player->deckIndex) == NULL) {
	    recycleDeck(player);
	}

	if(*(player->deckIndex) == NULL) {
	    break;
	}

	*(player->handIndex) = *(player->deckIndex);
	player->handIndex++;

	*(player->deckIndex) = NULL;
	player->deckIndex++;
    }
}

void discardAll(player_t* player) {
    for(int i = 0; player->hand[i] != NULL; i++) {
	*(player->discardIndex) = player->hand[i];
	player->discardIndex++;
	player->hand[i] = NULL;
    }

    player->handIndex = player->hand;
}

// assumes you make no mistakes
void discardCard(player_t* player, int num) {
    *(player->discardIndex) = player->hand[num];
    player->discardIndex++;
    player->hand[num] = NULL;

    player->handIndex -= 1;

    if(*(player->handIndex) != NULL) {
	player->hand[num] = *(player->handIndex);
	*(player->handIndex) = NULL;
    }
}

void addFinalPoints(player_t* player) {
    if(player->location > 15) {
	player->points = 0;
	return;
    }

    for(int i = 0; i < MAX_DECK; i++) {
	if(player->deck[i] == NULL) {
	    continue;
	}

	*(player->discardIndex) = player->deck[i];
	player->discardIndex++;
    }

    for(int i = 0; i < MAX_HAND; i++) {
	if(player->hand[i] == NULL) {
	    break;
	}

	*(player->discardIndex) = player->hand[i];
	player->discardIndex++;
    }

    if(player->location == 0) {
	player->points += 20;

	if(cardInDiscard(player, "Dragon's Eye")) {
	    player->points += 10;
	}
    }


    if(cardInDiscard(player, "Dwarven Peddler")) {
	if((player->egg + player->chalice + player->monkey) >= 2) {
	    player->points += 4;
	}
    }

    if(cardInDiscard(player, "The Duke")) {
	player->points += (player->coins / 5);
    }

    if(cardInDiscard(player, "Wizard")) {
	for(int i = 0; i < MAX_DECK; i++) {
	    if(player->discard[i] == NULL) {
		break;
	    }

	    if(!strcmp(player->discard[i]->name, "Secret Tome")) {
		player->points += 2;
	    }
	}
    }

    player->points += player->coins;

}

char cardInDiscard(player_t* player, char* name) {
    for(int i = 0; i < MAX_DECK; i++) {
	if(player->discard[i] == NULL) {
	    break;
	}

	if(!strcmp(player->discard[i]->name, name)) {
	    return 1;
	}
    }

    return 0;
}
