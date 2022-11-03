/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Cursus : BA2-INFO
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/
#include <iostream>
#include "query.hpp"
#include "process.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include "utils.hpp"
#include <cstring>

int operation_in_progress = 0;
pid_t child_select = -1;
pid_t child_insert = -1;
pid_t child_delete = -1;
pid_t child_update = -1;

void close_application(bool force)
{
    char *status = new char[256];

    if (!force)
    {
        puts("Waiting for requests to terminate");
        close(fd_response[1]);
        while (operation_in_progress > 0)
        {
            while (read(fd_response[0], status, 256) > 0)
            {
                operation_in_progress--;
            }
            if (operation_in_progress > 0)
            {
                std::cout << operation_in_progress << " operations in progress: Wait" << std::endl;
                sleep(1);
            }
        }
    }

    puts("Committing database changes to the disk...");
    db->db_save(db_path);
    char kill_message[256] = "KILL";

    if (!force)
    {

        close(fd_select[0]);
        safe_write(fd_select[1], kill_message, 256);
        close(fd_update[0]);
        safe_write(fd_update[1], kill_message, 256);
        close(fd_delete[0]);
        safe_write(fd_delete[1], kill_message, 256);
        close(fd_insert[0]);
        safe_write(fd_insert[1], kill_message, 256);
        int wstatus;
        waitpid(child_delete, &wstatus, 0);
        waitpid(child_select, &wstatus, 0);
        waitpid(child_insert, &wstatus, 0);
        waitpid(child_update, &wstatus, 0);
    }
    else
    {
        kill(child_delete, SIGKILL);
        kill(child_insert, SIGKILL);
        kill(child_select, SIGKILL);
        kill(child_update, SIGKILL);
    }

    puts("Done");
}
void signal_handling(int signum)
{
    close_application(signum != 2 or signum != 10);

    kill(getpid(), SIGKILL);
}

void process_select()
{
    char query[256];
    bool killed = false;
    while (!killed)
    {
        close(fd_select[1]);                                                           // close the writing end of the pipe
        safe_read(fd_select[0], query, 256);                                           // reads 256 bytes into memory area indicated by query
        char *query_copy = new char[256], *query_key = new char[6](), *p_end_of_query; // create a new modifiable string
        memcpy(query_copy, query, 256);                                                // save query to query_copy
        char success[256] = "SUCCESS";
        query_key = strtok_r(query_copy, " ", &p_end_of_query); // write the first word to query_key

        if (strcmp(query_key, "KILL") == 0)
        {
            killed = true;
        }
        else if (strcmp(query_key, "select") == 0)
        {
            query_result_t query_result(query);
            query_result.query_select(db, query, p_end_of_query);

            close(fd_response[0]);
            safe_write(fd_response[1], success, 256);
        }
    }
    exit(0);
}

void process_insert()
{
    char query[256];

    bool killed = false;
    while (!killed)
    {
        close(fd_insert[1]);
        safe_read(fd_insert[0], query, 256);
        char *query_copy = new char[256], *query_key = new char[6](), *p_end_of_query; // create a new modifiable string
        memcpy(query_copy, query, 256);
        char success[256] = "SUCCESS";
        query_key = strtok_r(query_copy, " ", &p_end_of_query);
        if (strcmp(query_key, "KILL") == 0)
        {
            killed = true;
        }
        else if (strcmp(query_key, "insert") == 0)
        {
            query_result_t query_result{query};
            query_result.query_insert(db, query, p_end_of_query);
            close(fd_response[0]);
            safe_write(fd_response[1], success, 256);
        }
    }
    exit(0);
}
void process_delete()
{
    char query[256];

    bool killed = false;
    while (!killed)
    {
        close(fd_delete[1]);
        safe_read(fd_delete[0], query, 256);
        char *query_copy = new char[256], *query_key = new char[6](), *p_end_of_query; // create a new modifiable string
        memcpy(query_copy, query, 256);
        char success[256] = "SUCCESS";
        query_key = strtok_r(query_copy, " ", &p_end_of_query);
        if (strcmp(query_key, "KILL") == 0)
        {
            killed = true;
        }

        else if (strcmp(query_key, "delete") == 0)
        {
            query_result_t query_result{query};
            query_result.query_delete(db, query, p_end_of_query);
            close(fd_response[0]);
            safe_write(fd_response[1], success, 256);
        }
    }

    exit(0);
}
void process_update()
{
    char query[256];
    bool killed = false;
    while (!killed)
    {
        close(fd_update[1]);
        safe_read(fd_update[0], query, 256);
        char *query_copy = new char[256], *query_key = new char[6](), *p_end_of_query; // create a new modifiable string
        memcpy(query_copy, query, 256);
        char success[256] = "SUCCESS";
        query_key = strtok_r(query_copy, " ", &p_end_of_query);
        if (strcmp(query_key, "KILL") == 0)
        {
            killed = true;
        }

        else if (strcmp(query_key, "update") == 0)
        {
            query_result_t query_result{query};
            query_result.query_update(db, query, p_end_of_query);
            close(fd_response[0]);
            safe_write(fd_response[1], success, 256);
        }
    }
    exit(0);
}

void main_process()
{

    child_select = fork(); // create child
    if (child_select < 0)
    {
        perror("fork error");
        abort();
    }
    if (child_select == 0)
        process_select();

    child_insert = fork();
    if (child_insert < 0)
    {
        perror("fork error");
        abort();
    }
    if (child_insert == 0)
        process_insert();

    child_update = fork();
    if (child_update < 0)
    {
        perror("fork error");
        abort();
    }
    if (child_update == 0)
        process_update();

    child_delete = fork();
    if (child_delete < 0)
    {
        perror("fork error");
        abort();
    }
    if (child_delete == 0)
        process_delete();

    char query[256] = "0";
    bool transaction = false;
    char *status = new char[256]; // put the response of the fd_response pipe
    while (!std::cin.eof() && std::cin.getline(query, 256))
    {
        close(fd_response[1]);
        while (read(fd_response[0], status, 256) > 0)
        {
            operation_in_progress--;
        }
        if (operation_in_progress > 0)
        {
            std::cout << operation_in_progress << " operations in progress: no wait" << std::endl;
        }
        query[strcspn(query, "\n")] = 0;
        char *query_copy = new char[256], *p_end_of_query;
        memcpy(query_copy, query, 256);
        const char *query_key = new char[6]();
        query_key = strtok_r(query_copy, " ", &p_end_of_query);

        if (strcmp(query, "transaction") == 0)
        {
            close(fd_response[1]);
            while (operation_in_progress > 0)
            {
                while (read(fd_response[0], status, 256) > 0)
                    operation_in_progress--;
                if (operation_in_progress > 0)
                {
                    std::cout << operation_in_progress << " operations in progress: Wait" << std::endl;
                    sleep(1);
                }
            }
            transaction ^= transaction;
        }
        else if (strcmp(query_key, "select") == 0)
        {
            operation_in_progress++;
            close(fd_select[0]);
            safe_write(fd_select[1], query, 256);
        }
        else if (strcmp(query_key, "insert") == 0)
        {
            operation_in_progress++;
            close(fd_insert[0]);
            safe_write(fd_insert[1], query, 256);
        }
        else if (strcmp(query_key, "update") == 0)
        {
            operation_in_progress++;
            close(fd_update[0]);
            safe_write(fd_update[1], query, 256);
        }
        else if (strcmp(query_key, "delete") == 0)
        {
            operation_in_progress++;
            close(fd_delete[0]);
            safe_write(fd_delete[1], query, 256);
        }
        else
        {
            puts("Bad query");
        }

        if (transaction == true)
        {
            close(fd_response[1]);
            while (operation_in_progress > 0)
            {
                std::cout << operation_in_progress << " operations in progress: Wait" << std::endl;
                if (read(fd_response[0], status, 256) > 0)
                {
                    operation_in_progress--;
                }
                sleep(1);
            }
        }
    }
}
