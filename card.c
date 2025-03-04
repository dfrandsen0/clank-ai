#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>

#include "card.h"
#include "details.h"

card_t* allCards[NUM_TOTAL_CARDS];
card_t* basicCards[NUM_BASIC_CARDS];
card_t* libraryCards[NUM_LIBRARY_CARDS];
card_t* gemCards[NUM_GEM_CARDS];

char allCardsOffset;

char* cardIndexOrdering[20];

void createCards() {
    allCardsOffset = 0;

    int fd = fileno(fopen("cardsinfo.csv", "r"));

    unsigned char buffer[8192];
    bzero(buffer, 8192);
    int offset = 0;
    int back;

    // read file into memory
    for(;;) {
	back = read(fd, buffer + offset, 8192 - offset);

	if(back == 0) {
	    break;
	}

	offset += back;
    }


    unsigned char lineBuffer[128];
    unsigned char* current = buffer;
    unsigned char* next;
    // Parse each line
    for(;;) {
	bzero(lineBuffer, 128);

	next = strstr(current, "\n");

	if(next == NULL) {
	    break;
	}

	memcpy(lineBuffer, current, next - current);

	addCard(lineBuffer);

	current = next + 1;
    }

    while(allCardsOffset < NUM_TOTAL_CARDS) {
	allCards[allCardsOffset] = NULL;
	allCardsOffset++;
    }

    createCardSet(basicCards, BASIC);
    createCardSet(libraryCards, LIBRARY);
    createGemSet();
}

void createCardSet(card_t** set, card_type type) {
    int offset = 0;

    for(int i = 0; allCards[i] != NULL; i++) {
	if(allCards[i]->type == type) {
	    set[offset] = allCards[i];
	    offset++;
	}
    }
}

void createGemSet() {
    int offset = 0;

    for(int i = 0; allCards[i] != NULL; i++) {
	if(allCards[i]->gem) {
	    gemCards[offset] = allCards[i];
	    offset++;
	}
    }
}

void addCard(unsigned char* buf) {
    card_t* newCard = malloc(sizeof(card_t));
    memset(newCard, 0, sizeof(card_t));

    //Name
    unsigned char* comma = strstr(buf, ",");
    memcpy(newCard->name, buf, comma - buf);

    buf = comma + 1;
    unsigned char action = *buf;
    if(action) {
	addAction(newCard);
    }

    //Type
    buf += 2;
    comma = strstr(buf, ",");
    addType(newCard, buf, comma);

    buf = comma + 1;

    cardIndexOrdering[0] = &(newCard->quantity);
    cardIndexOrdering[1] = &(newCard->blue);
    cardIndexOrdering[2] = &(newCard->boots);
    cardIndexOrdering[3] = &(newCard->clank);
    cardIndexOrdering[4] = &(newCard->swords);
    cardIndexOrdering[5] = &(newCard->price);
    cardIndexOrdering[6] = &(newCard->points);
    cardIndexOrdering[7] = &(newCard->cards);
    cardIndexOrdering[8] = &(newCard->companion);
    cardIndexOrdering[9] = &(newCard->deep);
    cardIndexOrdering[10] = &(newCard->cave);
    cardIndexOrdering[11] = &(newCard->dragon);
    cardIndexOrdering[12] = &(newCard->coins);
    cardIndexOrdering[13] = &(newCard->gem);
    cardIndexOrdering[14] = &(newCard->arrive);
    cardIndexOrdering[15] = &(newCard->danger);
    cardIndexOrdering[16] = &(newCard->acquireHeart);
    cardIndexOrdering[17] = &(newCard->acquireBoot);
    cardIndexOrdering[18] = &(newCard->acquireSword);
    cardIndexOrdering[19] = &(newCard->acquireClank);

    unsigned char tempBuf[4];
    bzero(tempBuf, 4);
    char offset = 0;
    char values = 0;

    for(;;) {
	if(*buf != ',') {
	    tempBuf[offset] = *buf;
	    offset++;
	} else {
	    *(cardIndexOrdering[values]) = atoi(tempBuf);

	    values++;
	    if(values == 20) {
		break;
	    }

	    bzero(tempBuf, 4);
	    offset = 0;
	}

	buf++;
    }

    allCards[allCardsOffset] = newCard;
    allCardsOffset++;
}

void addType(card_t* card, unsigned char* start, unsigned char* end) {
    unsigned char buffer[8];
    bzero(buffer, 8);

    memcpy(buffer, start, end - start);

    if(!strcmp(buffer, "Basic")) {
	card->type = BASIC;
    } else if(!strcmp(buffer, "Goblin")) {
	card->type = GOBLIN;
    } else if(!strcmp(buffer, "Library")) {
	card->type = LIBRARY;
    } else if(!strcmp(buffer, "Monster")) {
	card->type = MONSTER;
    } else if(!strcmp(buffer, "Regular")) {
	card->type = REGULAR;
    } else {
	printf("error!\n");
	printf("Not a type. Received buffer: %s\n", start);
	exit(1);
    }
}

void addAction(card_t* card) {
    if(!(strcmp(card->name, "Watcher"))) {
	card->action = &specWatcher;
    } else if(!(strcmp(card->name, "Swagger"))) {
	card->action = &specSwagger;
    } else if(!(strcmp(card->name, "Archaeologist"))) {
	card->action = &specArchaeologist;
    } else if(!(strcmp(card->name, "Rebel Soldier"))) {
	card->action = &specRebel;
    } else if(!(strcmp(card->name, "Rebel Miner"))) {
	card->action = &specRebel;
    } else if(!(strcmp(card->name, "Treasure Hunter"))) {
	card->action = &specTreasureHunter;
    } else if(!(strcmp(card->name, "Master Burglar"))) {
	card->action = &specMasterBurglar;
    } else if(!(strcmp(card->name, "Tattle"))) {
	card->action = &specTattle;
    } else if(!(strcmp(card->name, "Rebel Captain"))) {
	card->action = &specRebel;
    } else if(!(strcmp(card->name, "Rebel Scout"))) {
	card->action = &specRebel;
    } else if(!(strcmp(card->name, "Kobold Merchant"))) {
	card->action = &specKoboldMerchant;
    } else if(!(strcmp(card->name, "Search"))) {
	card->action = &specSearch;
    } else if(!(strcmp(card->name, "Invoker of the Ancients"))) {
	card->action = &specInvokeroftheAncients;
    } else if(!(strcmp(card->name, "Gem Collector"))) {
	card->action = &specGemCollector;
    } else if(!(strcmp(card->name, "Wand of Recall"))) {
	card->action = &specWandofRecall;
    } else if(!(strcmp(card->name, "The Mountain King"))) {
	card->action = &specTheMountainKing;
    } else if(!(strcmp(card->name, "The Queen of Hearts"))) {
	card->action = &specTheQueenofHearts;
    } else if(!(strcmp(card->name, "Wand of Wind"))) {
	card->action = &specWandofWind;
    }
}

void printAllCards() {
    for(int i = 0; i < NUM_TOTAL_CARDS; i++) {
	if(allCards[i] == NULL) {
	    break;
	}

	printCard(allCards[i]);
    }
}

void printCard(card_t* card) {
    printf("%s: ", card->name);
    unsigned char* theCard = (unsigned char*)((void*)card);

    for(int i = CARD_NAME_LENGTH + sizeof(card_type) - 1; i < CARD_NAME_LENGTH + sizeof(card_type) + 20; i++) {
	printf("%x ", theCard[i]);
    }
    printf("\n");
}

void cleanUpCards() {
    for(int i = 0; allCards[i] != NULL; i++) {
	free(allCards[i]);
    }
}

