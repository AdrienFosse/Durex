#include "../includes/main.h"

extern t_daemon server;

void sig_handler(int signum)
{
    printf("received signal %d\n", signum);
    quit();
}

void daemonize()
{
    pid_t pid;
    pid_t sid;

    pid = fork();
    if (pid == -1)
    {
        printf("Failed to daemonize\n");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
        exit(EXIT_SUCCESS);
        
    //Child 777 permissions
    umask(0);
    //Set Child as Lead
    sid = setsid();
    if (sid == -1)
        exit(EXIT_FAILURE);
    chdir("/");
}

void wait_connexion()
{
    uint32_t		    cslen;
	int				    new_sock;
	struct sockaddr_in	csin;
    t_session           *session;
    pthread_t           thread;
    
    server.nb_client = 0;
	while ((new_sock = accept(server.master_sock, (struct sockaddr*)&csin, &cslen)) != -1)
	{
        printf("New client from socket\n");
        if (server.nb_client >= 3)
        {
            close(new_sock);
            continue;
        }
        server.nb_client++;

        session = (t_session *)malloc(sizeof(t_session));
        if (session == NULL){
            quit();
        }
        session->sock = new_sock;
        if(pthread_create(&thread, NULL, &main_process, session) == -1)
        {
            perror("pthread_create");
            quit();
        }
    }
    printf("Server quit\n");
}

void init()
{
    server.master_sock = CreateSocket();
    server.rsa = (t_rsa *)malloc(sizeof(t_rsa));
    if (server.rsa == NULL)
        exit(EXIT_FAILURE);
    bind_to_port(server.master_sock);
    daemonize();
    printf ("start generate keys\n");
    GenerateKeyPair(&(server.rsa));

    signal(SIGABRT, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGILL, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGSEGV, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGPIPE, sig_handler);
}

int             main(void)
{
    int fd;

    if (getuid() != 0)
    {
        exit(EXIT_FAILURE);
    }
    fd = open("/var/lock/durex.lock", O_CREAT);
    if (fd == -1)
    {
        printf("FAIL OPEN DUREX.LOCK\n");
        exit(EXIT_FAILURE);
    }
    if (flock(fd, LOCK_EX | LOCK_NB) == -1)
    {
        printf("FAIL PUT LOCK ON DUREX\n");
        exit(EXIT_FAILURE);
    }
    init();
    wait_connexion();
    quit();
    printf("DAEMONIZE DONE\n");
    return (0);
}