#ifndef DAEMON_H
#define DAEMON_H

#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

#include "../../common/includes/socket.h"
#include "../../common/includes/rsa.h"
#include "../../common/includes/aes.h"

typedef struct  s_daemon
{
    int         master_sock;
    short       nb_client;
    t_rsa       *rsa;

}               t_daemon;

typedef struct  s_session
{
    int         sock;
    t_aes       *aes;

}               t_session;

#include "shell.h"

t_daemon server;

void            clean_session(t_session *session);
void            *main_process(void *session);
void            key_exchange(t_session *session, t_rsa *rsa);
int		        getMessage(t_session *session, unsigned char **msg_out);
void            quit();

#endif