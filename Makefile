FLAGS = -Isrc/include/SDL2 -Lsrc/lib -Wall -std=c99 -lmingw32 -lSDL2main -lSDL2 -lm
SDL2_GFX = SDL2_gfx/SDL2_gfxPrimitives.o SDL2_gfx/SDL2_rotozoom.o

runl: clean strangeAttractors
	./strangeAttractors

run: clean main
	./main

strangeAttractors: strangeAttractors.o
	gcc ${SDL2_GFX} strangeAttractors.o ${FLAGS} -o strangeAttractors

strangeAttractors.o:
	gcc -c strangeAttractors.c ${FLAGS}

clean:
	del *.o *.exe
