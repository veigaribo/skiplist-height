CC=gcc

debug: demo.c random_level.h rng.c rng.h
	$(CC) -mlzcnt -g3 -o $@ demo.c rng.c

release: demo.c random_level.h rng.c rng.h
	$(CC) -mlzcnt -flto -O3 -o $@ demo.c rng.c
