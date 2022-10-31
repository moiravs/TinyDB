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

void database_t::db_save(const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
    {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(this->data, sizeof(student_t), this->lsize, f) < 0)
    {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
    std::cout << "db save finisihed" << std::endl;
}

void database_t::db_load( const char *path)
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
        this->db_add(student);
    }
    fclose(file);
}

void database_t::db_upsize()
{
    if (this->lsize > (this->psize / sizeof(student_t))) // if we reached the end of the allocated size for this
    {
        size_t oldPsize = this->psize;
        this->psize *= 2;
        student_t *newStudent;
        newStudent = (student_t *)mmap(NULL, this->psize, PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS, -1, 0); // establishes a mapping between an adress space of a process and a memory object
        memcpy(newStudent, this->data, oldPsize);                            // copy this to newly allocated memory
        munmap(this->data, oldPsize);                                        // deallocate old memory
        this->data = newStudent;
    }
}

void database_t::db_init()
{
    this->lsize = 0;
    this->psize = sizeof(student_t) * 2000000;
    this->data = (student_t *)mmap(NULL, this->psize, PROT_READ | PROT_WRITE,
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void database_t::db_add(student_t student)
{
    bool exists = false;
    this->lsize += 1;
    this->db_upsize();
    memcpy(&this->data[this->lsize - 1], &student, 256); // at end of db
}

void database_t::db_delete(size_t indice)
{
    if (indice >= this->lsize)
    {
        perror("db_delete()");
    }
    this->lsize--;
    memmove(&this->data[indice], &this->data[indice + 1], (sizeof(student_t) * (this->lsize - indice))); // overriding a student by moving what's after it to the deleted student's location in memory
}