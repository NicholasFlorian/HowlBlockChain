#include <sstream>
#include <cstring>
#include <iostream>
#include <math.h>

namespace OpenSSL {
    #include <openssl/conf.h>
    #include <openssl/bn.h>
    #include <openssl/engine.h>
    #include <openssl/rsa.h>
    #include <openssl/evp.h>
    #include <openssl/sha.h>
    #include <openssl/err.h>
};

#define SHA512_DIGEST_LENGTH        64
#define SHA512_HEX_DIGEST_LENGTH    126
#define MAX_MESSAGE                 256

namespace howl{

    class Block {
        
    public:

        Block(
            uint32_t    nIndexIn, 
            Block*      previousblock, 
            char*       previousHash, 
            char*       message);

        Block(char* plaintextBlock, Block* previousBlock);

        uint32_t    getVersion();
        char*       getPreviousHash();
        char*       getHash();
        char*       toString();
        void        mine(uint32_t  work);

    private:

        int         _calculateMerklerootHash();
        int         _calculateHash();

        uint32_t    _version;
        uint32_t    _nonce;
        Block*      _previousBlock;
        char*       _previousHash;
        char*       _currentHash;
        char*       _merklerootHash;
        char*       _message;
        time_t      _timeSent;
        time_t      _timeRecieved;

    };
}