#include "blockChain.h"

namespace howl {

    BlockChain::BlockChain(char* chatId) {
        
        Block*  genBlock;
        char*   initialMerkleRootHash;
        char*   initialMessage;
        int     chatIdLength;

        genBlock = (Block*) malloc(sizeof(Block));
        initialMerkleRootHash = (char*) malloc(sizeof(char) * 5);
        initialMessage = (char*) malloc(sizeof(char) * 12);
        chatIdLength = strlen(chatId);
        _chatId = (char*) malloc(sizeof(char*) * (chatIdLength + 1));

        _toLength = 0;
        _work = 3;

        sprintf(initialMerkleRootHash, "NULL");
        sprintf(initialMessage, "GENSISBLOCK");
        sprintf(_chatId, "CHAT ID");
        //for(int i = 0; i < chatIdLength; i++)
        //    _chatId[i] = chatId[i];
        //chatId[chatIdLength] = '\0';

        genBlock = new Block(
            _toLength++, 
            NULL, 
            initialMerkleRootHash, 
            initialMessage);

        (*genBlock).mine(_work);
        _toHead = genBlock;
    }

    char* BlockChain::addToBlock(char* message, char* publicKey){

        OpenSSL::RSA*   rsa = NULL;
        OpenSSL::BIO*   bp;
        Block*          newBlock;
        char*           plaintextBlock;
        char*           encryptedBlock = NULL;

        newBlock = (Block*) malloc(sizeof(Block));
        
        newBlock = new Block(_toLength++, _toHead, (*_toHead).getHash(), message);
        (*newBlock).mine(_work);
        _toHead = newBlock;

        plaintextBlock = _toHead->toString();

        
        bp = OpenSSL::BIO_new_mem_buf(publicKey, -1); //strlen(publicKey)
        OpenSSL::PEM_read_bio_RSAPublicKey(bp, &rsa, 0, 0);

        encryptedBlock = (char*) malloc(sizeof(char*) * 513);

        int val = OpenSSL::RSA_public_encrypt(
            strlen(plaintextBlock) + 1,  //TODO maybe + 1
            (unsigned char*) plaintextBlock,
            (unsigned char*) encryptedBlock,
            rsa,
            RSA_PKCS1_OAEP_PADDING);

        std::cout << val << " addToBlock -> ::" << std::endl;
        std::cout << "Plaintext Block:" << std::endl;
        std::cout << "<" << plaintextBlock << ">" << std::endl;
        std::cout << "Encrypted Block:" << std::endl;
        std::cout << "<" << encryptedBlock << ">" << std::endl << std::endl;

        //OpenSSL::RSA_free(rsa);
        //OpenSSL::BIO_free(bp);

        return encryptedBlock;
    }

    void BlockChain::addFromBlock(char* encryptedBlock, char* privateKey){

        OpenSSL::RSA*   rsa = NULL;
        OpenSSL::BIO*   bp;
        Block*          newBlock;
        char*           plaintextBlock = NULL;

        bp = OpenSSL::BIO_new_mem_buf(privateKey, -1);
        OpenSSL::PEM_read_bio_RSAPrivateKey(bp, &rsa, 0, 0);

        plaintextBlock = (char*) malloc(sizeof(char*) * 513);
        
        std::cout << strlen(encryptedBlock) + 1 << std::endl;
        OpenSSL::RSA_private_decrypt(
            512,
            (unsigned char*) encryptedBlock,
            (unsigned char*) plaintextBlock,
            rsa,
            RSA_PKCS1_OAEP_PADDING);

        std::cout << "addFromBlock -> ::" << std::endl;
        std::cout << "Encrypted Block:" << std::endl;
        std::cout << "<" << encryptedBlock << ">" << std::endl;
        std::cout << "Plaintext Block:" << std::endl;
        std::cout << "<" << plaintextBlock << ">" << std::endl << std::endl;

        //OpenSSL::RSA_free(rsa);
        //OpenSSL::BIO_free(bp);
    }

    char* BlockChain::toString(){

        return _toHead->toString();
    }

    Block BlockChain::_getLastblock() const {

        return *_toHead;
    }
}