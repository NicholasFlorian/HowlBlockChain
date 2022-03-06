#include <iostream>
#include <string.h>

#include "blockChain.h"

int main(int argc, char *argv[]) {

    // constants
    char* userAPublic = NULL;
    char* userAPrivate = NULL;
    
    howl::BlockChain::loadSSL();
    howl::BlockChain::generateKeyPair(&userAPublic, &userAPrivate);

    std::cout << userAPublic << std::endl;
    std::cout << userAPrivate << std::endl;

    return 1;
}