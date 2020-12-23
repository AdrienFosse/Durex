#include "../includes/aes.h"

void create_keys(t_aes **aes)
{
    (*aes)->_key = random_string(KEY_SIZE);
    (*aes)->_IV = random_string(IV_SIZE);
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
}

int encrypt(t_aes *aes, unsigned char *plaintext, int plaintext_len, unsigned char **ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    unsigned char *cipher = *ciphertext;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aes->_key, aes->_IV))
        handleErrors();


    if(1 != EVP_EncryptUpdate(ctx, cipher, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, cipher + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int decrypt(t_aes *aes, unsigned char *ciphertext, int ciphertext_len, unsigned char **plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    unsigned char *plain = *plaintext;


    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aes->_key, aes->_IV))
        handleErrors();

    if(1 != EVP_DecryptUpdate(ctx, plain, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plain + len, &len))
        handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plain[plaintext_len] = '\0';

    return plaintext_len;
}

unsigned char *random_string(size_t length)
{
    const char  charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    unsigned char        *rand_string;
    size_t      i;
    int         key;

    rand_string = (unsigned char *)malloc(sizeof(unsigned char) * length + 1);
    if (rand_string == NULL)
        exit(EXIT_FAILURE);
    for (i = 0; i < length; i++)
    {
        key = rand() % (int)(sizeof(charset) -1);
        rand_string[i] = charset[key];
    }
    rand_string[length] = '\0';

    return rand_string;
}
