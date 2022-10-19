#include "db.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <iostream>
#include "student.hpp"

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
    std::cout << "i wrote";
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
        // student_to_str(buffer, &student);
        db_add(db, student);
    }
    fclose(file);
}

void db_upsize(database_t *db)
{
    if (db->lsize >= (db->psize / sizeof(student_t)))
    {
        student_t *old_data = db->data;  // need to stock old data to copy it to new allocated memory
        size_t old_psize = db->psize;
        db->psize *= 2;
        db->data = (student_t *)malloc(db->psize);
        memcpy(db->data, old_data, old_psize);
        free(old_data);
    }
}

void db_init(database_t *db)
{
    db->lsize = 0;
    db->psize = sizeof(student_t);
    db->data = new student_t;
}

void db_add(database_t *db, student_t student)
{
    db->lsize += 1;
    db_upsize(db);
    db->data[db->lsize] = student;  // at end of db
}

void db_delete(database_t *db, size_t indice){
    if (indice >= db->lsize){  // indice out of range
        perror("db_delete()");
    }
    db->lsize--;
    for (size_t i = indice; i < db->lsize; i++){
        db->data[i] = db->data[i+1];
    }
}