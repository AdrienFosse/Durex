#include "../includes/client.h"

unsigned char       *RecieveMessageRsaType(int sock, EVP_PKEY *_own_pkey)
{
    unsigned char   *msg_receive;
    unsigned char   *clear_msg;
    int             len;
    
    len = ReadMessage(sock, &msg_receive);
    clear_msg = DecryptMessage(msg_receive, len, _own_pkey);
    free(msg_receive);
    return(clear_msg);
}

void                GetSymKey(t_client *client)
{
    (*client).aes = (t_aes *)malloc(sizeof(t_aes));
    if ((*client).aes == NULL){
        exit(EXIT_FAILURE);
    }   
    (*client).aes->_key = RecieveMessageRsaType((*client).sock, client->rsa->_own_pkey);
    (*client).aes->_IV = RecieveMessageRsaType((*client).sock, client->rsa->_own_pkey);
}

EVP_PKEY            *GetServPubKey(int sock)
{
    unsigned char   *msg;
    EVP_PKEY        *serv_pkey;
    int             len;

    len = ReadMessage(sock, &msg);
    printf("%s\n", msg);
    serv_pkey = GeneratePKEY(msg, len);
    free(msg);
    return (serv_pkey);
}

int                 LoginToServ(t_client client, char *password)
{
    unsigned char   *msg_receive;
    int             ret;

    ret = 0;
    SendMessageToServ(client, (unsigned char *)password);
    msg_receive = RecieveMessageAesType(client);
    if (memcmp(msg_receive, "OK", 2) == 0)
        ret = 1;
    free(msg_receive);
    return (ret);
}

void                SendMessageToServ(t_client client, unsigned char *msg)
{
    int             cryp_len;
    int             plain_len;
    unsigned char   *crypt_msg;

    plain_len = strlen((const char*)msg);
    crypt_msg = (unsigned char *)malloc(sizeof(unsigned char) * (plain_len + AES_BLOCK_SIZE));
    if (crypt_msg == NULL)
        exit(EXIT_FAILURE);
    crypt_msg = memset(crypt_msg, 0, plain_len + AES_BLOCK_SIZE);
    cryp_len = encrypt(client.aes, msg, plain_len, &crypt_msg);
    SendMessage(client.sock, crypt_msg, cryp_len);
}

unsigned char       *RecieveMessageAesType(t_client client)
{
    unsigned char   *msg_receive;
    unsigned char   *clear_msg;
    int             len;
    
    len = ReadMessage(client.sock, &msg_receive);
    clear_msg = (unsigned char *)malloc(sizeof(unsigned char) * len);
	if (clear_msg == NULL)
		exit(EXIT_FAILURE);
    decrypt(client.aes, msg_receive, len, &clear_msg);
    return(clear_msg);
}

int                 manage_cmd(char *user_input, int shell_on)
{
    int             ret = 1;

    if ((strcmp(user_input, "quit") == 0) || (strcmp(user_input, "exit")) == 0)
        ret = -1;
    SendMessageToServ(client, (unsigned char *)user_input);
    return (ret);
}

void                main_process()
{
    char            prompt[6];
    char            *user_input;
    int             shell_on = 0;
    fd_set 			rfds;
    int             retval;
    unsigned char   *output;


    strcpy(prompt, ">: ");
    write(1, prompt, strlen(prompt));
    user_input = (char *)malloc(sizeof(char) * INPUT_LIMIT);
    if (user_input == NULL)
        exit(EXIT_FAILURE);
    user_input = memset(user_input, '\0', INPUT_LIMIT);
    while(42)
    {
        FD_ZERO(&rfds);
        FD_SET(client.sock, &rfds);
        FD_SET(STDIN_FILENO, &rfds);

        retval = select(client.sock + 1, &rfds, NULL, NULL, NULL);
        if (retval == -1)
        {
            perror("select()");
            break;
        }
        else if(retval)
        {
            if (FD_ISSET(STDIN_FILENO, &rfds))
            {
                retval = read(STDIN_FILENO, user_input, INPUT_LIMIT);
                user_input[retval - 1] = '\0';
                if (manage_cmd(user_input, shell_on) == -1)
                    break;
            }
            else if (FD_ISSET(client.sock, &rfds))
            {
                output = RecieveMessageAesType(client);
                printf("%s\n", output);
            }
        }
        write(1, prompt, strlen(prompt));
    }
    free(user_input);
}