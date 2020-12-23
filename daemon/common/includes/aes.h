#ifndef AES_H
#define AES_H
#define KEY_SIZE 256
#define IV_SIZE 128

#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>

typedef struct      s_aes
{
    unsigned char   *_key;
    unsigned char   *_IV;
}                   t_aes;



int decrypt(t_aes *aes, unsigned char *ciphertext, int ciphertext_len, unsigned char **plaintext);
int encrypt(t_aes *aes, unsigned char *plaintext, int plaintext_len, unsigned char **ciphertext);
void handleErrors(void);
void create_keys(t_aes **aes);
unsigned char *random_string(size_t length);


#endif