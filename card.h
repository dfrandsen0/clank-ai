#ifndef CARD_H_
#define CARD_H_

typedef struct player_t player_t;

#include "config.h"
#include "player.h"

typedef enum card_type {
    BASIC,
    LIBRARY,
    GOBLIN,
    MONSTER,
    REGULAR
} card_type;

typedef struct card_t {
    unsigned char name[CARD_NAME_LENGTH];
    card_type type;
    char quantity;
    char blue;
    char boots;
    char swords;
    char coins;
    char clank;
    char price;
    char points;
    char cards;
    char companion;
    char deep;
    char cave;
    char dragon;
    char gem;
    char arrive;
    char danger;
    char acquireHeart;
    char acquireBoot;
    char acquireSword;
    char acquireClank;
    void (* action)(player_t* player, char choice);
} card_t;

void createCards();
void createCardSet(card_t** set, card_type type);
void createGemSet();

void addCard(unsigned char* buf);
void addType(card_t* card, unsigned char* start, unsigned char* end);
void addAction(card_t* card);

void printAllCards();
void printCard(card_t* card);

void cleanUpCards();

extern card_t* allCards[NUM_TOTAL_CARDS];
extern card_t* basicCards[NUM_BASIC_CARDS];
extern card_t* libraryCards[NUM_LIBRARY_CARDS];
extern card_t* gemCards[NUM_GEM_CARDS];

#endif
