CC = gcc
CFLAGS = -O2 -Wall

ifeq ($(OS), Windows_NT)
	EXECUTABLE = rando.exe
else
	EXECUTABLE = rando
endif

OBJECTS = main.o menu.o ips.o randomizer.o randpatcher.o spoilertranslator.o

$(EXECUTABLE) : $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS)

main.o : addresses.h options.h randpatcher.h

menu.o : menu.h

ips.o : ips.h

randomizer.o : addresses.h options.h randomizer.h

randpatcher.o : ips.h options.h randpatcher.h randomizer.h

spoilertranslator.o : spoilertranslator.h

.PHONY: clean
clean :
ifeq ($(OS), Windows_NT)
	del $(EXECUTABLE) $(OBJECTS)
else
	rm $(EXECUTABLE) $(OBJECTS)
endif
