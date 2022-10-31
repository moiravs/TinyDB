/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "db.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <iostream>
#include "student.hpp"
#include <sys/mman.h>
#include <iterator>

void db_save(database_t *db, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(db->data, sizeof(student_t), db->lsize, f) < 0)
    {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
    std::cout << "db save finisihed" << std::endl;
}

void db_load(database_t *db, const char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file))
    {
        db_add(db, student);
    }
    fclose(file);
}

void db_upsize(database_t *db)
{
    if (db->lsize > (db->psize / sizeof(student_t))) // if we reached the end of the allocated size for db
    {
        size_t oldPsize = db->psize;
        db->psize *= 2;
        student_t *newStudent;
        newStudent = (student_t *)mmap(NULL, db->psize, PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS, -1, 0); // establishes a mapping between an adress space of a process and a memory object
        memcpy(newStudent, db->data, oldPsize);                            // copy db to newly allocated memory
        munmap(db->data, oldPsize);                                        // deallocate old memory
        db->data = newStudent;
    }
}

void database_t::db_init()
{
    this->lsize = 0;
    this->psize = sizeof(student_t) * 2000000;
    this->data = (student_t *)mmap(NULL, this->psize, PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void db_add(database_t *db, student_t student)
{
    bool exists = false;
    db->lsize += 1;
    db_upsize(db);
    memcpy(&db->data[db->lsize - 1], &student, 256); // at end of db
    /*
    char *buffer = new char[512];
    student_to_str(buffer, &db->data[db->lsize - 1]);
    printf("buffer : %s\n", buffer);*/
    msync(&db, sizeof(database_t), MS_SYNC);
    msync(db->data, db->psize, MS_SYNC); // synchronize the db with added student
}

void db_delete(database_t *db, size_t indice)
{
    if (indice >= db->lsize)
    {
        perror("db_delete()");
    }

    // std::copy(db->data[indice + 1], db->data + db->lsize, db->data + indice); // overwrite the object to delete by copying everything following the given adress of the object to the said adress
    db->lsize--;
    memmove(&db->data[indice], &db->data[indice + 1], (sizeof(student_t) * (db->lsize - indice))); // overriding a student by moving what's after it to the deleted student's location in memory
}