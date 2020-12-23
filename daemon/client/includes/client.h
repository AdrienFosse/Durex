#ifndef CLIENT_H
#define CLIENT_H

#include "../../common/includes/socket.h"
#include "../../common/includes/rsa.h"
#include "../../common/includes/aes.h"

#define INPUT_LIMIT 500 

typedef struct  s_client
{
    int         sock;
    int         logged;
    t_aes       *aes;
    t_rsa       *rsa;
}               t_client;

void            SendPubKey(int sock, char *own_pub_key);
EVP_PKEY        *GetServPubKey(int sock);
void            SendMessageToServ(t_client client, unsigned char *msg);
unsigned char   *RecieveMessageRsaType();
unsigned char   *RecieveMessageAesType(t_client client);
void            GetSymKey(t_client *client);
int             LoginToServ(t_client client, char *password);
void            main_process();

t_client client;

#endif