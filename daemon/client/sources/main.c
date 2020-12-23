#include "../includes/main.h"
extern t_client client;

void sig_handler(int signum)
{
    if (client.logged == 1)
        SendMessageToServ(client, (unsigned char *)"quit");
    free(client.aes->_key);
    free(client.aes->_IV);
    EVP_PKEY_free(client.rsa->_own_pkey);
    close(client.sock);
    exit(EXIT_SUCCESS);
}

int                Login(t_client client)
{
    int             isLog = -1;
    char            *user_input;

    user_input = (char *)malloc(sizeof(char) * INPUT_LIMIT);
    if (user_input == NULL)
        exit(EXIT_FAILURE);
    while (isLog == -1)
    {
        write(1, "password : ", 11);    
        fgets(user_input, INPUT_LIMIT, stdin);
        if (user_input[0] == '\n')
        {
            printf("Please enter a password\n");
            continue;
        }
        else
        {
            user_input[strlen(user_input) - 1] = '\0';
            if (LoginToServ(client, user_input) == 1)
            {
                printf("Successfully connected\n");
                isLog = 1;
                break;
            }
            else
            {
                printf("Incorrect password, sorry =)\n");
                break;
            }
        }
    }
    free(user_input);
    return (isLog);
}

void                init(char *address)
{
    memset(&client, 0, sizeof(client));
    signal(SIGABRT, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGTERM, sig_handler);

    client.rsa = (t_rsa *)malloc(sizeof(t_rsa));
    if (client.rsa == NULL)
        exit(EXIT_FAILURE);
    GenerateKeyPair(&client.rsa);
    client.sock = CreateSocket();
    ConnectToServ(client.sock, address);
    SendMessage(client.sock, (unsigned char *)client.rsa->_own_pub_key, strlen(client.rsa->_own_pub_key));
    GetSymKey(&client);
}

int                 main(int argc, char *argv[])
{   

    if (argc != 2)
    {
        printf("Usage: ./client <Server Address>\n");
        exit (EXIT_FAILURE);
    }
    init(argv[1]);
    if ((client.logged = Login(client)) == 1){
        main_process();
    }
    free(client.aes->_key);
    free(client.aes->_IV);
    EVP_PKEY_free(client.rsa->_own_pkey);
    close(client.sock);
    return (0);
}