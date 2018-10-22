CC=gcc

CFLAGS=-pthread -I. -Wall



binaries=mash



all: $(binaries)



clean:

	$(RM) -f $(binaries) *.o


