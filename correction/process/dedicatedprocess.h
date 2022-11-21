/**
 * Structure générale entre un processus dédié et le
 * processus principal. Les commandes sont transmises
 * grâce à un pipe.
 *
 * La synchronisation requise pour les transactions se
 * fait via un autre pipe, le pipe de synchronisation,
 * utilisé aussi pour synchroniser l'extension de la
 * mémoire partagée.
 *
 * Les processus dédiés qui reçoivent un SIGPIPE se
 * ferment proprement.
 *
 *
 * +---------------------+
 * | Processus principal |
 * +---------------------+
 *    ||             ^^
 *  command          ||
 *   pipe        transaction
 *    ||            pipe
 *    vv             ||
 * +---------------------+
 * |   Processus dédié   |
 * +---------------------+
 *
 */

#ifndef _DEDICATED_PROCESS_H
#define _DEDICATED_PROCESS_H

#include <sys/types.h>

#include "../db.h"

typedef enum {
   PROCESS_SELECT,
   PROCESS_UPDATE,
   PROCESS_INSERT,
   PROCESS_DELETE,
   LAST_PROCESS_TYPE=PROCESS_DELETE /* ==> nombre de processus = LAST_PROCESS_TYPE + 1 */
} process_type_t;

#define UNSPECIFIED_PROCESS 0
#define UNINIT_PIPE (-1)

typedef struct {
   pid_t pid;
   process_type_t type;
   int fd_command[2];
   int fd_transaction[2];
} dedicated_process_t;

/**
 * Crée les LAST_PROCESS_TYPE + 1 processus dédiés et met
 * à jour le tableau processes.
 *
 * @return false en cas d'erreur et true en cas de succès.
 **/
bool create_dedicated_processes(dedicated_process_t* processes, database_t* db);

#endif
