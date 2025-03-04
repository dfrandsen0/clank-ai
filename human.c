#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "board.h"
#include "human.h"
#include "card.h"
#include "game.h"

int specEvaluateCardHuman(player_t* player, card_t* card) {
    if(!strcmp(card->name, "Treasure Hunter")) {
	printStore();

        printf("Which card to replace:");
	fflush(stdout);
	int choice;
	for(;;) {
	    choice = getchar();
	    choice -= 48;
	    if(choice>= 0 && choice <= 5) {
		if(gameObject->store[choice] != NULL) {
		    return choice;
		}
	    }
	}
    }

    return 0;
}

void specFindMoveHuman(player_t* player, char* moves) {
    printf("Provide Movement: \n");
    printf("location: %d, boots: %d, teleports: %d\n", player->location, player->boots, player->teleports);

    //assumes you don't cheat
    fillArray(moves);
}

void humanMoveHelper(player_t* player, char* moves) {

}

int specActInLocationHuman(player_t* player) {
    board_node_t* node = boardNodes[player->location];

    if(node->artifact) {
        return ACT_ARTIFACT;
    } else if(node->bigSecret) {
        return ACT_BS;
    } else if(node->littleSecret) {
        return ACT_LS;
    } else if(node->monkeys) {
        return ACT_MONKEY;
    } else if(node->store && player->coins > STORE_PRICE) {
	printf("Entered store. Buy something?\n");
	printf("[0] None\n");
	if(gameObject->keys) {
	    printf("[1] Key (%d left)\n", gameObject->keys);
	}

	if(gameObject->backpacks) {
	    printf("[2] Backpack (%d left)\n", gameObject->backpacks);
	}

	if(gameObject->crowns) {
	    printf("[3] Crown %d (%d left)\n", gameObject->nextCrownValue, gameObject->crowns);
	}

	int choice;
	for(;;) {
	    choice = getchar();
	    choice -= 48;

	    if(choice >= 0 && choice <= 3) {
		switch(choice) {
		    case 0:
			return -1;
		    case 1:
			return ACT_BUY_KEY;
		    case 2:
			return ACT_BUY_BACKPACK;
		    default: //3
			return ACT_BUY_CROWN;
		}
	    }
	}

    }

    return -1;
}

void specActKillMonstersHuman(player_t* player, char* monsters) {
    printf("Kill Monsters: \n");
    printf("swords: %d\n", player->swords);

    //assumes you don't cheat
    fillArray(monsters);

}

void specActBuyCardsHuman(player_t* player, char* cards) {
    printf("Buy Cards: \n");
    printf("blue: %d\n", player->blue);

    //assumes you don't cheat
    fillArray(cards);
}

void fillArray(char* container) {
    int temp;
    int offset = 0;

    fflush(stdin);
    for(;;) {
	scanf("%d", &temp);
	container[offset] = (char)temp;

	if(container[offset] == -1) {
	    break;
	}

	offset++;
    }

}
