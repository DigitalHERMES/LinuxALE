CFLAGS = -g
CC = gcc
RM = rm -f
CP = cp -r
INSTALL = install -s -m 700
DEFINES = -I. 
LIBS = -L. -lm -lpthread

INSTALL_NAME = LinuxALE

OBJECTS = main.o modem.o golay.o server.o 
SOURCES = main.c modem.c golay.c server.c
HEADERS = modem.h golay.h server.h main.h

all: ale

ale: ${OBJECTS}
	${CC} ${CFLAGS} ${DEFINES} -o ${INSTALL_NAME} ${OBJECTS} ${LIBS}

.c.o: ; ${CC} ${CFLAGS} ${DEFINES} -c $<

.h: ; touch $<

.txt: ; touch $<

# Dependencies
main.c:		modem.h main.h
modem.c:	modem.h
golay.c:	golay.h
server.c:	server.h

clean:
	rm *.o ${INSTALL_NAME}

install: ale
	${INSTALL} ale ${INSTALL_NAME}

