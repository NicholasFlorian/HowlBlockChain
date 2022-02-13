#include <sstream>
#include <iostream>

#include "block.h"

#define SSE                         RSA_F4
#define RSA_BITS                    4096
#define RSA_DIGEST_LENGTH           512
#define RSA_HEX_DIGEST_LENGTH       1024
#define RSA_PRIMES                  2

namespace howl {

    class BlockChain {

    public:

        BlockChain(char* chatId);
        
        void        addSentBlock(char* message);
        void        addReceivedBlock(char* encryptedBlock, char* privateKey);
        void        addPrevSentBlock(char* encryptedBlock);
        void        addPrevReceivedBlock(char* encryptedBlock);
        char*       toString();
        char*       getEncryptedBlock(char* publicKey);

    private:

        char*       _chatId;
        uint32_t    _work;
        uint32_t    _sentLength;
        uint32_t    _receivedLength;
        Block*      _sentHead;
        Block*      _receivedHead;

        Block       _getLastblock() const;
    };
}