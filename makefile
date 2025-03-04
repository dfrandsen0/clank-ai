all: clank

clank: clank.c
	gcc -o clank clank.c card.c details.c config.c player.c game.c board.c turn.c ai.c human.c

clean:
	rm -f clank
