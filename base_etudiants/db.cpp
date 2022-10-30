#include "db.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
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
    if (db->lsize >= (db->psize / sizeof(student_t))) // if we reached the end of the allocated size for db
    {
        size_t oldPsize = db->psize;
        db->psize *= 2;
        student_t *newStudent;
        newStudent = (student_t *)mmap(NULL, db->psize, PROT_READ | PROT_WRITE, MAP_SYNC | MAP_SHARED | MAP_ANONYMOUS, -1, 0); // establishes a mapping between an adress space of a process and a memory object
        memcpy(newStudent, db->data, oldPsize);                                                                                // copy db to newly allocated memory
        munmap(db->data, oldPsize);                                                                                            // deallocate old memory
        db->data = newStudent;
    }
}

void db_init(database_t *db)
{
    db->lsize = -1;
    db->psize = sizeof(student_t) * 16;
    db->data = (student_t *)mmap(NULL, db->psize, PROT_READ | PROT_WRITE, MAP_SYNC | MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void db_add(database_t *db, student_t student)
{
    bool exists = false;
    db->lsize += 1;
    db_upsize(db);
    db->data[db->lsize] = student; // at end of db
}

void db_delete(database_t *db, size_t indice)
{
    if (indice >= db->lsize)
    {
        perror("db_delete()");
    }

    // std::copy(db->data[indice + 1], db->data + db->lsize, db->data + indice); // overwrite the object to delete by copying everything following the given adress of the object to the said adress
    db->lsize--;
    memmove(&db->data[indice], &db->data[indice + 1], (sizeof(student_t) * (db->lsize - indice)));
}