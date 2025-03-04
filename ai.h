#ifndef AI_H_
#define AI_H_

#include <stdio.h>

#include "card.h"

int specEvaluateCardAI(player_t* player, card_t* card);

void specFindMoveAI(player_t* player, char* moves);
void initValues(player_t* player, char* values);
char updateValues(char* values, char* tempValues);

int specActInLocationAI(player_t* player);
void specActKillMonstersAI(player_t* player, char* monsters);
void specActBuyCardsAI(player_t* player, char* cards);

//temp functions
void tryToBuyMercenary(char* offset, char* blue, char* cards);
void tryToBuyExplore(char* offset, char* blue, char* cards);
void tryToBuySecretTome(char* offset, char* blue, char* cards);

#endif
