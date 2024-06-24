# Définition des cibles par défaut
all: PC

RPI: libMcsRPI serveurRPI clientRPI
PC: libMcsPC serveurPC clientPC

# Compilation des fichiers source
serveurPC: source/serveur.c
	gcc -ggdb -o serveur.e $^ -lpthread -lmcs -lncurses -Llib -LoutncursesPC/lib -IoutncursesPC/include/ncurses

# Compilation des fichiers source
clientPC: source/client.c
	gcc -o client.e $^ -Wall -Wextra -pedantic -static -lpthread -lmcs -lncurses -Llib -LoutncursesPC/lib -IoutncursesPC/include/ncurses

# Compilation des fichiers source de LIBRAIRIE
libDataPC: lib/data.c lib/data.h
	gcc -Wall -Wextra -pedantic -c -o lib/data.o $<

libSessionPC: lib/session.c lib/session.h
	gcc -c -o lib/session.o $<

# Création de la bibliothèque statique
libMcsPC: libSessionPC libDataPC
	ar rcs lib/libmcs.a lib/data.o lib/session.o

# Compilation des fichiers source
serveurRPI: source/serveur.c
	$(PATH_CC)/arm-linux-gnueabihf-gcc -o serveurRPI.e $^  -std=c99 -D_SVID_SOURCE -lpthread -lmcs -lncurses -Llib -LoutncursesRPI/lib/ -Iinclude/ncurses

# Compilation des fichiers source
clientRPI: source/client.c
	export RPI="";
	$(PATH_CC)/arm-linux-gnueabihf-gcc -o clientRPI.e $^ -static -std=c99 -D_SVID_SOURCE -lpthread -lwiringpi -lmcs -lncurses -Llib -LoutncursesRPI/lib/ -IwiringPi/wiringPi/ -Iinclude/ncurses

libDataRPI: lib/data.c lib/data.h
	$(PATH_CC)/arm-linux-gnueabihf-gcc -c -o lib/data.o $<

libSessionRPI: lib/session.c lib/session.h
	$(PATH_CC)/arm-linux-gnueabihf-gcc -ggdb -c -o lib/session.o $<

# Création de la bibliothèque statique
libMcsRPI: libSessionRPI libDataRPI
	$(PATH_CC)/arm-linux-gnueabihf-ar rcs lib/libmcs.a lib/data.o lib/session.o

# Nettoyage complet
clean: cleanLib cleanExec cleanObj

# Nettoyage des fichiers e
cleanExec:
		rm -f *.e

# Nettoyage des fichiers objets
cleanObj:
		rm -f lib/*.o

# Nettoyage de la lib
cleanLib:
		rm -f lib/libmcs.a
