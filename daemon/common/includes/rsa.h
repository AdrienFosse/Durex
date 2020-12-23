#ifndef RSA_H
#define RSA_H

#include <openssl/evp.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <string.h>

typedef struct  s_rsa
{
    EVP_PKEY    *_own_pkey;
    char        *_own_pub_key;
    char        *_own_priv_key;
}               t_rsa;

EVP_PKEY        *GeneratePKEY(unsigned char *clear_key, size_t len);
int             EncryptMessage(const char *msg_in, unsigned char **msg_out, EVP_PKEY *pkey);
unsigned char   *DecryptMessage(char *msg_in, int inlen, EVP_PKEY *pkey);
EVP_PKEY        *GetOwnPKEY();
char            *GetOwnPubKey();
void            SendMessageToServ();
void            GenerateKeyPair(t_rsa **rsa);

#endif