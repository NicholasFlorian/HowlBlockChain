#include <sstream>
#include <iostream>
namespace OpenSSL {
    #include <openssl/conf.h>
    #include <openssl/bn.h>
    #include <openssl/engine.h>
    #include <openssl/rsa.h>
    #include <openssl/evp.h>
    #include <openssl/sha.h>
    #include <openssl/err.h>
};

namespace howl{

    class Block {
        
    public:

        // constructor
        Block(
            uint32_t    nIndexIn, 
            Block*      previousblock, 
            char*       previousHash, 
            char*       message);

        // variables
        char*       sPrevHash;


        // accessors
        char*       getHash();
        char*       toString();

        void        mine(uint32_t  work);
        
    private:

        // variables
        uint32_t    _version;
        uint32_t    _nonce;
        Block*      _previousBlock;
        char*       _previousHash;
        char*       _currentHash;
        char*       _merkleRootHash;
        char*       _message;
        time_t      _time;
        
        // functions
        
        char*       _calculateMerkleRootHash();
        char*       _calculateHash();
    };
}