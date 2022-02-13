#include "blockChain.h"

namespace howl {

    BlockChain::BlockChain(char* chatId) {
        
        Block*  genBlock;
        char*   initialPreviousHash;
        char*   initialMessage;

        initialPreviousHash = (char*) malloc(sizeof(char) * 5);
        initialMessage = (char*) malloc(sizeof(char) * 12);
        _chatId = (char*) malloc(sizeof(char*) * (SHA512_HEX_DIGEST_LENGTH + 1));

        _sentLength = 0;
        _receivedLength = 0;
        _work = 3;

        sprintf(initialPreviousHash, "NULL");
        sprintf(initialMessage, "GENSISBLOCK");
        sprintf(_chatId, "%s", chatId);

        genBlock = new Block(
            _sentLength++, 
            NULL, 
            initialPreviousHash, 
            initialMessage);

        (*genBlock).mine(_work);
        _sentHead = genBlock;
        _receivedHead = NULL;
    }

    char* BlockChain::addSentBlock(char* message, char* publicKey){

        OpenSSL::RSA*   rsa = NULL;
        OpenSSL::BIO*   bp;
        Block*          newBlock;
        char*           plaintextBlock;
        char*           buffer;
        char*           encryptedBlock;
        char*           p;

        newBlock = new Block(
            _sentLength++, 
            _sentHead, 
            (*_sentHead).getHash(),
            message);

        (*newBlock).mine(_work);
        _sentHead = newBlock;

        plaintextBlock = _sentHead->toJSON();
        buffer = (char*) malloc(sizeof(char) * RSA_DIGEST_LENGTH);
        encryptedBlock = (char*) malloc(sizeof(char) * (RSA_HEX_DIGEST_LENGTH + 1));

        bp = OpenSSL::BIO_new_mem_buf(publicKey, -1); //strlen(publicKey)
        OpenSSL::PEM_read_bio_RSAPublicKey(bp, &rsa, 0, 0);

        buffer = (char*) malloc(sizeof(char*) * RSA_DIGEST_LENGTH);

        int val = OpenSSL::RSA_public_encrypt(
            strlen(plaintextBlock) + 1,  //TODO maybe + 1
            (unsigned char*) plaintextBlock,
            (unsigned char*) buffer,
            rsa,
            RSA_PKCS1_OAEP_PADDING);

        p =  encryptedBlock;
        for(int i = 0; i < RSA_DIGEST_LENGTH; i++){

            sprintf(p, "%02x", (unsigned char) buffer[i]);
            p += 2;
        }
        encryptedBlock[RSA_HEX_DIGEST_LENGTH] = '\0';

        //std::cout << val << " addSentBlock -> ::" << std::endl;
        //std::cout << "Plaintext Block:" << std::endl;
        //std::cout << "<" << plaintextBlock << ">" << std::endl;
        //std::cout << "Encrypted Block:" << std::endl;
        //std::cout << "<" << encryptedBlock << ">" << std::endl << std::endl;

        OpenSSL::RSA_free(rsa);
        OpenSSL::BIO_free(bp);
        //free(plaintextBlock);
        //free(buffer);

        return buffer;
    }

    void BlockChain::addReceivedBlock(char* encryptedBlock, char* privateKey){

        OpenSSL::RSA*   rsa = NULL;
        OpenSSL::BIO*   bp;
        Block*          newBlock;
        char*           plaintextBlock = NULL;

        bp = OpenSSL::BIO_new_mem_buf(privateKey, -1);
        OpenSSL::PEM_read_bio_RSAPrivateKey(bp, &rsa, 0, 0);

        plaintextBlock = (char*) malloc(sizeof(char*) * (RSA_DIGEST_LENGTH + 1));
        
        OpenSSL::RSA_private_decrypt(
            RSA_DIGEST_LENGTH ,
            (unsigned char*) encryptedBlock,
            (unsigned char*) plaintextBlock,
            rsa,
            RSA_PKCS1_OAEP_PADDING);

        if(_receivedHead == NULL){
        
            newBlock = new Block(plaintextBlock, NULL);
            _receivedHead = newBlock;
        }
        else{

            newBlock = new Block(plaintextBlock, _receivedHead);

            if(newBlock->getVersion() != _receivedLength)
                std::cout << "\tVersion Failed" << std::endl;
                
            if(strcmp(newBlock->getPreviousHash(), _receivedHead->getHash()) == 0)
                std::cout << "\tPrevious Hash Failed" << std::endl;

            _receivedHead = newBlock;
            _receivedLength++;
        }
    
        //std::cout << "addReceivedBlock -> ::" << std::endl;
        //std::cout << "Encrypted Block:" << std::endl;
        //std::cout << "<" << encryptedBlock << ">" << std::endl;
        //std::cout << "Plaintext Block:" << std::endl;
        //std::cout << "<" << plaintextBlock << ">" << std::endl << std::endl;

        OpenSSL::RSA_free(rsa);
        OpenSSL::BIO_free(bp);
        //free(encryptedBlock);
        //free(plaintextBlock);
    }

    char* BlockChain::toString(){

        return _sentHead->toString();
    }

    Block BlockChain::_getLastblock() const {

        return *_sentHead;
    }
}