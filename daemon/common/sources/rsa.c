#include "../includes/rsa.h"

EVP_PKEY            *GeneratePKEY(unsigned char *clear_key, size_t len)
{
    BIO *b;
    EVP_PKEY *side_pkey;

    side_pkey = EVP_PKEY_new();
    b = BIO_new(BIO_s_mem());
    BIO_write(b, clear_key, len); //????
    side_pkey = PEM_read_bio_PUBKEY(b, &side_pkey, NULL, NULL);
    if (side_pkey == NULL){
        printf("ERROR GET PKEY FROM STR\n");
        exit(EXIT_FAILURE);
    }
    return side_pkey;
}

void                GenerateKeyPair(t_rsa **rsa)
{
    EVP_PKEY_CTX    *ctx;
    BIO             *pub_bio;
    BIO             *priv_bio;

    int             res;

    (*rsa)->_own_pkey = NULL;
    
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx)
    {
        printf("Failed to create CTX \n");
        exit(EXIT_FAILURE);
    }
    res = EVP_PKEY_keygen_init(ctx);
    if (res <= 0)
    {
        printf("Failed to init Keygen \n");
        exit(EXIT_FAILURE);
    }
    res = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 4096);
    if (res <= 0)
    {
        printf("Failed to set Keygen bits \n");
        exit(EXIT_FAILURE);
    }
    (*rsa)->_own_pkey = EVP_PKEY_new();

    res = EVP_PKEY_keygen(ctx, &((*rsa)->_own_pkey));
    if (res <= 0)
    {
        printf("Failed to generate Keys \n");
        exit(EXIT_FAILURE);
    }

    priv_bio = BIO_new(BIO_s_mem());
    res = PEM_write_bio_PrivateKey(priv_bio, (*rsa)->_own_pkey, NULL, NULL, 0, NULL, NULL);
    if (res != 1)
    {
        printf("Unable to write private key to memory \n");
        exit(EXIT_FAILURE);
    }

    BIO_flush(priv_bio);
    BIO_get_mem_data(priv_bio, &((*rsa)->_own_priv_key));

    pub_bio = BIO_new(BIO_s_mem());    
    res = PEM_write_bio_PUBKEY(pub_bio, (*rsa)->_own_pkey);
    if (res != 1)
    {
        printf("Unable to write public key to menory \n");
        exit(EXIT_FAILURE);
    }
    BIO_flush(pub_bio);
    BIO_get_mem_data(pub_bio, &((*rsa)->_own_pub_key));
}

int        EncryptMessage(const char *msg_in, unsigned char **msg_out, EVP_PKEY *pkey)
{
    EVP_PKEY_CTX    *curr_ctx;
    size_t          outlen = 0;
    size_t          inlen = strlen(msg_in);

    // printf("Message to encrypt : %s", msg_in);
    curr_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (EVP_PKEY_encrypt_init(curr_ctx) <= 0)
    {
        printf("Unable to init encrypt \n");
    }
    if (EVP_PKEY_CTX_set_rsa_padding(curr_ctx, RSA_PKCS1_PADDING) <= 0)
    {
        printf("Unable to set padding \n");
    }
    if (EVP_PKEY_encrypt(curr_ctx, NULL, &outlen, (const unsigned char*)msg_in, inlen) <= 0)
    {
        printf("Unable to get encrypt len \n");
    }
    *msg_out = (unsigned char *)OPENSSL_malloc(outlen);
    if (!*msg_out)
    {
        printf("Unable to malloc encrypted msg \n");
    }
    if (EVP_PKEY_encrypt(curr_ctx, *msg_out, &outlen, (const unsigned char*)msg_in, inlen) <= 0)
    {
        printf("Unable to encrypt msg \n");
    }
    return (outlen);
}

unsigned char       *DecryptMessage(char *msg_in, int inlen, EVP_PKEY *pkey)
{
    EVP_PKEY_CTX    *curr_ctx;
    unsigned char   *msg_out;
    size_t          outlen = 0;

    curr_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (EVP_PKEY_decrypt_init(curr_ctx) <= 0)
    {
        printf("Unable to init decrypt \n");
    }
    if (EVP_PKEY_CTX_set_rsa_padding(curr_ctx, RSA_PKCS1_PADDING) <= 0)
    {
        printf("Unable to set padding \n");
    }
    if (EVP_PKEY_decrypt(curr_ctx, NULL, &outlen, (const unsigned char*)msg_in, inlen) <= 0)
    {
        printf("Unable to get decrypt len \n");
    }
    msg_out = (unsigned char *)OPENSSL_malloc(outlen + 1);
    if (!msg_out)
    {
        printf("Unable to malloc decrypt msg \n");
    }
    if (EVP_PKEY_decrypt(curr_ctx, msg_out, &outlen, (const unsigned char*)msg_in, inlen) <= 0)
    {
        printf("Unable to decrypt msg\n");
    }
    msg_out[outlen] = '\0';
    msg_in = NULL;
    return (msg_out);
}