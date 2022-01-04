#include "block.h"

#define SHA512_DIGEST_LENGTH 64
#define MAX_MESSAGE 256

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
        //_calculateMerkleRootHash(); 

        _merkleRootHash = (char *) "I'VE SEEN FOOTAGE";

        // calculate the hash
        proofOfWork = false;
        while(!proofOfWork){

            _nonce++;
            _calculateHash();
            proofOfWork = true;
            for(uint32_t i = 0; i < work; i++){

                if(_currentHash[i] != '0'){

                    proofOfWork = false;
                    free(_currentHash);
                }
            }
        }      
    }

    int Block::_calculateHash(){

        // variables
        char* salt;
        int saltLength;
        int messageLength;
        int previousHashLength;
        int merkleRootHashLength;
        int hashIterations;
        int hashLength;
        
        messageLength = strlen(_message);
        previousHashLength = strlen(_previousHash);
        merkleRootHashLength = strlen(_merkleRootHash);

        saltLength = 50 + messageLength +
            previousHashLength + merkleRootHashLength + 1;
        salt = (char*) malloc(sizeof(char) * saltLength);

        // season the hash
        saltLength = sprintf(
            salt, 
            "%d%d%jd%s%s%s",
            _version,
            _nonce,
            _time,
            _message,
            _previousHash,
            _merkleRootHash);

        //std::cout << "HASHING::" << std::endl;
        //std::cout << "SALT::" << std::endl;
        //std::cout << strlen(salt) << " " << i << " :: " << salt << std::endl;

        hashIterations = ceil(saltLength / (SHA512_DIGEST_LENGTH - 1));
        hashLength = (hashIterations * (SHA512_DIGEST_LENGTH - 1)) + 1;
        _currentHash = (char*) malloc(sizeof(char) * (hashLength));

        int i = 0;
        for(int j = 0; j < hashIterations; j++){

            OpenSSL::SHA512_CTX* ctx;
            int k;
            char* iteration;
            char* iterationHash;

            ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
            iteration = (char*) malloc(sizeof(char) * SHA512_DIGEST_LENGTH);
            iterationHash = (char*) malloc(sizeof(char) * SHA512_DIGEST_LENGTH);
            
            for(k = 0; k < SHA512_DIGEST_LENGTH - 1; k++)
                iteration[j] = salt[(j * (SHA512_DIGEST_LENGTH - 1)) + k];
            
            iteration[++k] = '\0';

            OpenSSL::SHA512_Init(ctx);
            OpenSSL::SHA512_Update(ctx, iteration, k);
            OpenSSL::SHA512_Final((unsigned char*) iterationHash, ctx);

            for(j = 0; j < SHA512_DIGEST_LENGTH - 1; j++)
                _currentHash[i++] = iterationHash[j];

            free(iteration);
            free(iterationHash);
            free(ctx);
        }

        _currentHash[i] = '\0';

        free(salt);
        return 1;
    }

    int Block::_calculateMerkleRootHash(){
        
        // variables 
        OpenSSL::SHA512_CTX* ctx;
        Block*  iterator;
        char*   salt;
        int     i;

        // assign memory
        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char*) malloc(sizeof(char) * 64);
        _merkleRootHash = (char*) malloc(sizeof(char) * 64);

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
        res = OpenSSL::SHA512_Final((unsigned char*) _merkleRootHash, ctx);

        _merkleRootHash[i] = '\0';

        //std::cout << "HASH::" << std::endl;
        //std::cout << strlen(hash) << " :: " << (unsigned char*)hash << std::endl;

        // free
        free(ctx);

        return 1;
    }
}