#ifndef BOARD_H_
#define BOARD_H_

#include "config.h"

struct board_edge_t;

typedef struct board_node_t {
    char index;
    char heart;
    char littleSecret;
    char bigSecret;
    char artifact;
    char monkeys;
    char store;
    char cave;
    char connections;
    struct board_edge_t** edges;
} board_node_t;

typedef struct board_edge_t {
    char cost;
    char enemies;
    char locked;
    char nextNodeIndex;
    struct board_node_t* nextNode;
} board_edge_t;

void createBoard();
void printNode();
void printAllNodes();
void cleanUpBoard();

extern board_node_t* boardNodes[BOARD_TOTAL_NODES];

#endif
