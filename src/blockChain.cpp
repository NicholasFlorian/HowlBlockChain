#include "blockChain.h"

namespace howl {

    BlockChain::BlockChain() {
        
        // variables
        Block* genBlock;
        char* initialMerkleRootHash;
        char* initialMessage;

        // memory
        genBlock = (Block*) malloc(sizeof(Block));
        initialMerkleRootHash = (char*) malloc(sizeof(char) * 5);
        initialMessage = (char*) malloc(sizeof(char) * 12);

        // assign
        _length = 0;
        _work = 2;

        sprintf(initialMerkleRootHash, "NULL");
        sprintf(initialMessage, "GENSISBLOCK");

        genBlock = new Block(
            _length++, 
            NULL, 
            initialMerkleRootHash, 
            initialMessage);

        (*genBlock).mine(_work);
        _head = genBlock;
    }

    void BlockChain::addblock(char* message) {

        // variables
        Block* newBlock;

        // memory
        newBlock = (Block*) malloc(sizeof(Block));

        // assign
        newBlock = new Block(_length++, _head, (*_head).getHash(), message);
        (*newBlock).mine(_work);
        _head = newBlock;
    }

    char* BlockChain::toString(){

        return (*_head).toString();
    }

    Block BlockChain::_getLastblock() const {

        return *_head;
    }
}