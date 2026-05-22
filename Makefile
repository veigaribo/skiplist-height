CC=gcc

debug: demo.c random_level.h rng.c rng.h
	$(CC) -mlzcnt -g3 -o $@ $^

release: demo.c random_level.h rng.c rng.h
	$(CC) -mlzcnt -flto -O3 -o $@ $^
