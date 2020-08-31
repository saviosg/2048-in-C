

all: game

PDCursesMod:
	git clone --depth=1 https://github.com/Bill-Gray/PDCursesMod

pdcurses: PDCursesMod
	WIDE=Y PDC_WIDE=TRUE make -C PDCursesMod/sdl1

game: pdcurses
	gcc 2048.c -lSDL_ttf -lSDL PDCursesMod/sdl1/pdcurses.a -O2 -o game2048.o

clean:
	make clean -C PDCursesMod/sdl1
	rm -rf game2048.o
