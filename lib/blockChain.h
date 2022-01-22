#include <sstream>
#include <iostream>

#include "block.h"

namespace howl {

    class BlockChain {

    public:

        BlockChain(char* chatId);
        
        char*       addToBlock(char* message, char* publicKey);
        void        addFromBlock(char* encryptedBlock, char* privateKey);
        char*       toString();

    private:

        uint32_t    _work;
        uint32_t    _toLength;
        uint32_t    _fromLength;
        char*       _chatId;
        Block*      _toHead;
        Block*      _fromHead;

        Block       _getLastblock() const;
    };
}