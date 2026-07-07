CC = gcc
CFLAGS = -g

ARQUIVO_TAR = 2024102896_2024102900.tar.gz

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

