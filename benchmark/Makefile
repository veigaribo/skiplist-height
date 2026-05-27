CC=g++

bench: benchmark.cpp ../random_level.h ../rng.c ../rng.h
	$(CC) -mlzcnt -O3 -flto -lbenchmark -I.. -o $@ ../rng.c benchmark.cpp
