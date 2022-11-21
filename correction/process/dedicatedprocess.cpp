#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>

#include "process.h"
#include "../ipc/ipc.h"
#include "../query.h"
#include "../parsing.h"
#include "../log.h"
#include "../ipc/sighandler.h"
#include "../db.h"

/**
 * Effectue la requête reçue.
 * 
 * @return 0 (pas de requête à log), 1 (requête à log)
 */
static inline bool perform_user_request(const dedicated_process_t* process, database_t* db, char* query, query_result_t* result) {
   if (is_select_request(query)) {
      return perform_select(db, query, result);
   } else if (is_update_request(query)) {
      return perform_update(db, query, result);
   } else if (is_insert_request(query)) {
      if (is_db_full(db)) {
         fprintf(stderr, "Error in the logic: the db is full but this should never happen. The following insert request has been ignored: %s\n", query);
         return 0;
      }
      
      return perform_insert(db, query, result);
   } else if (is_delete_request(query)) {
      return perform_delete(db, query, result);
   } else {
      if (is_transaction_request(query)) {
         notify_sync_done(process);
      } else if (strcmp(query, "munmap\n") == 0) {
         db_munmap(db);
         notify_sync_done(process);
      } else if (strcmp(query, "resync\n") == 0) {
         db_resync(db); // Actualiser la mémoire partagée
      } else {
         fprintf(stderr, "Unknown command, please try again!\n");
      }
      
      return 0;
   }
}

/**
 * Boucle principal d'un processus dédié.
 *
 * Lit les commandes envoyées sur le pipe de commande
 * et les exécute.
 **/
static inline void dedicated_process(const dedicated_process_t* process, database_t* db) {
   char query[QUERY_MAX_SIZE];
   query_result_t result;
   read_state_t read_state;
   unsigned i;
   
   setup_dedicated_interrupt_handler();

   i = 0;
   while (!is_quitting_asked()) {
      read_state = read_line(process->fd_command[0], query, QUERY_MAX_SIZE, &i);
      
      switch (read_state) {
         case READ_ERROR:
         case READ_TOO_LARGE:
            return;
         
         case READ_INTERRUPTED:
            if (is_quitting_asked()) {
               return;
            }
            break;
         
         case READ_EOF:
            if (i == 0) // Fin propre
               return;
            
            // EOF rencontré avant un retour à la ligne
            //  --> on ajoute un '\n' à la fin et essaie d'exécuter
            //      la requête.
            else if (i < QUERY_MAX_SIZE - 1)
               query[i] = '\n', query[i + 1] = '\0';
            else
               return;
            
            [[fallthrough]];
               
         case READ_OK:
            query_result_init(&result, query);
            
            if (perform_user_request(process, db, query, &result)) {
               struct timespec now;
               clock_gettime(CLOCK_REALTIME, &now);
               result.end_ns = now.tv_nsec + 1e9 * now.tv_sec;
               log_query(&result);
            }
            
            query_result_free_internals(&result);
            i = 0;
            break;
      }
   }
}

/**
 * Initialiser le processus dédié en tant que processus
 * du type spécifié.
 */
static inline void init_dedicated_process(dedicated_process_t* process, process_type_t type) {
   process->pid = UNSPECIFIED_PROCESS;
   process->type = type;
   process->fd_command[0] = UNINIT_PIPE;
   process->fd_command[1] = UNINIT_PIPE;
   process->fd_transaction[0] = UNINIT_PIPE;
   process->fd_transaction[1] = UNINIT_PIPE;
}

/**
 * Crée les processus dédiés.
 **/
bool create_dedicated_processes(dedicated_process_t* processes, database_t* db) {
   pid_t pid_fils;
   
   // Créer les 4 processus dédiés
   for (int i = 0; i < LAST_PROCESS_TYPE + 1; ++i) {
      init_dedicated_process(&processes[i], static_cast<process_type_t>(i));

      // 2 pipes : une pour transmettre les commandes, une pour indiquer qu'on
      // a reçu la commande transaction
      if (pipe(processes[i].fd_command) < 0 || pipe(processes[i].fd_transaction) < 0) {
         perror("pipe()");
         return 0;
      }

      pid_fils = fork();
      if (pid_fils == 0) { // 0 = read, 1 = write
         processes[i].type = static_cast<process_type_t>(i);
         close_main_process_pipes(processes, i + 1);

         dedicated_process(&processes[i], db);

         close(processes[i].fd_command[0]);
         close(processes[i].fd_transaction[1]);
         db_close(db);
         exit(0);
      } else if (pid_fils > 0) {
         processes[i].pid = pid_fils;
         close(processes[i].fd_command[0]);
         close(processes[i].fd_transaction[1]);
      } else { // Erreur
         perror("fork()");
         return 0;
      }
   }
   
   return 1;
}
