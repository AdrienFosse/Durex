#include "../includes/daemon.h"

void					quit()
{
	int 				fd;
	int 				ret = EXIT_SUCCESS;

	fd = open("/var/lock/durex.lock", O_CREAT);
    if (fd == -1)
        ret = EXIT_FAILURE;
    if (flock(fd, LOCK_UN) == -1)
        ret = EXIT_FAILURE;
    close(fd);
    if (remove("/var/lock/durex.lock") != 0);
        ret = EXIT_FAILURE;
	exit(ret);
}

void					key_exchange(t_session *session, t_rsa *rsa)
{
	unsigned char 		*msg;
	EVP_PKEY    		*client_pkey;
    int                 len;

	len = ReadMessage(session->sock, &msg);
	client_pkey = GeneratePKEY(msg, len);
	if (client_pkey == NULL){
		printf("INVALID CLIENT KEY\n");
	}
	printf("Client pubkey received\n");
    free(msg);

	len = EncryptMessage((char *)session->aes->_key, &msg, client_pkey);
	SendMessage(session->sock, msg, len);
	printf("encrypted symmetric key sent \n");
	free(msg);

	len = EncryptMessage((char *)session->aes->_IV, &msg, client_pkey);
	SendMessage(session->sock, msg, len);
    printf("encrypted symmetric IV sent\n");
	free(msg);

}

int							getMessage(t_session *session, unsigned char **msg_out)
{
	unsigned char 			*msg_cipher;
	unsigned char 			*msg;
	int 					len;

	len = ReadMessage(session->sock, &msg_cipher);
	if (len == -1)
		return -1;
	msg = (unsigned char *)malloc(sizeof(unsigned char) * len);
	if (msg == NULL)
		return -1;
	msg = memset(msg, 0, len);
	len = decrypt(session->aes, msg_cipher, len, &msg);
	*msg_out = msg;
	return len;
}

void sendEncryptedMessage(unsigned char *msg, t_session *session)
{
    int             cryp_len;
    int             plain_len;
    unsigned char   *crypt_msg;

    plain_len = strlen((const char *)msg);
    crypt_msg = (unsigned char *)malloc(sizeof(unsigned char) * (plain_len + AES_BLOCK_SIZE));
	if (crypt_msg == NULL)
		quit();
    cryp_len = encrypt(session->aes, msg, plain_len, &crypt_msg);
    SendMessage(session->sock, crypt_msg, cryp_len);
	free(crypt_msg);
}

int login(t_session *session)
{
	int					len;
	unsigned char 		hash[MD5_DIGEST_LENGTH];
	unsigned char 		hex_hash[MD5_DIGEST_LENGTH * 4];
	unsigned char 		*msg;
	const unsigned char pass[] = "098f6bcd4621d373cade4e832627b4f6";

	len = getMessage(session, &msg);
	if (len == -1)
		return -1;
	printf("Password : %s\n", msg);
	MD5(msg, len, hash);
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		snprintf((char *)hex_hash + (i * 2), 3, "%02x", hash[i]);
	}
	free(msg);
	if (memcmp(hex_hash, pass, MD5_DIGEST_LENGTH) == 0)
	{
		sendEncryptedMessage((unsigned char *)"OK", session);
		return 1;
	}
	else{
		sendEncryptedMessage((unsigned char *)"NOT OK", session);
		return -1;
	}
}

void clean_session(t_session *session)
{
	free(session->aes->_key);
	free(session->aes->_IV);
	close(session->sock);
	free(session);
}

int max(int sock, int err, int out)
{
	int max = sock;
	if (err > max)
		max = sock;
	if (out > max)
		max = out;
	return (max);
}

int    						shell(t_session *session)
{
    char           			*buffer;
    t_shell                 *shell;
	int						read_bytes;
	fd_set 					rfds;
	unsigned char 			*cmd = NULL;
	int                     retval;


    shell = create_shell();
	printf("Start shell\n");
    buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
    if (buffer == NULL)
        quit();
	while (42)
	{
		FD_ZERO(&rfds);
		FD_SET(session->sock, &rfds);
		FD_SET(shell->outfd[READ_END], &rfds);
    	FD_SET(shell->errfd[READ_END], &rfds);

		retval = select(max(session->sock, shell->outfd[READ_END], shell->errfd[READ_END]) + 1, &rfds, NULL, NULL, NULL);
		if (retval == -1)
			break;
		if (retval)
		{
			if (FD_ISSET(session->sock, &rfds))
			{
				retval = getMessage(session, &cmd);
				if (retval == -1)
				{
					free(shell);
					free(cmd);
					return (-1);
				}
				if (cmd != NULL)
				{
					printf("commande : %s\n", cmd);
					if ((strcmp((const char*)cmd, (const char*)"quit") == 0) || (strcmp((const char*)cmd, (const char*)"exit") == 0))
					{
						free(shell);
						free(cmd);
						return (-1);
					}
					write(shell->infd[WRITE_END], cmd, strlen((char *)cmd));
					write(shell->infd[WRITE_END], "\n", 1);
					free(cmd);
				}
			}
			else
			{
				if (FD_ISSET(shell->outfd[READ_END], &rfds))
				{
					read_bytes = read(shell->outfd[READ_END], buffer, BUFFER_SIZE);
				}
				else if (FD_ISSET(shell->errfd[READ_END], &rfds))
				{
					read_bytes = read(shell->errfd[READ_END], buffer, BUFFER_SIZE);
				}
				if (read_bytes > 0)
				{
					sendEncryptedMessage((unsigned char *)buffer, session);
					memset(buffer, 0, BUFFER_SIZE);
				}
			}
		}
	}
	printf("Shell closed\n");
	free(shell);
	pthread_exit(NULL);
	return 0;
}

int manage_message(t_session *session, unsigned char *msg)
{
	if (strcmp((char *)msg, "shell") == 0)
		return shell(session);
	else if ((strcmp((char *)msg, "quit") == 0) || (strcmp((char *)msg, "exit")) == 0)
		return (-1);
	else
		printf("%s\n", msg);
	return (0);
}

void *main_process(void *sess)
{
	unsigned char 				*msg;
	int 						len;
	t_session 					*session;
	session = (t_session *)sess;
	session->aes = (t_aes *)malloc(sizeof(t_aes));
    if (session->aes == NULL){
        quit();
    }
    create_keys(&(session->aes));
	key_exchange(session, server.rsa);
	if (login(session) == 1)
	{
		printf("User logged\n");
		while (42)
		{
			if ((len = getMessage(session, &msg)) == -1)
				break;
			if (manage_message(session, msg) == -1)
				break;
			free(msg);
		}
	}
	else
	{
		printf("Login failure\n");
	}
	clean_session(session);
	printf("Client quit his connection\n");
	server.nb_client -= 1;
	return (NULL);
}