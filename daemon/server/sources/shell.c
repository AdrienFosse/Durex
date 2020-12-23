#include "../includes/shell.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

t_shell     *create_shell()
{
    t_shell *shell;
    int     rc;

    shell = (t_shell *)malloc(sizeof(t_shell));
    if (shell == NULL)
        exit(EXIT_FAILURE);

    shell->infd[0] = 0;
    shell->infd[1] = 0;
    shell->outfd[0] = 0;
    shell->outfd[1] = 0;
    shell->errfd[0] = 0;
    shell->errfd[1] = 0;

    rc = pipe(shell->infd);
    if(rc < 0){
        exit(EXIT_FAILURE);
    }

    rc = pipe(shell->outfd);
    if(rc < 0)
    {
        close(shell->infd[READ_END]);
        close(shell->infd[WRITE_END]);
        exit(EXIT_FAILURE);
    }

    rc = pipe(shell->errfd);
    if(rc < 0)
    {
        close(shell->infd[READ_END]);
        close(shell->infd[WRITE_END]);
        close(shell->outfd[READ_END]);
        close(shell->outfd[WRITE_END]);
        exit(EXIT_FAILURE);
    }
    shell->_pid = fork();
    if(shell->_pid > 0) // PARENT
    {
        close(shell->infd[READ_END]);    // Parent does not read from stdin
        close(shell->outfd[WRITE_END]);  // Parent does not write to stdout
        close(shell->errfd[WRITE_END]);  // Parent does not write to stderr
    }
    else if(shell->_pid == 0) // CHILD
    {
        dup2(shell->infd[READ_END], STDIN_FILENO);
        dup2(shell->outfd[WRITE_END], STDOUT_FILENO);
        dup2(shell->errfd[WRITE_END], STDERR_FILENO);


        close(shell->infd[WRITE_END]);   // Child does not write to stdin
        close(shell->outfd[READ_END]);   // Child does not read from stdout
        close(shell->errfd[READ_END]);   // Child does not read from stderr
        execl("/bin/bash", "bash", NULL);
    }
    // PARENT
    if(shell->_pid < 0){
        exit(EXIT_FAILURE);
    }
    return shell;
}