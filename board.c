#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "board.h"
#include "config.h"

board_node_t* boardNodes[BOARD_TOTAL_NODES];
char* boardIndexOrdering[8];

void createBoard() {
    int fd = fileno(fopen("clankboard.csv", "r"));

    int bufferSize = 2048;
    char buf[bufferSize];
    bzero(buf, bufferSize);
    int offset = 0;
    int back;

    for(;;) {
	back = read(fd, buf + offset, bufferSize - offset);
	if(back == 0) {
	    break;
	}

	offset += back;
    }

    char* first;
    char* next;

    if(buf[0] == '#') {
	first = strstr(buf, "\n") + 1;
    } else {
	first = buf;
    }

    char smallBuf[4];
    bzero(smallBuf, 4);
    offset = 0;

    int values;

    for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	next = strstr(first, "\n");

	board_node_t* newNode = malloc(sizeof(board_node_t));
	memset(newNode, 0, sizeof(board_node_t));

	board_edge_t** newEdges = malloc(sizeof(board_edge_t*) * BOARD_MAX_CONNECTIONS);
	newNode->edges = newEdges;

	boardIndexOrdering[0] = &(newNode->index);
	boardIndexOrdering[1] = &(newNode->heart);
	boardIndexOrdering[2] = &(newNode->littleSecret);
	boardIndexOrdering[3] = &(newNode->bigSecret);
	boardIndexOrdering[4] = &(newNode->artifact);
	boardIndexOrdering[5] = &(newNode->monkeys);
	boardIndexOrdering[6] = &(newNode->store);
	boardIndexOrdering[7] = &(newNode->cave);

	bzero(smallBuf, 4);
	values = 0;
	offset = 0;
	for(;;) {
	    if(*first != ',') {
		smallBuf[offset] = *first;
		offset++;
	    } else {
		*(boardIndexOrdering[values]) = atoi(smallBuf);

		values++;
		if(values == 8) {
		    first++;
		    break;
		}

		offset = 0;
		bzero(smallBuf, 4);
	    }

	    first++;
	}

	boardNodes[newNode->index] = newNode;

	newNode->connections = (*first) - 48;
	first += 2;

	for(int j = 0; j < newNode->connections; j++) {
	    board_edge_t* newEdge = malloc(sizeof(board_edge_t));
	    newEdges[j] = newEdge;

	    boardIndexOrdering[0] = &(newEdge->cost);
	    boardIndexOrdering[1] = &(newEdge->enemies);
	    boardIndexOrdering[2] = &(newEdge->locked);
	    boardIndexOrdering[3] = &(newEdge->nextNodeIndex);

	    bzero(smallBuf, 4);
	    offset = 0;
	    values = 0;

	    for(;;) {
		if(*first != ',') {
		    smallBuf[offset] = *first;
		    offset++;
		} else {
		    *(boardIndexOrdering[values]) = atoi(smallBuf);
		    values++;

		    if(values == 4) {
			first++;
			break;
		    }

		    offset = 0;
		    bzero(smallBuf, 4);
		}

		first++;
	    }
	}

	first = next + 1;
    }

    for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	for(int j = 0; j < boardNodes[i]->connections; j++) {
	    boardNodes[i]->edges[j]->nextNode = boardNodes[boardNodes[i]->edges[j]->nextNodeIndex];
	}
    }
}

void printNode(board_node_t* node) {
    printf("%d: ", node->index);

    if(node->heart) {
	printf("heart, ");
    }

    if(node->littleSecret) {
	printf("littleSecret, ");
    }

    if(node->bigSecret) {
	printf("bigSecret, ");
    }

    if(node->artifact) {
	printf("artifact: %d, ", node->artifact);
    }

    if(node->monkeys) {
	printf("monkeys: %d, ", node->monkeys);
    }

    if(node->store) {
	printf("store, ");
    }

    if(node->cave) {
	printf("cave, ");
    }

    printf("connections: %d; ", node->connections);

    for(int i = 0; i < node->connections; i++) {
	printf("%d, ", node->edges[i]->nextNode->index);
    }

    printf("\n");
}

void printAllNodes() {
    for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	printNode(boardNodes[i]);
    }
}

void cleanUpBoard() {
    for(int i = 0; i < BOARD_TOTAL_NODES; i++) {
	for(int j = 0; j < boardNodes[i]->connections; j++) {
	    free(boardNodes[i]->edges[j]);
	}

	free(boardNodes[i]->edges);
	free(boardNodes[i]);
    }
}

