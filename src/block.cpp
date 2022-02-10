#include "block.h"

#define SHA512_DIGEST_LENGTH 64
#define SHA512_HEX_DIGEST_LENGTH 126
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

    char* Block::getHash(){

        return _currentHash;
    }

    char* Block::toString(){

        char* buffer;

        buffer = (char*) malloc(sizeof(char) * 513);

        sprintf(
            buffer, 
            "{\n\t\"version\":%d\n\t\"nonce\":%d\n\t\"previousHash\":\"%s\"\n\t\"merkleRootHash\":\"%s\"\n\t\"message\":\"%s\"\n\t\"time\":%ld\n}",
            //"%d\n%d\n%s\n%s\n%ld",
            _version,
            _nonce,
            _previousHash,
            _merkleRootHash,
            _message,
            _time);

        return buffer;
    }

    void Block::mine(uint32_t work){
    
        bool proofOfWork;

        _calculateMerkleRootHash(); 
        proofOfWork = false;
        while(!proofOfWork){

            _nonce++;
            _calculateHash();
            proofOfWork = true;
            for(uint32_t i = 0; i < work; i++){

                if(_currentHash[i] != '0'){

                    proofOfWork = false;
                    //free(_currentHash);
                }
            }
        }      
    }

    int Block::_calculateHash(){

        OpenSSL::SHA512_CTX* ctx;
        char*   salt;
        char*   bitHash;
        char*   p; 
        int     saltLength;
        int     messageLength;
        int     previousHashLength;
        int     merkleRootHashLength;
        
        messageLength = strlen(_message);
        previousHashLength = strlen(_previousHash);
        merkleRootHashLength = strlen(_merkleRootHash);
        saltLength = 50 + messageLength +
            previousHashLength + merkleRootHashLength + 1;

        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char*) malloc(sizeof(char) * saltLength);
        bitHash = (char*) malloc(sizeof(char) * (SHA512_DIGEST_LENGTH + 1));
        _currentHash = (char*) malloc(sizeof(char) * ((SHA512_DIGEST_LENGTH * 2) + 1));

        saltLength = sprintf(
            salt, 
            "%d%d%jd%s%s%s",
            _version,
            _nonce,
            _time,
            _message,
            _previousHash,
            _merkleRootHash);

        OpenSSL::SHA512_Init(ctx);
        OpenSSL::SHA512_Update(ctx, salt, saltLength);
        OpenSSL::SHA512_Final((unsigned char*) bitHash, ctx);
        bitHash[SHA512_DIGEST_LENGTH] = '\0';

        p =  _currentHash;
        for(int i = 0; i < SHA512_DIGEST_LENGTH; i++){

            sprintf(p, "%02x", (unsigned char) bitHash[i]);
            p += 2;
        }
        _currentHash[SHA512_DIGEST_LENGTH * 2] = '\0';

        free(bitHash);
        free(salt);
        free(ctx);

        return 1;
    }

    int Block::_calculateMerkleRootHash(){
        
        OpenSSL::SHA512_CTX* ctx;
        Block*  iterator;
        char*   salt;
        char*   bitHash;
        char*   p;
        int     i;

        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char*) malloc(sizeof(char) * 65536);
        bitHash = (char*) malloc(sizeof(char) * (SHA512_DIGEST_LENGTH + 1));
        _merkleRootHash = (char*) malloc(sizeof(char) * ((SHA512_DIGEST_LENGTH * 2) + 1));

        iterator = this;
        i = 0;
        while(true){
    
            bool    flag;
            int     previousHashLength;

            flag = true;
            previousHashLength = strlen(iterator->_previousHash);
            for(int j = 0; j < previousHashLength; j++){
                
                salt[i++] = iterator->_previousHash[j];
                if(i == 65536){

                    flag = false;
                    break;
                }
            }

            if(!flag || iterator->_version == 0)
                break;
            else
                iterator = iterator->_previousBlock;
        } 
        salt[i] = '\0';

        //std::cout << "SALT::" << std::endl;
        //std::cout << strlen(salt) << " " << i << " :: " << salt << std::endl;

        OpenSSL::SHA512_Init(ctx);
        OpenSSL::SHA512_Update(ctx, salt, i);
        OpenSSL::SHA512_Final((unsigned char*) bitHash, ctx);
        _merkleRootHash[SHA512_DIGEST_LENGTH] = '\0';

        p =  _merkleRootHash;
        for(int i = 0; i < SHA512_DIGEST_LENGTH; i++){

            sprintf(p, "%02x", (unsigned char) bitHash[i]);
            p += 2;
        }
        _merkleRootHash[SHA512_DIGEST_LENGTH * 2] = '\0';

        free(bitHash);
        free(salt);
        free(ctx);

        //std::cout << "HASH::" << std::endl;
        //std::cout << strlen(hash) << " :: " << (unsigned char*)hash << std::endl;m

        return 1;
    }
}