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
}

void db_add(database_t *db, student_t student) {
    // allouer espace supplémentaire pour l'étudiant
    if (db->lsize > 0)
    {
        db->data = (student_t*)realloc(db->data, 256);
    }
    else
    {
        db->data = (student_t*)malloc(sizeof(student));
    }
    db->lsize += 1;
    db->psize += 1*(sizeof(student));
    // db->data + espace supplémentaire (jsp comment on fait)
    // il faut ajouter un student à la fin de la liste, donc la fin c'est genre l'adresse mémoire + 1
    // on a la physical size, dcp on fait juste physicalsize*logical size et on a l'endroit où il faut le mettre
    // c'est comme ça qu'on change ? à vérifier
    *(db->data + db->lsize*db->psize + 1) = student; // pointeur actuel (qq part en mémoire) + taille physique de la bdd*taille logique (pas sûr)
}