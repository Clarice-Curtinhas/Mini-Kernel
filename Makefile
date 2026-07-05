CC = gcc
CFLAGS = -Wall -Wextra -g

ARQUIVO_TAR = mini_kernel.tar.gz

MONO = main_mono.c escalonador.c PCB.c TCB.c fila.c

MULTI = main_multi.c escalonador.c PCB.c TCB.c fila.c

all:
	monoprocessador

extract:
	tar -xzvf $(ARQUIVO_TAR)

monoprocessador: $(MONO)
	$(CC) $(CFLAGS) $(MONO) -o trabSO

multiprocessador: $(MULTI)
	$(CC) $(CFLAGS) $(MULTI) -o trabSO

clean:
	rm -f *.o trabSO

