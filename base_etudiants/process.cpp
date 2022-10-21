#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>           // std::cout
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define PROCESS_SELECT 1
#define PROCESS_INSERT 2
#define PROCESS_MAIN 0

int main(int argc, char const *argv[])
{
    int mode = PROCESS_MAIN;
    int toto = 1;
    printf("toto: %d\n", toto);
    int err;
    int fd1[2];
    pipe(fd1);
    int fd2[2];
    pipe(fd2);
    /*
    int fd3[2];
    pipe(fd3);
    int fd4[2];
    pipe(fd4);
    */
    char query[256] = "0";
    pid_t child_select = fork();
    if (child_select < 0)
    {
        perror("fork error");
        abort();
    }
    if (child_select == 0)
    {
        toto = 2;
        printf("select process:%d\n", getpid());
        char jsp[256] = "01";
        puts("here");
        char okidok[256] = "okido";
        while (true)
        {
            close(fd2[1]);
            puts(jsp);
            read(fd2[0], jsp, 256);
            close(fd2[0]);
            if (strcmp(jsp, "select") == 0)
            {
                puts("ahhh ça fonctionne1");
                write(fd2[1], okidok, 256);
                close(fd2[1]);
            }
        }
        exit(0);
    }
    toto = 3;

    pid_t child_insert = fork();
    if (child_insert < 0)
    {
        perror("fork error");
    }
    if (child_insert == 0)
    {
        printf("insert process:%d\n", getpid());
        toto = 4;
        while (true)
        {
        }
    }
    printf("main process:%d\n", getpid());

    toto = 5;

    int status;
    pid_t pid;
    int n = 2;
    while (n > 0)
    {
        std::cin.getline(query, sizeof(query));
        write(fd2[1], query, 256);
        close(fd2[1]);
        //pid = wait(&status);
        //printf("Child PID %ld exit with status%d\n", (long)pid, status);
        //n--;
    }
}
