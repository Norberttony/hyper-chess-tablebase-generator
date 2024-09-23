all:
	gcc -oFast src/bitboard-utility.c src/defines.c src/look-up-tables.c src/magic-bitboards.c src/move.c src/make-unmake.c src/perft.c src/godel.c src/transform.c src/unmove.c src/tablebase.c src/main.c src/perms.c src/vector.c -o bin/main
