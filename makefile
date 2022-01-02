# HowlblockChain
 
# *****************************************************
# compile variables
 
CC = g++
CFLAGS = -Wall -g -Ilib

# ****************************************************

main: blockChain.o main.o
	$(CC) $(CFLAGS) -o main main.o blockChain.o block.o -L./lib/ -lcrypto

main.o: src/main.cpp lib/blockChain.h
	$(CC) $(CFLAGS) -c src/main.cpp
 
# ****************************************************

block.o: src/block.cpp lib/block.h
	$(CC) $(CFLAGS) -c src/block.cpp

blockChain.o: src/blockChain.cpp lib/blockChain.h block.o  
	$(CC) $(CFLAGS) -c src/blockChain.cpp

# ****************************************************

clean:
	rm *.o