#ifndef GAME_H_
#define GAME_H_

#include <stdio.h>
#include "card.h"
#include "player.h"

typedef enum big_secret_type {
    BS_BLUE,
    BS_CHALICE,
    BS_CARDS,
    BS_HEARTS,
    BS_COINS
} big_secret_type;

typedef enum little_secret_type {
    LS_TRASH,
    LS_BLUE,
    LS_COINS,
    LS_HEART,
    LS_EGG,
    LS_SWORDS,
    LS_BOOT
} little_secret_type;

typedef struct game_t {
    char deathLevel;
    char dragonLevel;
    char dragonCubes;
    char extraCubes;
    char backpacks;
    char crowns;
    char nextCrownValue;
    char keys;
    char monkeys;
    char exploreLeft;
    char mercenaryLeft;
    char secretTomesLeft;
    big_secret_type* bigSecrets;
    little_secret_type* littleSecrets;
    player_t** players;
    card_t** storeDeck;
    card_t** storeIndex;
    card_t** store;
    void (**afterFuncs)(player_t* player);
    int afterFuncsIndex;
} game_t;

void setupGame(player_type* types);
game_t* createGame();
void shuffleSecrets(int* secrets, int size);
void prepareStore(game_t** game);
char fillStore(char attack);
char checkGame(player_t* current_player);
void printStore();
void cleanUpGame();
//  - Dragon attacks

player_t* whoGoesFirst();
void nextPlayer(player_t** current);

int cubesToPull(char level, char extra);
char sumArray(char* set, char size);

extern game_t* gameObject;
extern char aiOverallPath[12];

#endif

