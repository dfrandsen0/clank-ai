#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "card.h"
#include "details.h"
#include "config.h"
#include "player.h"
#include "game.h"
#include "turn.h"
#include "board.h"

void waitForInput();

int main(int arc, char **argv) {
    player_type types[NUM_PLAYERS] = {AI, AI};
    setupGame(types);

    player_t* current = whoGoesFirst();

    for(;;) {
	//waitForInput();
	takeTurn(current);
	printf("End of %d Turn\n", current->id);

	if(checkGame(current)) {
	    break;
	}

	nextPlayer(&current);
    }

    printf("It is finished.\n");

    player_t* next;
    char winner = -1;
    char max = -1;
    for(int i = 0; i < NUM_PLAYERS; i++) {
	next = gameObject->players[i];
	addFinalPoints(next);
	if(next->type == HUMAN) {
	    printf("Player ");
	} else {
	    printf("AI ");
	}

	printf("%d scored %d points!\n", next->id, next->points);
	if(next->points >= max) {
	    max = next->points;
	    winner = next->id;
	}
    }

    printf("%d wins!\n", winner);

    cleanUpGame();
}

void waitForInput() {
    unsigned char temp;
    read(0, &temp, 1);

    fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);

    while(read(0, &temp, 1) > 0) {}

    fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~O_NONBLOCK);
}
