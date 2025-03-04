#ifndef CONFIG_H_
#define CONFIG_H_

#define CARD_NAME_LENGTH	32

#define NUM_TOTAL_CARDS		70 // last one always NULL
#define NUM_BASIC_CARDS		4
#define NUM_LIBRARY_CARDS	3
#define NUM_GEM_CARDS		5

#define NUM_EXPLORE		15
#define NUM_MERCENARY		15
#define NUM_SECRET_TOMES	12

#define MAX_DECK		128
#define MAX_HAND		32

#define SHUFFLE_POWER		50

#define BOARD_MAX_CONNECTIONS	6
#define BOARD_TOTAL_NODES	39
#define MAX_MOVES		16

#define NUM_DRAGON_LEVELS	7
#define START_DRAGON_LEVEL	2
#define NUM_DRAGON_CUBES	24
#define NUM_PLAYER_CUBES	30

#define STARTING_LIVES		10

#define NUM_PLAYERS		2

#define STARTING_BACKPACKS	2
#define	STARTING_CROWNS		3
#define START_CROWN_VALUE	10
#define STARTING_KEYS		2
#define KEY_VALUE		5
#define BACKPACK_VALUE		5
#define STORE_PRICE		7

#define STARTING_MONKEYS	3
#define MONKEY_VALUE		5

#define NUM_BIG_SECRETS		11
#define NUM_LITTLE_SECRETS	18

#define STORE_SIZE		6
#define EXPLORE_ID		6
#define MERCENARY_ID		7
#define SECRET_TOME_ID		8

#define MAX_AFTER_FUNCS		32

// AI Defines

#define MIN_EXP_BOOT		0.4 //2 boots in a turn
#define VALUE_REGRET		1

// Non-specific Types

typedef enum act_type {
    ACT_ARTIFACT,
    ACT_BS,
    ACT_LS,
    ACT_BUY_KEY,
    ACT_BUY_CROWN,
    ACT_BUY_BACKPACK,
    ACT_MONKEY,
} act_type;

void initRand();


#endif
