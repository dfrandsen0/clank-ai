#ifndef HUMAN_H_
#define HUMAN_H_

#include <stdio.h>

#include "card.h"

int specEvaluateCardHuman(player_t* player, card_t* card);
void specFindMoveHuman(player_t* player, char* moves);
int specActInLocationHuman(player_t* player);
void specActKillMonstersHuman(player_t* player, char* monsters);
void specActBuyCardsHuman(player_t* player, char* cards);

void fillArray(char* container);

#endif
