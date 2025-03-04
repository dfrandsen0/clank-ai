
### Introduction

This is an unfinished project based on the game *Clank!*. In this game, players sneak into a dragon guarded dungeon to steal treasure. If they make too much noise, the dragon can find them and kill them! The player to escape with the most treasure wins.

I built this project to create an AI to play the game. This game is incredibly complicated, so I simplified it some. A full list of simplifications can be found at the bottom.

You can learn more about the game, including the rule book, [here](https://boardgamegeek.com/boardgame/201808/clank-a-deck-building-adventure).

The AI currently uses value iteration to determine where to move, but it doesn't consider the value of individual spaces (only artifacts). Despite it's simplicity, it can beat humans. 

### Usage

To run the program, do the following:
1. Compile by using 'make' with the given Makefile
2. Run the program: ```./clank```

The program will pause every time 

### Caution

This program is unfinished. It will only sort of work out of the box. You can simulate two AI against each other, and it works with a human player, but you'll have to study human.c and human.h to determine what format to give moves and buys.

You can optionally uncomment ```waitForInput();``` in clank.c to have the simulation wait at every turn. Press any key to continue.

### Next Steps

- Three new versions of the AI to either build on or replace the current version:
    - Implement minimax with alpha-beta pruning, based on expected values of the deck, future cards, and potential kills.
    - Implement MCTS (separately) to simulate game logic and determine most profitable moves
    - Implement some mix between the two, where it simulates higher profitable paths first
- Smoothen out Human interaction, and add prompts for formatting
- Potentially desimplify

### Simplifications

- Archaeologist doesn't act on extra skill if you pick up a monkey during the turn
- Apothecary thrown out
- Sleight of Hand thrown out
- Underworld dealing cannot buy tomes
- Big Secret +3 cards gives 5 coins instead
- Will use swords in path before monsters in the store
- Wand of Wind only teleports to other rooms
- Wand of Recall ignores the teleport the turn you pick up the artifact
- Replaced boots from animated doors with coins
- Monsters killed ignores newly drawn cards (Overlord, Cave Troll)
- Ignore when buying cards, acquire Boot and acquire sword
- Threw out Items
- Threw out Crystal Golem
- Can only buy one item at store, and only when entering
- Only pick up artifacts when entered in
- Threw out trash card little secret

###### Written by Devin Frandsen; December 2024
