#ifndef DETAILS_H_
#define DETAILS_H_

#include "player.h"

void specWatcher(player_t* player, char choice);
void specSwagger(player_t* player, char choice);
void specArchaeologist(player_t* player, char choice);
void specRebel(player_t* player, char choice);
void specTreasureHunter(player_t* player, char choice);
void specMasterBurglar(player_t* player, char choice);
void specTattle(player_t* player, char choice);
void specKoboldMerchant(player_t* player, char choice);
void specSearch(player_t* player, char choice);
void specInvokeroftheAncients(player_t* player, char choice);
void specGemCollector(player_t* player, char choice);
void specWandofRecall(player_t* player, char choice);
void specTheMountainKing(player_t* player, char choice);
void specTheQueenofHearts(player_t* player, char choice);
void specWandofWind(player_t* player, char choice);

void afterFuncGemCollector(player_t* player);
void afterFuncMasterBurglar(player_t* player);
void afterFuncSearch(player_t* player);
void afterFuncSwagger(player_t* player);

#endif
