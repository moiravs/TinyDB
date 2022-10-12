#include "db.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "student.h"

void db_save(database_t *db, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(db->data, sizeof(student_t), db->lsize, f) < 0) {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
}

void db_load(database_t *db, const char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file)) {
        db_add(db, student);
    }
    fclose(file);
}

void db_init(database_t *db) {
  // Your code here
  db->lsize = 0;
  db->psize = 1*sizeof(int);
  db->data = (int*)malloc(db->psize * sizeof(int));
}

void db_add(database_t *db, student_t student) {
    // allouer espace supplémentaire pour l'étudiant
    int *array;
    db->lsize += 1;
    db->psize += 1*(sizeof(student));
    if (db->lsize == 0){
        array = realloc(array, 256);
    } else {
        realloc(array, size_of(student));
    }
    // db->data + espace supplémentaire (jsp comment on fait)
    // il faut ajouter un student à la fin de la liste, donc la fin c'est genre l'adresse mémoire + 1
    // on a la physical size, dcp on fait juste physicalsize*logical size et on a l'endroit où il faut le mettre
    // c'est comme ça qu'on change ? à vérifier
    *(db->data + (db->psize*db->lsize)) = student;
}