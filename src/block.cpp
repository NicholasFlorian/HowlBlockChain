#include <regex>
#include "block.h"

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
        _timeSent = time(nullptr); // set the current time
    }

    Block::Block(char* plaintextBlock, Block* previousBlock){

        //uint32_t    version;
        //uint32_t    nonce;
        //char*       previousHash;
        //char*       merklerootHash;
        //char*       message;
        //time_t      timeSent;
        //time_t      timeRecieved;

        char* buffer;

        buffer = (char*) malloc(sizeof(char) * 1000);
        _previousHash = (char*) malloc(sizeof(char) * 1000);
        _message = (char*) malloc(sizeof(char) * 1000);

        std::regex regexp("\"\n\t");
        buffer = std::regex_replace(plaintextBlock, regexp, " \"\n\t");

        int val = sscanf(
            buffer,
            "{\n\t\"version\":%d\n\t\"nonce\":%d\n\t\"previousHash\":\"%s\"\n\t\"message\":\"%s\"\n\t\"time\":%ld\n}",
            &_version,
            &_nonce,
            _previousHash,
            _message,
            &_timeSent);
            
        std::cout << "VAL: " << val << std::endl;

        _previousBlock = previousBlock;
        _timeRecieved = time(nullptr);
        _calculateHash();
        _calculateMerklerootHash();
    }

    uint32_t Block::getVersion(){

        return _version;
    }
    
    char* Block::getPreviousHash(){

        return _previousHash;
    }

    char* Block::getHash(){

        return _currentHash;
    }

    char* Block::toString(){
        
        char* buffer;

        buffer = (char*) malloc(sizeof(char) * 1000);

        sprintf(
            buffer, 
            "{\n\t\"version\":%d\n\t\"nonce\":%d\n\t\"previousHash\":\"%s\"\n\t\"currentHash\":\"%s\"\n\t\"merklerootHash\":\"%s\"\n\t\"message\":\"%s\"\n\t\"time\":%ld\n}",
            _version,
            _nonce,
            _previousHash,
            _currentHash,
            _merklerootHash,
            _message,
            _timeSent);

        return buffer;
    }

    char* Block::toJSON(){

        char* buffer;

        buffer = (char*) malloc(sizeof(char) * 513);

        sprintf(
            buffer, 
            "{\n\t\"version\":%d\n\t\"nonce\":%d\n\t\"previousHash\":\"%s\"\n\t\"message\":\"%s\"\n\t\"time\":%ld\n}",
            _version,
            _nonce,
            _previousHash,
            _message,
            _timeSent);

        return buffer;
    }

    void Block::mine(uint32_t work){
    
        bool proofOfWork;

        _calculateMerklerootHash(); 
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
        char*   buffer;
        char*   p; 
        int     saltLength;
        int     messageLength;
        int     previousHashLength;
        int     merklerootHashLength;
        
        messageLength = strlen(_message);
        previousHashLength = strlen(_previousHash);
        merklerootHashLength = strlen(_merklerootHash);
        saltLength = 50 + messageLength +
            previousHashLength + merklerootHashLength + 1;

        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char*) malloc(sizeof(char) * saltLength);
        buffer = (char*) malloc(sizeof(char) * SHA512_DIGEST_LENGTH);
        _currentHash = (char*) malloc(sizeof(char) * (SHA512_HEX_DIGEST_LENGTH + 2));

        saltLength = sprintf(
            salt, 
            "%d%d%jd%s%s%s",
            _version,
            _nonce,
            _timeSent,
            _message,
            _previousHash,
            _merklerootHash);

        OpenSSL::SHA512_Init(ctx);
        OpenSSL::SHA512_Update(ctx, salt, saltLength);
        OpenSSL::SHA512_Final((unsigned char*) buffer, ctx);
        p =  _currentHash;
        for(int i = 0; i < SHA512_DIGEST_LENGTH; i++){

            sprintf(p, "%02x", (unsigned char) buffer[i]);

            if(i != SHA512_DIGEST_LENGTH - 2)
                p += 2;
        }
        _currentHash[SHA512_HEX_DIGEST_LENGTH] = '\0';

        free(buffer);
        free(salt);
        free(ctx);

        return 1;
    }

    int Block::_calculateMerklerootHash(){
        
        OpenSSL::SHA512_CTX* ctx;
        Block*  iterator;
        char*   salt;
        char*   buffer;
        char*   p;
        int     i;

        ctx = (OpenSSL::SHA512_CTX *) malloc(sizeof(OpenSSL::SHA512_CTX));
        salt = (char*) malloc(sizeof(char) * 65536);
        buffer = (char*) malloc(sizeof(char) * SHA512_DIGEST_LENGTH);
        _merklerootHash = (char*) malloc(sizeof(char) * (SHA512_HEX_DIGEST_LENGTH + 2));

        iterator = this;
        salt[0] = 'm';
        i = 1;
        while(true){
    
            int hashLength;

            if(iterator->_version == 0)
                break;
            else
                iterator = iterator->_previousBlock;
                
            hashLength = strlen(iterator->_currentHash);
            for(int j = 0; j < hashLength; j++){
                
                salt[i++] = iterator->_currentHash[j];
                if(i == 65536)
                    break;
            }

            
        } 
        salt[i] = '\0';

        //std::cout << "SALT::" << std::endl;
        //std::cout << strlen(salt) << " " << i << " :: " << salt << std::endl;

        OpenSSL::SHA512_Init(ctx);
        OpenSSL::SHA512_Update(ctx, salt, i);
        OpenSSL::SHA512_Final((unsigned char*) buffer, ctx);

        p =  _merklerootHash;
        for(int i = 0; i < SHA512_DIGEST_LENGTH; i++){

            sprintf(p, "%02x", (unsigned char) buffer[i]);

            if(i != SHA512_DIGEST_LENGTH - 2)
                p += 2;
        }
        _merklerootHash[SHA512_HEX_DIGEST_LENGTH] = '\0';

        free(buffer);
        free(salt);
        free(ctx);

        //std::cout << "HASH::" << std::endl;
        //std::cout << strlen(hash) << " :: " << (unsigned char*)hash << std::endl;m

        return 1;
    }
}