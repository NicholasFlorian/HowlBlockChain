
#include <iostream>

#include "blockChain.h"

int main(int argc, char *argv[]) {

    // constants
       // constants
    char* userAPublic = NULL;
    char* userAPrivate = NULL;
    char* userBPublic = NULL;
    char* userBPrivate = NULL;
    howl::BlockChain* userA = NULL;
    howl::BlockChain* userB = NULL;  
    
    howl::BlockChain::loadSSL();
    howl::BlockChain::generateKeyPair(&userAPublic, &userAPrivate);
    howl::BlockChain::generateKeyPair(&userBPublic, &userBPrivate);

    std::cout << userAPublic << std::endl;
    std::cout << userAPrivate << std::endl;
    
    userA = new howl::BlockChain((char *) "af13e92d44b1ac31");
    userB = new howl::BlockChain((char *) "af13e92d44b1ac31");


    //
    std::cout << std::endl << "DISPLAY GENISIS BLOCK:" << std::endl;
    std::cout << userA->getLastSentBlock()->toString() << std::endl;
    
    std::cout << std::endl << "SEND GENISIS TO OTHER:" << std::endl;
    char* temp1 = userA->getEncryptedBlock(userBPublic);
    userB->addReceivedBlock(temp1, userBPrivate);
    std::cout << userB->getLastReceivedBlock()->toString() << std::endl;

    //
    userA->addSentBlock((char *) "message 1");

    std::cout << std::endl << "DISPLAY FIRST MESSAGE:" << std::endl;
    std::cout << userA->getLastSentBlock()->toString() << std::endl;

    std::cout << std::endl << "SEND MESSAGE TO OTHER:" << std::endl;
    char* temp2 = userA->getEncryptedBlock(userBPublic);
    userB->addReceivedBlock(temp2, userBPrivate);
    std::cout << userB->getLastReceivedBlock()->toString() << std::endl;
    
    return 1;
}