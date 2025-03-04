#ifndef TURN_H_
#define TURN_H_

#include <stdio.h>

#include "player.h"
#include "board.h"
#include "config.h"

void takeTurn(player_t* player);

void playCards(player_t* player);
void move(player_t* player);
void killMonsters(player_t* player);
void buyCards(player_t* player);

void takeAction(player_t* player, act_type action);
void printAction(act_type action);
void useBigSecret(player_t* player, board_node_t* node);
void useLittleSecret(player_t* player, board_node_t* node);

void resolveAfterFuncs(player_t* player);
void discardResources(player_t* player);

#endif
