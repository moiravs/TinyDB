#include <iostream>
#include "query.hpp"
#include "process.hpp"
#include <unistd.h>

void process_select() {
    char query[256] = "01";
    bool killed = false;
    while (!killed)
    {
    
    close(fdSelect[1]);            // close the writing end of the pipe
    read(fdSelect[0], query, 256); // reads 256 bytes into memory area indicated by query
    if (strcmp(query, "01") != 0)  // if query changed
    {
        char *querymod = new char[256], *queryKey = new char[6](),* saveptr; // create a new modifiable string
        memcpy(querymod, query, 256);   // save query to querymod
        char success[256] = "SUCCESS";
        queryKey = strtok_r(querymod, " ", &saveptr); // write the first word to queryKey

        if (strcmp(queryKey, "KILL") == 0)
        {
            killed = true;
        }
        else if (strcmp(queryKey, "select") == 0)
        {
            query_result_t queryResult(query);
            queryResult.query_select(db, query, saveptr);

            close(fdResponse[0]);
            write(fdResponse[1], success, 256);
        }
        memcpy(query, "\n", 2); // change the query back to 01
    }}
}

void process_insert()
{
    char query[256] = "01";

    bool killed = false;
    while (!killed)
    {
        close(fdInsert[1]);
        read(fdInsert[0], query, 256);
        std::cout << query << std::endl;
        if (strcmp(query, "01") != 0)
        {
            char *querymod = new char[256], *queryKey = new char[6](), *saveptr; // create a new modifiable string
            memcpy(querymod, query, 256);
            char success[256] = "SUCCESS";
            queryKey = strtok_r(querymod, " ", &saveptr);
            if (strcmp(queryKey, "KILL") == 0)
            {
                killed = true;
            }
            else if (strcmp(queryKey, "insert") == 0)
            {
                query_result_t queryResult{query};
                queryResult.query_insert(db, query, saveptr);
                close(fdResponse[0]);
                write(fdResponse[1], success, 256);
            }
            memcpy(query, "\n", 2);
        }
    }
}
void process_delete()
{
    char query[256] = "01";

    bool killed = false;
    while (!killed)
    {
        close(fdDelete[1]);
        read(fdDelete[0], query, 256);
        if (strcmp(query, "\n") != 0)
        {
            char *querymod = new char[256], *queryKey = new char[6](), *saveptr; // create a new modifiable string
            memcpy(querymod, query, 256);
            char success[256] = "SUCCESS";
            queryKey = strtok_r(querymod, " ", &saveptr);
            if (strcmp(queryKey, "KILL") == 0)
            {
                killed = true;
            }

            else if (strcmp(queryKey, "delete") == 0)
            {
                query_result_t queryResult{query};
                queryResult.query_delete(db, query, saveptr);
                close(fdResponse[0]);
                write(fdResponse[1], success, 256);
            }
            memcpy(query, "\n", 2);
        }
    }
    exit(0);
}
void process_update()
{
    char query[256] = "01";
    bool killed = false;
    while (!killed)
    {
        close(fdUpdate[1]);
        read(fdUpdate[0], query, 256);
        if (strcmp(query, "01") != 0)
        {
            char *querymod = new char[256], *queryKey = new char[6](), *saveptr; // create a new modifiable string
            memcpy(querymod, query, 256);
            char success[256] = "SUCCESS";
            queryKey = strtok_r(querymod, " ", &saveptr);
            if (strcmp(queryKey, "KILL") == 0)
            {
                killed = true;
            }

            else if (strcmp(queryKey, "update") == 0)
            {
                query_result_t queryResult{query};
                queryResult.query_update(db, query, saveptr);
                close(fdResponse[0]);
                write(fdResponse[1], success, 256);
            }
            memcpy(query, "\n", 2);
        }
    }
}