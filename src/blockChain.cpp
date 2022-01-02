#include "blockChain.h"

namespace howl {

    BlockChain::BlockChain() {
        
        // variables
        Block* genBlock;

        // memory
        genBlock = (Block*) malloc(sizeof(Block));

        // assign
        _length = 0;
        _work = 2;

        genBlock = new Block(
            _length++, 
            NULL, 
            (char *) "NULL", 
            (char *) "GENESISBLOCK");
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