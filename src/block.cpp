#include "block.h"

int MAX_MESSAGE = 256;

namespace howl {

    Block::Block(
        uint32_t index, 
        Block* previousBlock, 
        char* previousHash, 
        char* message): 

        _version(index),
        _previousBlock(previousBlock),
        _previousHash(previousHash),
        _message(message){
        
        _nonce = 0;
        _time = time(nullptr); // set the current time
    }

    // return the blocks hash 
    char* Block::getHash() {

        return _currentHash;
    }

    char* Block::toString(){

        // variables 
        char* buffer;

        // assign memory
        buffer = (char*) malloc(sizeof(char) * 10000);

        sprintf(
            buffer, 
            "{\n\t\"version\":%d\n\t\"nonce\":%d\n\t\"previousHash\":\"%s\"\n\t\"merkleRootHash\":\"%s\"\n\t\"message\":\"%s\"\n\t\"time\":%ld\n}",
            _version,
            _nonce,
            _previousHash,
            _merkleRootHash,
            _message,
            _time);

        return buffer;
    }

    void Block::mine(uint32_t work) {
    
        // variable
        bool proofOfWork;

        // calculate the merkleroot hash
        _merkleRootHash = _calculateMerkleRootHash(); 

        // calculate the hash
        proofOfWork = false;
        while(!proofOfWork){

            _nonce++;
            _currentHash = _calculateHash();

            proofOfWork = true;
            for(uint32_t i = 0; i < work; i++)
                if(_currentHash[i] != '0')
                    proofOfWork = false;
        }      
    }

    char* Block::_calculateHash(){

        // variables
        OpenSSL::SHA512_CTX* ctx;
        char* salt;
        char* hash;
        int i;
        int messageLength;
        int previousHashLength;
        int merkleRootHashLength;
        
        // assign memory
        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char *) malloc(sizeof(char) * (1024));
        hash = (char *) malloc(sizeof(char) * (1024));

        //std::cout << "HASHING::" << std::endl;

        // season the hash
        i = 0;
        for (int j = 0; j < 4; j++)
            salt[i++] = (_version >> (8*j)) & 0xff;

        for (int j = 0; j < 4; j++)
            salt[i++] = (_time >> (8*j)) & 0xff;

        for (int j = 0; j < 4; j++)
            salt[i++] = (_nonce >> (8*j)) & 0xff;

        messageLength = strlen(_message);
        for (int j = 0; j < messageLength; j++)
            salt[i++] = _message[j];

        previousHashLength = strlen(_previousHash);
        for (int j = 0; j < previousHashLength; j++)
            salt[i++] = _previousHash[j];

        merkleRootHashLength = strlen(_merkleRootHash);
        for (int j = 0; j < merkleRootHashLength; j++)
            salt[i++] = _merkleRootHash[j];

        for (int j = 0; j < i; j++)
            if(salt[j] == '\0')
                salt[j] = '/';

        salt[i] = '\0';
        
        //std::cout << "SALT::" << std::endl;
        //std::cout << strlen(salt) << " " << i << " :: " << salt << std::endl;

        // hash
        OpenSSL::SHA512_Init(ctx);
        OpenSSL::SHA512_Update(ctx, salt, i);
        OpenSSL::SHA512_Final((unsigned char*) hash, ctx);

        hash[i] = '\0';
                
        //std::cout << "HASH::" << std::endl;
        //std::cout << strlen(hash) << " :: " << hash << std::endl << std::endl << std::endl;

        // free
        free(ctx);

        return hash;
    }

    char* Block::_calculateMerkleRootHash(){
        
        // variables 
        OpenSSL::SHA512_CTX* ctx;
        Block*  iterator;
        char*   salt;
        char*   hash;
        int     i;

        // assign memory
        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char*) malloc(sizeof(char) * 64);
        hash = (char*) malloc(sizeof(char) * 64);

        //std::cout << "MERKLEROOT::" << std::endl;

        // iterate
        iterator = this;
        i = 0;
        while(true){
    
            // variables
            bool    flag;
            int     previousHashLength;

            flag = true;

            // concat each previous hash until the genisis Block is reached
            previousHashLength = strlen(iterator->_previousHash);
            for(int j = 0; j < previousHashLength; j++){
                
                salt[i++] = iterator->_previousHash[j];
                if(i == 64){

                    flag = false;
                    break;
                }
            }

            if(iterator->_version == 0)
                break;

            if(!flag)
                break;
            else
                iterator = iterator->_previousBlock;
        } 

        //for (int j = 0; j < i; j++)
        //    hash[i] = hash[i] | 0x3f;

        for(int j = 0; j < i - 1; j++)
            if(salt[j] == '\0' || salt[j] == '\n' || 
                salt[j] == '\t' || salt[j] == '\"')
                    salt[j] = '/';

        salt[i] = '\0';

        //std::cout << "SALT::" << std::endl;
        //std::cout << strlen(salt) << " " << i << " :: " << salt << std::endl;

        // hash
        int res = OpenSSL::SHA512_Init(ctx);
        res = OpenSSL::SHA512_Update(ctx, salt, i);
        res = OpenSSL::SHA512_Final((unsigned char*) hash, ctx);

        hash[i] = '\0';

        //std::cout << "HASH::" << std::endl;
        //std::cout << strlen(hash) << " :: " << (unsigned char*)hash << std::endl;

        // free
        free(ctx);

        return hash;
    }
}