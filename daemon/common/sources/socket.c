#include "../includes/socket.h"

int CreateSocket(void)
{
  	struct protoent		*proto;
    int                 _sock_fd;

    proto = getprotobyname("tcp");
	if (proto == 0)
    {
        exit(EXIT_FAILURE);
    }
        
    _sock_fd = socket(AF_INET, SOCK_STREAM, proto->p_proto);
    if (_sock_fd == -1)
    {
        printf("Unable to create client socket\n");
        exit(EXIT_FAILURE);
    }
    return _sock_fd;
}

void bind_to_port(int sock)
{
    struct sockaddr_in	sin;
    int                 param = 1;

    sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(int)) < 0)
    {
        printf("%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if (bind(sock, (const struct sockaddr*)&sin, sizeof(sin)) == -1)
	{
        printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	listen(sock, MAX_CLIENT);
}

void ConnectToServ(int sock, const char *address)
{
    struct sockaddr_in server_addr;

    server_addr.sin_addr.s_addr = inet_addr(address);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("Unable to establish connection to server\n");
        exit(EXIT_FAILURE);
    }
}

void SendMessage(int sock, unsigned char *msg, int size)
{
    int ret;

    //send size
    ret = send(sock, &size, sizeof(int), 0);
    if (ret == -1){
        printf("TAMERE SEND\n");
        printf("%s\n", strerror(errno));

    }
   //send msg
    ret = send(sock, msg, size, 0);
    // printf("Send %d/%d bytes\n", ret, size);

    if (ret == -1){
        printf("TAMERE SEND 2\n");
        printf("%s\n", strerror(errno));
    }
}

int ReadMessage(int sock, unsigned char **buf)
{
    int     size;
    int     ret;
    int     curr_size = 0;
    unsigned char    *msg_recv;

    // receive size
    ret = recv(sock, &size, sizeof(int), 0);
    if (ret <= 0){
        return (-1);
    }
    //receive msg
    msg_recv = (unsigned char *)malloc(sizeof(unsigned char) * (size + 1));
    bzero(msg_recv, size);
    if (msg_recv != NULL)
    {
        while (curr_size < size)
        {
            ret = recv(sock, msg_recv + curr_size, size - curr_size, 0);
            // printf("Receive %d/%d bytes\n", ret, size);

            if (ret == -1){
                printf("TAMERE SEND 2\n");
                return (-1);
            }
            curr_size += ret;
        }
        
    }
    msg_recv[size] = '\0';
    *buf = msg_recv;
    // printf ("strlen receive : %zu", str)
    return(size);
}
