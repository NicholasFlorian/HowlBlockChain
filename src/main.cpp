
#include <iostream>

#include "blockChain.h"

int main(int argc, char *argv[]) {

    // constants
    // constants
    char* userAId = NULL;
    char* userAPublic = NULL;
    char* userAPrivate = NULL;
    char* userBId = NULL;
    char* userBPublic = NULL;
    char* userBPrivate = NULL;
    char* chatId = NULL;
    howl::BlockChain* userA = NULL;
    howl::BlockChain* userB = NULL;  
    
    howl::BlockChain::loadSSL();

    howl::BlockChain::generateUserId(&userAId, (char*) "E4-57-1F-1B-D7-7D");
    howl::BlockChain::generateUserId(&userBId, (char*) "79-63-9D-37-97-F1");
    howl::BlockChain::generateChatId(&chatId, userAId, userBId);

    howl::BlockChain::generateKeyPair(&userAPublic, &userAPrivate);
    howl::BlockChain::generateKeyPair(&userBPublic, &userBPrivate);

    std::cout << userAPublic << std::endl;
    std::cout << userAPrivate << std::endl;
    
    userA = new howl::BlockChain(chatId);
    userB = new howl::BlockChain(chatId);

    //
    std::cout << std::endl << "DISPLAY GENISIS BLOCK:" << std::endl;
    userA->buildGenisisBlock();

    char* temp1 = userA->getLastSentBlock()->toJSON();
    std::cout << temp1 << std::endl;

    std::cout << std::endl << "SEND GENISIS TO OTHER:" << std::endl;

    char* temp2 = userA->getEncryptedBlock(userBPublic);
    std::cout << temp2 << std::endl;

    userB->addReceivedBlock(temp1, userBPrivate);

    char* temp3 = userB->getLastReceivedBlock()->toString();
    std::cout << temp3 << std::endl;

    //
    userA->buildSentBlock((char *) "message 1");

    std::cout << std::endl << "DISPLAY FIRST MESSAGE:" << std::endl;

    char* temp4 = userA->getLastSentBlock()->toString();
    std::cout << temp4 << std::endl;

    std::cout << std::endl << "SEND MESSAGE TO OTHER:" << std::endl;

    char* temp5 = userA->getEncryptedBlock(userBPublic);
    std::cout << temp5 << std::endl;

    userB->addReceivedBlock(temp2, userBPrivate);
    char* temp6 = userB->getLastReceivedBlock()->toString();
    std::cout << temp6 << std::endl;


    howl::BlockChain::freeBlockChain(userA);
    howl::BlockChain::freeBlockChain(userB);
    free(userAId);
    free(userAPublic);
    free(userAPrivate);
    free(userBId);
    free(userBPublic);
    free(userBPrivate);
    free(chatId);
    free(temp1);
    free(temp2);
    free(temp3);
    free(temp4);
    free(temp5);
    free(temp6);

    return 1;
}