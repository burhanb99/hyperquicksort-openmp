#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handler(int sig)
{
    write(1, "success", strlen("success")); // printf is not recommended here, but should work as well
    exit(0);
}

int main()
{
    struct sigaction sa;
    memset (&sa, '\0', sizeof(sa));
    sa.sa_sigaction = &handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL); // register handler for SIGSEGV
    int *a; // a will contain some garbage value
    int b = *a; // trigger segmentation fault; transfer control to handler
}