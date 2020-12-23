#ifndef SHELL_H
#define SHELL_H

#define READ_END 0
#define WRITE_END 1
#define BUFFER_SIZE 4096
#include "daemon.h"

typedef struct  s_shell
{
    int         _pid;
    int         infd[2];
    int         outfd[2];
    int         errfd[2];

}               t_shell;

t_shell         *create_shell(void);

#endif