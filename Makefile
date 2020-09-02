
ifeq ($(OS),Windows_NT)
file_format = .exe
PATH += :/usr/$(arch)-w64-mingw32/sys-root/mingw/bin/
endif


all: game

PDCursesMod:
	git clone --depth=1 https://github.com/Bill-Gray/PDCursesMod

pdcurses: PDCursesMod
	for command in $(shell ls /bin/$(prefix)*) ; do \
		if [ ! -e /bin/$${command#*$$prefix} ] ; then ln -s $$command /bin/$${command#*$$prefix} ; fi \
	done
	make -C PDCursesMod/sdl1 WIDE=Y CC=gcc

game: pdcurses
	if [ ! -d out ]; then mkdir out ; fi
	$(prefix)gcc 2048.c PDCursesMod/sdl1/pdcurses.a -lSDL_ttf -lSDL -O2 -o out/game2048$(file_format)
ifeq ($(OS),Windows_NT)
	cp /usr/$(arch)-w64-mingw32/sys-root/mingw/bin/*.dll utils/game.cmd out/
else
	cp utils/game out/
endif
	cp -r utils/assets out/

clean:
	make clean -C PDCursesMod/sdl1
	rm -rf out/