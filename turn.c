#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "turn.h"
#include "board.h"
#include "player.h"
#include "details.h"
#include "game.h"
#include "config.h"


void takeTurn(player_t* player) {
    if(player->status != ALIVE) {
	if(player->status == DEATH_TIMER) {
	    gameObject->deathLevel += 1;
	    gameObject->extraCubes += 1;
	}

	return;
    }

    if(player->type == HUMAN) {
	printf("player %d going\n", player->id);
    } else {
	printf("ai %d going\n", player->id);
    }

    printf("Hand:\n");
    printHand(player);
    printf("\n");

    playCards(player);
    printf("\n");
    printPlayer(player);
    printf("\n");

    move(player);
    printf("Final location: %d\n\n", player->location);

    printf("Store:\n");
    printStore();
    printf("\n");

    killMonsters(player);
    printf("\n");

    if(player->swagger && (player->clank > player->clankStartofTurn)) {
	player->blue += (player->clank - player->clankStartofTurn);
    }

    buyCards(player);

    resolveAfterFuncs(player);

    discardAll(player);
    discardResources(player);

    drawCards(player, 5);
}

void playCards(player_t* player) {
    card_t* currCard;
    player->clankStartofTurn = player->clank;

    for(char currCardIndex = 0; player->hand[currCardIndex] != NULL; currCardIndex++) {
	printf("Playing %s\n", player->hand[currCardIndex]->name);

	currCard = player->hand[currCardIndex];

	player->blue += currCard->blue;
	player->boots += currCard->boots;
	player->swords += currCard->swords;
	player->coins += currCard->coins;
	player->clank += currCard->clank;
	player->clankLeft -= currCard->clank;

	if(currCard->cards > 0) {
	    drawCards(player, currCard->cards);
	}

	if(currCard->companion) {
	    player->companion = 1;
	}

	if(currCard->action != NULL) {
	     currCard->action((void*)player, player->evaluateCard(player, currCard));
	}
    }
}

void move(player_t* player) {
    char moves[MAX_MOVES];
    bzero(moves, MAX_MOVES);

    player->findMove(player, moves);

    board_edge_t* tempEdge;
    for(int i = 0; moves[i] != -1 && i < MAX_MOVES; i++) {
	for(int j = 0; j < BOARD_MAX_CONNECTIONS; j++) {
	    tempEdge = boardNodes[player->location]->edges[j];
	    if(tempEdge == NULL) {
		printf("invalid move from %d to %d\n", player->location, moves[i]);
		exit(1);
	    } else {
		if(tempEdge->nextNodeIndex == moves[i]) {
		    break;
		}
	    }
	}

	//not worried about boots because teleports
	player->location = tempEdge->nextNodeIndex;
	player->swords -= tempEdge->enemies;
	if(player->swords < 0) {
	    player->lives -= (player->swords * -1);
	    player->swords = 0;
	}

	printf("Move to %d\n", player->location);
	takeAction(player, player->actInLocation(player));
    }
}

// recieves kills [goblin, index, index, index...]
// where goblin is # times killed goblin
// indecies represent other monsters in store killed
void killMonsters(player_t* player) {
    char monsters[STORE_SIZE + 1];
    for(int i = 0; i < STORE_SIZE + 1; i++) {
	monsters[i] = -1;
    }

    player->actKillMonsters(player, monsters);

    if(monsters[0] < 0) {
	return;
    }

    for(int i = 0; i < monsters[0]; i++) {
	printf("Kill Goblin\n");
    }

    player->coins += monsters[0];

    card_t* temp;
    for(int i = 1; i < STORE_SIZE + 1 && monsters[i] != -1; i++) {
	temp = gameObject->store[monsters[i]];
	printf("Kill %s\n", temp->name);
	player->coins += temp->coins;
	player->clank += temp->clank;
	player->clankLeft -= temp->clank;
	player->blue += temp->blue;

	if(temp->danger) {
	    gameObject->extraCubes -= 1;
	}

	if(temp->action) {
	    temp->action(player, 0);
	}

	gameObject->store[monsters[i]] = NULL;
    }
}

void buyCards(player_t* player) {
    char cards[STORE_SIZE];
    for(int i = 0; i < STORE_SIZE; i++) {
	cards[i] = -1;
    }

    player->actBuyCards(player, cards);

    for(int i = 0; i < STORE_SIZE && cards[i] != -1; i++) {
	if(cards[i] < STORE_SIZE) {
	    card_t* newCard = gameObject->store[cards[i]];
	    *(player->discardIndex) = newCard;
	    gameObject->store[cards[i]] = NULL;
	    player->points += newCard->points;
	    if(newCard->acquireHeart) {
		player->lives++;
		if(player->lives > STARTING_LIVES) {
		    player->lives = STARTING_LIVES;
		} else {
		    player->clankLeft += 1;
		}
	    } else if(newCard->acquireClank) {
		player->clank += newCard->acquireClank;
		player->clankLeft -= newCard->acquireClank;
	    }
	} else if(cards[i] == EXPLORE_ID) {
	    *(player->discardIndex) = libraryCards[EXPLORE_ID - STORE_SIZE];
	    gameObject->exploreLeft--;
	} else if(cards[i] == MERCENARY_ID) {
	    *(player->discardIndex) = libraryCards[MERCENARY_ID - STORE_SIZE];
	    gameObject->mercenaryLeft--;
	} else {
	    *(player->discardIndex) = libraryCards[SECRET_TOME_ID - STORE_SIZE];
	    gameObject->secretTomesLeft--;
	    player->points += libraryCards[SECRET_TOME_ID - STORE_SIZE]->points;
	}

	printf("Bought: %s\n", (*(player->discardIndex))->name);

	player->discardIndex++;
    }
}

void takeAction(player_t* player, act_type action) {

    if(action < 0 || action > 50) {
	return;
    }

    printAction(action);

    board_node_t* node = boardNodes[player->location];

    switch(action) {
	case ACT_ARTIFACT:
	    printf("Took artifact %d out of %d\n", node->artifact, player->location);
	    gameObject->dragonLevel++;
	    player->points += node->artifact;
	    player->artifact++;
	    node->artifact = 0;
	    break;
	case ACT_BS:
	    printf("Took Big Secret ");
	    useBigSecret(player, node);
	    printf(" out of %d\n", player->location);
	    break;
	case ACT_LS:
	    printf("Took Little Secret ");
	    useLittleSecret(player, node);
	    printf(" out of %d\n", player->location);
	    break;
	case ACT_BUY_KEY:
	    printf("Bought Key\n");
	    player->key += 1;
	    gameObject->keys -= 1;
	    player->points += KEY_VALUE;
	    player->coins -= STORE_PRICE;
	    break;
	case ACT_BUY_BACKPACK:
	    printf("Bought Backpack\n");
	    player->backpacks += 1;
	    gameObject->backpacks -= 1;
	    player->points += BACKPACK_VALUE;
	    player->coins -= STORE_PRICE;
	    break;
	case ACT_BUY_CROWN:
	    printf("Bought Crown\n");
	    player->crown += 1;
	    gameObject->crowns -= 1;
	    player->points += gameObject->nextCrownValue;
	    gameObject->nextCrownValue -= 1;
	    player->coins -= STORE_PRICE;
	    break;
	case ACT_MONKEY:
	    player->monkey += 1;
	    player->points += MONKEY_VALUE;
	    gameObject->monkeys -= 1;
	    node->monkeys -= 1;
	    break;
	default:
	    //error!
    }
}

void printAction(act_type action) {
    printf("Taking Action: ");
    switch(action) {
	case ACT_ARTIFACT:
	    printf("Take Artifact");
	    break;
	case ACT_BS:
	    printf("Take Big Secret");
	    break;
	case ACT_LS:
	    printf("Take Little Secret");
	    break;
	case ACT_BUY_KEY:
	    printf("Buy Key");
	    break;
	case ACT_BUY_CROWN:
	    printf("Buy Crown");
	    break;
	case ACT_BUY_BACKPACK:
	    printf("Buy Backpack");
	    break;
	case ACT_MONKEY:
	    printf("Take Monkey");
	    break;
	default:
	    //Error!
    }
    printf("\n");
}

void useBigSecret(player_t* player, board_node_t* node) {
    node->bigSecret = 0;
    int i = 0;

    big_secret_type secret;
    for(int i = 0; i < NUM_BIG_SECRETS; i++) {
	if(gameObject->bigSecrets[i] != -1) {
	    secret = gameObject->bigSecrets[i];
	    gameObject->bigSecrets[i] = -1;
	    break;
	}
    }

    switch(secret) {
	case BS_BLUE:
	    printf("Blue");
	    player->blue += 5;
	    break;
	case BS_CHALICE:
	    printf("Chalice");
	    player->points += 7;
	    player->chalice++;
	    break;
	case BS_CARDS:
	    printf("Cards");
	    // simplified into 5 coins
	    player->coins += 5;
	    break;
	case BS_HEARTS:
	    printf("Hearts");
	    player->bigHeartBottles++;
	    break;
	case BS_COINS:
	    printf("Coins");
	    player->coins += 5;
	    break;
	default:
	    // error!
    }
}

void useLittleSecret(player_t* player, board_node_t* node) {
    node->littleSecret = 0;
    int i = 0;

    little_secret_type secret;
    for(int i = 0; i < NUM_LITTLE_SECRETS; i++) {
        if(gameObject->littleSecrets[i] != -1) {
            secret = gameObject->littleSecrets[i];
            gameObject->littleSecrets[i] = -1;
            break;
        }
    }

    switch(secret) {
	case LS_TRASH:
	    printf("blue");
	    player->blue += 2;
	    break;
	case LS_BLUE:
	    printf("blue");
	    player->blue += 2;
	    break;
	case LS_COINS:
	    printf("coins");
	    player->coins += 2;
	    break;
	case LS_HEART:
	    printf("heart");
	    player->heartBottles++;
	    break;
	case LS_EGG:
	    printf("egg");
	    player->egg++;
	    player->points += 3;
	    gameObject->dragonLevel++;
	    break;
	case LS_SWORDS:
	    printf("swords");
	    player->swordBottles++;
	    break;
	case LS_BOOT:
	    printf("boot");
	    player->bootBottles++;
	    break;
        default:
            // error!
    }
}

void resolveAfterFuncs(player_t* player) {
    for(int i = 0; gameObject->afterFuncs[i] != NULL; i++) {
	gameObject->afterFuncs[i](player);
	gameObject->afterFuncs[i] = NULL;
    }

    gameObject->afterFuncsIndex = 0;
}

void discardResources(player_t* player) {
    player->blue = 0;
    player->boots = 0;
    player->swords = 0;

    if(player->clank < 0) {
	player->clank = 0;
	player->clankLeft = NUM_PLAYER_CUBES;
    }

    if(player->clank > NUM_PLAYER_CUBES) {
	player->clank = NUM_PLAYER_CUBES;
	player->clankLeft = 0;
    }

    player->companion = 0;
}
