CC = gcc
CFLAGS = -O2 -Wall

ifeq ($(OS), Windows_NT)
	EXECUTABLE = rando.exe
else
	EXECUTABLE = rando
endif

OBJECTS = main.o menu.o ips.o randomizer.o randpatcher.o spoilertranslator.o countdown.o subweapon.o entities.o graphicsfix.o

$(EXECUTABLE) : $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS)

main.o : addresses.h options.h randpatcher.h

menu.o : menu.h

ips.o : ips.h

randomizer.o : addresses.h options.h randomizer.h spoilertranslator.h

randpatcher.o : ips.h options.h randpatcher.h randomizer.h countdown.h subweapon.h graphicsfix.h

spoilertranslator.o : spoilertranslator.h

countdown.o : countdown.h game_structure.h

subweapon.o : subweapon.h game_structure.h

entities.o : entities.h game_structure.h

graphicsfix.o : graphicsfix.h entities.h

.PHONY: clean
clean :
ifeq ($(OS), Windows_NT)
	del $(EXECUTABLE) $(OBJECTS)
else
	rm $(EXECUTABLE) $(OBJECTS)
endif
