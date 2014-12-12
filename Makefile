CFLAGS = -Wall -g

all: nobody setuid

setuid:
	su -c 'chown root nobody && chmod +s nobody'

nobody: nobody.o

nobody.o: nobody.c

clean:
	$(RM) nobody nobody.o
