#ifndef _DB_H
#define _DB_H

#include "student.h"

/**
 * Structure de la base de données.
 */
typedef struct {
    int fd_shm;       /** Descripteur de fichier pour accéder à la mémoire partagée **/
    student_t *data;  /** Liste des étudiants **/
    size_t* lsize;    /** Taille logique (en mémoire partagée) **/
    size_t psize;     /** Taille physique **/
    const char* path; /** Chemin vers la BDD **/
} database_t;

/**
 * Ajouter un étudiant à la base de données.
 **/
void db_add(database_t *db, student_t s);

/**
 * Supprimer un étudiant de la base de données.
 **/
void db_delete(database_t *db, student_t *s);

/**
 * Sauvegarder le contenu de la base de données dans
 * le fichier spécifié.
 **/
void db_save(database_t *db, const char *path);

/**
 * Charger le contenu de la base de données depuis
 * un fichier.
 **/
void db_load(database_t *db, const char *path);

/**
 * Ferme la BDD. À appeler dans chaque processus.
 **/
void db_close(database_t *db);

/**
 * Afficher la base de données.
 * Débogage uniquement.
 **/
void db_display(database_t *db);

/**
 * Vérifie si la BDD est remplie.
 */
bool is_db_full(database_t *db);

/**
 * Double la mémoire physique allouée pour la BDD.
 **/
bool db_extend(database_t *db);

/**
 * Resynchronise la BDD sur base de db->fd_shm.
 * Le contenu de la mémoire et 
 **/
bool db_resync(database_t *db);

/**
 * Munmap la mémoire partagée contenant les étudiants.
 **/
void db_munmap(database_t *db);

#endif
