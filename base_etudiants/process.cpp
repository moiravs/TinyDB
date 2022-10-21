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

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void *create_shared_memory(size_t size)
{
    // Our memory buffer will be readable and writable:
    int protection = PROT_READ | PROT_WRITE;

    // The buffer will be shared (meaning other processes can access it), but
    // anonymous (meaning third-party processes cannot obtain an address for it),
    // so only this process and its children will be able to use it:
    int visibility = MAP_SHARED | MAP_ANONYMOUS;

    // The remaining parameters to `mmap()` are not important for this use case,
    // but the manpage for `mmap` explains their purpose.
    return mmap(NULL, size, protection, visibility, -1, 0);
}
//mremap
int main(int argc, char const *argv[])
{
    void *shmem = create_shared_memory(256);
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
            read(fd2[0], jsp, 256);
            close(fd2[0]);
            if (strcmp(jsp, "01") != 0){
                char *querymod = new char[256]; // créer un nv string modifiable car strtok modifie les strings
                memcpy(querymod, jsp, 256);
                char *saveptr;
                const char *queryKey = new char[6](); // premier mot de la query (insert, delete, ...)
                queryKey = strtok_r(querymod, " ", &saveptr);
            
            if (strcmp(queryKey, "select") == 0)
            {
                puts("ahhh ça fonctionne1");
                //code à faire
                memcpy(&queryKey, "0", 6);
                write(fd2[1], "0", 256);
                close(fd2[1]);
                memcpy(shmem, "ahhh", 5);
            }
            memcpy(jsp, "01", 256);
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
            sleep(2);
            printf("%s\n", (char *)shmem);
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
