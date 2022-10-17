#include "db.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>

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
    char buffer[60];
    while (fread(&student, sizeof(student_t), 1, file))
    {
        student_to_str(buffer, &student);
        //db_add(db, student);
    }
    fclose(file);
}

void db_upsize(database_t *db){
    if (db->lsize >= db->psize){
        student_t *old_data = db->data;
        size_t old_psize = db->psize;
        db->psize = db->psize * 2;
        db->data = (student_t *)malloc(db->psize * sizeof(student_t));
        memcpy(db->data, old_data, old_psize*sizeof(student_t));
        free(old_data);
    }
}

void db_init(database_t *db)
{
    /*db->lsize = 0;
    db->psize = 0;
    db->data = NULL;*/
    db->lsize = 0;
    db->psize = sizeof(student_t);
    db->data = new student_t;
}
/*
void db_add(database_t *db, student_t student)
{
    TODO upsize la db si taille logique == taille physique, puis ajouter le student à la fin de la db et taille_logique++ 
    db->data = malloc(student);
    if (db->lsize == 0) // si premier student
    {
        db->data->precedent_student = NULL;
    }
    else if (db->data + 1 == NULL) // si en dehors de dbdata
    {
        db->data->precedent_student = (db->data - 1);
    }
    else
    {
        db->data->precedent_student = (db->data - 1);
        db->data->next_student = (db->data + 1);
    }
    db->lsize += 1;
    db->psize += 1 * (sizeof(student));
}
*/