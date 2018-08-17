CC=gcc
CFLAGS=-mavx -std=c99
HELLOOUT=hello_avx.out
UNOPTOUT=unopt_dgmm.out
UNROLLOUT=unroll_dgmm.out

all: unopt_dgmm unroll_dgmm unopt_dgmm.asm unroll_dgmm.asm

unopt_dgmm: unopt_dgmm.o
	$(CC) $(CFLAGS) -O0 -o $(UNOPTOUT) unopt_dgmm.o

unopt_dgmm.o: unopt_dgmm.c
	$(CC) $(CFLAGS) -c unopt_dgmm.c

unopt_dgmm.asm: unopt_dgmm.c
	$(CC) -S $(CFLAGS) -o unopt_dgmm.asm -c unopt_dgmm.c

unroll_dgmm: unroll_dgmm.o
	$(CC) $(CFLAGS) -O3 -o $(UNROLLOUT) unroll_dgmm.o

unroll_dgmm.o: unroll_dgmm.c
	$(CC) $(CFLAGS) -c unroll_dgmm.c

unroll_dgmm.asm: unroll_dgmm.c
	$(CC) -S -O3 $(CFLAGS) -o unroll_dgmm.asm -c unroll_dgmm.c


clean: 
	rm *.o *.out *.asm
