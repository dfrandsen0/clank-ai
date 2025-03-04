#ifndef PLAYER_H_
#define PLAYER_H_

typedef struct card_t card_t;

#include "config.h"
#include "card.h"

typedef enum player_type {
    HUMAN,
    AI
} player_type;

typedef enum player_status {
    ALIVE,
    FINISHED,
    DEAD,
    DEATH_TIMER
} player_status;

typedef struct player_t {
    player_type type;
    player_status status;
    char id;
    char blue;
    char boots;
    char swords;
    char coins;
    char clank;
    char clankStartofTurn;
    char clankLeft;
    char clankInBag;
    char swagger;
    char lives;
    short points;
    char companion;
    char heartBottles;
    char bigHeartBottles;
    char swordBottles;
    char bootBottles;
    char key;
    char backpacks;
    char crown;
    char artifact;
    char monkey;
    char chalice;
    char egg;
    char location;
    char teleports;
    card_t** deck;
    card_t** discard;
    card_t** hand;
    card_t** deckIndex;
    card_t** discardIndex;
    card_t** handIndex;
    int (*evaluateCard)(struct player_t* player, card_t* card);
    void (*findMove)(struct player_t* player, char* moves);
    int (*actInLocation)(struct player_t* player);
    void (*actKillMonsters)(struct player_t* player, char* monsters);
    void (*actBuyCards)(struct player_t* player, char* cards);
} player_t;

player_t* createPlayer(player_type type, char id);
void cleanUpPlayer();
void printPlayer(player_t* player);

void addStartingCards(player_t* player);
void printDeck(card_t** deck);
void printHand(player_t* player);
void shuffleDeck(card_t** deck);
void recycleDeck(player_t* player);

void drawCards(player_t* player, int num);
void discardAll(player_t* player);
void discardCard(player_t* player, int num);

void addFinalPoints(player_t* player);
char cardInDiscard(player_t* player, char* name);

#endif
