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

// const 
const unsigned long _SSE =       RSA_F4;
const int           _RSA_BITS =  2048;
const int           _PRIMES =    2;

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
    *key = (char*) calloc(length + 1, 1);

    if(!(BIO_read(bp, (unsigned char*) *key, length)))
        handleErrors();

    return 1;
}

int HOWL_char_to_BIO(char* key, BIO** bp){

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

    if(!(BN_set_word(big_num, _SSE)))
        handleErrors();

    // initialize the rsa structure and generate the key
    if(!(rsa = RSA_new()))
        handleErrors();

    // generate private and public keys 
    if(!(RSA_generate_multi_prime_key(rsa, _RSA_BITS, _PRIMES, big_num, NULL)))
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

int HOWL_sha(char* plaintext, char** hash) {

    // variables
    SHA512_CTX* ctx;
    int         plaintext_length;

    // assign memory
    ctx = (SHA512_CTX *) malloc(sizeof(SHA512_CTX));
    *hash = (char*) malloc(sizeof(char) * 512);

    // copy the plaintext
    plaintext_length = strlen(plaintext);
    for(int i = 0; i < plaintext_length; i++)
        (*hash)[i] = plaintext[i];
    (*hash)[plaintext_length] = '\0';

    // hash
    SHA512_Init(ctx);
    SHA512_Update(ctx, *hash, plaintext_length);
    SHA512_Final((unsigned char*) *hash, ctx);
   
    // free
    free(ctx);

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
    
    // generate pair keys
    HOWL_rsa_generate(&userAPublic, &userAPrivate);
    HOWL_rsa_generate(&userBPublic, &userBPrivate);

    // generate the block chains for each user
    userA = new howl::BlockChain((char *) "af13e92d44b1ac31");
    userB = new howl::BlockChain((char *) "af13e92d44b1ac31");

    std::cout << userA->toString() << std::endl;
    char* temp = userA->addToBlock((char *) "message 1", userBPublic);
    std::cout << userA->toString() << std::endl;


    std::cout << temp << std::endl;

    userB->addFromBlock(temp, userBPrivate);

    free(userAPublic);
    free(userAPrivate);
    free(userBPublic);
    free(userBPrivate);
    free(userA);
    free(userB);


    return 1;
}