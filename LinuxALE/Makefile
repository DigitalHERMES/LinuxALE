CFLAGS = -g
CC = gcc
RM = rm -f
CP = cp -r
INSTALL = install -s -m 700
DEFINES = -I. 
LIBS = -L. -lm

INSTALL_NAME = LinuxALE

OBJECTS = main.o modem.o golay.o 
SOURCES = main.c modem.c golay.c
HEADERS = modem.h golay.h

all: ale

ale: ${OBJECTS}
	${CC} ${CFLAGS} ${DEFINES} -o ${INSTALL_NAME} ${OBJECTS} ${LIBS}

.c.o: ; ${CC} ${CFLAGS} ${DEFINES} -c $<

.h: ; touch $<

.txt: ; touch $<

# Dependencies
main.c:		modem.h
modem.c:	modem.h
golay.c:	golay.h

clean:
	rm *.o ${INSTALL_NAME}

install: ale
	${INSTALL} ale ${INSTALL_NAME}

