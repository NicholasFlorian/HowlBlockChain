# HowlblockChain
 
# *****************************************************
# compile variables
 
CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 -Ilib
BIN=bin/
OBJ=obj/
SOURCE=src/

# ****************************************************

main: $(OBJ)blockChain.o $(OBJ)main.o
	$(CC) $(CFLAGS) -o $(BIN)main $(OBJ)main.o $(OBJ)blockChain.o $(OBJ)block.o -L./lib/ -lcrypto

$(OBJ)main.o: src/main.cpp lib/blockChain.h
	$(CC) $(CFLAGS) -c src/main.cpp -o $(OBJ)main.o
 
# ****************************************************

$(OBJ)block.o: src/block.cpp lib/block.h
	$(CC) $(CFLAGS) -c src/block.cpp -o $(OBJ)block.o

$(OBJ)blockChain.o: src/blockChain.cpp lib/blockChain.h $(OBJ)block.o  
	$(CC) $(CFLAGS) -c src/blockChain.cpp -o $(OBJ)blockChain.o

# ****************************************************

clean:
	rm $(OBJ)*.o
	rm $(BIN)main