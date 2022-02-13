#include <openssl/conf.h>
#include <openssl/bn.h>
#include <openssl/engine.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#include <iostream>
#include <string.h>

#include "blockChain.h"

void handleErrors(void){
    
    printf("Nothing Personal Kid.");
    ERR_print_errors_fp(stderr);
    abort();
    // TODO managing freeing memory
}

//______________________________________________________________________________
int HOWL_BIO_to_char(
    BIO*            bp,
    char**          key){
    
    // variables
    int             length;
    
    // allocate key
    length = BIO_pending(bp);
    //*key = (char*) calloc(length + 1, 1);
    *key = (char*) malloc(sizeof(char) * (length + 1));

    if(!(BIO_read(bp, (unsigned char*) *key, length)))
        handleErrors();

    //std::cout << "CONVERTING KEY: " << length << std::endl;
    //std::cout << strlen(*key) << std::endl;
    //std::cout << *key << std::endl << std::endl;

    return 1;
}

int HOWL_rsa_generate(char** publicKey, char** privateKey){

    // variables
    BIGNUM* big_num;    // e
    RSA*    rsa;
    BIO*    bp_public;
    BIO*    bp_private;


    // initialize the big number
    if(!(big_num = BN_new()))
        handleErrors();

    if(!(BN_set_word(big_num, SSE)))
        handleErrors();

    // initialize the rsa structure and generate the key
    if(!(rsa = RSA_new()))
        handleErrors();

    // generate private and public keys 
    if(!(RSA_generate_multi_prime_key(rsa, RSA_BITS, RSA_PRIMES, big_num, NULL)))
        handleErrors();

    // store the public key and private key
    bp_public = BIO_new(BIO_s_mem());
    if(!(PEM_write_bio_RSAPublicKey(bp_public, rsa)))
        handleErrors();

    bp_private = BIO_new(BIO_s_mem());
    if(!(PEM_write_bio_RSAPrivateKey(bp_private, rsa, NULL, NULL, 0, NULL, NULL)))
        handleErrors();

    HOWL_BIO_to_char(bp_public, publicKey);
    HOWL_BIO_to_char(bp_private, privateKey);

    // free
    BN_free(big_num);
    RSA_free(rsa);
    BIO_free(bp_private);
    BIO_free(bp_public);

    return 1;
}

int HOWL_rsa_encrypt(BIO* bp_public, char* plaintext, char** cyphertext){

    // variables
    RSA*            rsa = NULL;

    // create RSA
    if(!(PEM_read_bio_RSAPublicKey(bp_public, &rsa, 0, 0)))
        handleErrors();

    *cyphertext = (char*) malloc(sizeof(char*) * 10000);
    
    // encrypt
    if(!(RSA_public_encrypt(
        strlen(plaintext), 
        (unsigned char*) plaintext,
        (unsigned char*) *cyphertext,
        rsa,
        RSA_PKCS1_PADDING)))
            handleErrors();

    return 1;
}

int HOWL_rsa_decrypt(BIO* bp_private, char* cyphertext, char** plaintext){

    // variables
    RSA*            rsa = NULL;

    // create RSA
    if(!(PEM_read_bio_RSAPrivateKey(bp_private, &rsa, 0, 0)))
        handleErrors();

    *plaintext = (char*) malloc(sizeof(char*) * 10000);

    // decrypt
    if(!(RSA_private_decrypt(
        strlen((char *) cyphertext),
        (unsigned char*) cyphertext,
        (unsigned char*) *plaintext,
        rsa,
        RSA_PKCS1_PADDING)))
            handleErrors();

    return 1;
}

//______________________________________________________________________________

int main(int argc, char *argv[]) {

    // constants
    char* userAPublic = NULL;
    char* userAPrivate = NULL;
    char* userBPublic = NULL;
    char* userBPrivate = NULL;
    howl::BlockChain* userA = NULL;
    howl::BlockChain* userB = NULL;  
    
    OpenSSL_add_all_algorithms();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    // generate pair keys
    //HOWL_rsa_generate(&userAPublic, &userAPrivate);
    HOWL_rsa_generate(&userBPublic, &userBPrivate);

    //std::cout << userBPublic << std::endl;
    //std::cout << userBPrivate << std::endl;

    ////////////////////////////////////////////////////////////////////////////
    userA = new howl::BlockChain((char *) "af13e92d44b1ac31");
    userB = new howl::BlockChain((char *) "af13e92d44b1ac31");

    std::cout << std::endl << "DISPLAY GENISIS BLOCK:" << std::endl;
    std::cout << userA->toString() << std::endl;
    
    std::cout << std::endl << "SEND GENISIS TO OTHER:" << std::endl;
    char* temp1 = userA->getEncryptedBlock(userBPublic);
    userB->addReceivedBlock(temp1, userBPrivate);


    ////////////////////////////////////////////////////////////////////////////
    userA->addSentBlock((char *) "message 1");

    std::cout << std::endl << "DISPLAY FIRST MESSAGE:" << std::endl;
    std::cout << userA->toString() << std::endl;

    std::cout << std::endl << "SEND MESSAGE TO OTHER:" << std::endl;
    char* temp2 = userA->getEncryptedBlock(userBPublic);
    userB->addReceivedBlock(temp2, userBPrivate);


    //khh
    std::cout << std::endl << "Nothing Personal Kid." << std::endl; 
    ERR_print_errors_fp(stderr);

    //free(userAPublic);
    //free(userAPrivate);
    //free(userBPublic);
    //free(userBPrivate);
    //free(userA);
    //free(userB);

    return 1;
}