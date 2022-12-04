#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>

#include "process.h"

#include "../db.h"
#include "../query.h"
#include "../ipc/ipc.h"

/**
 * Fermeture propre du processus principal et des
 * processus dédiés.
 **/
static inline void clean_exit(database_t* db, dedicated_process_t* processes) {
   int status = 0, created_processes = 0;
   
   // Compter combien de processus dédiés ont pu être créés
   // (ils pourraient être < LAST_PROCESS_TYPE+1 en cas d'erreur
   // avec fork() ou pipe()).
   for (int i = 0; i < LAST_PROCESS_TYPE + 1; ++i) {
      if (processes[i].pid != UNSPECIFIED_PROCESS) {
         ++created_processes;
      }
   }
   
   close_main_process_pipes(processes, created_processes);
   
   printf("Waiting for the child processes to terminate...\n");
    
   // Fermer proprement les processus enfants
   for (int i = 0; i < created_processes;) {
      pid_t p = wait(&status);
      if (WIFEXITED(status)) {
         if (WEXITSTATUS(status) != 0) {
            fprintf(stderr, "A dedicated process [pid: %d] exited abnormally (return status: %d).\n", p, WEXITSTATUS(status));
         }
         ++i;
      }
   }
   
   printf("Comitting database changes to the disk...\n");
   db_save(db, db->path);
   printf("Done.\n");
   
   db_close(db);
}

/**
 * Étend proprement la mémoire partagée de la base de données
 * pour tous les processus.
 **/
static inline void extend_db_for_all_processes(database_t* db, const dedicated_process_t* processes) {
   // 1. Signaler de munmap la mémoire partagée aux processus dédiés
   send_all_command("munmap\n", processes, LAST_PROCESS_TYPE + 1);
   
   // 2. Attendre qu'ils aient bien effectué le munmap
   wait_end_sync(processes); 

   // 3. Étendre la BDD et la resynchroniser dans le processus principal
   if (!db_extend(db) || !db_resync(db)) {
      close(STDIN_FILENO); // Forcer clean exit
      return;
   }
   
   // 4. Dire aux processus de resynchroniser leur BDD
   send_all_command("resync\n", processes, LAST_PROCESS_TYPE + 1);
}

/**
 * Effectue une transaction et bloque tant que les
 * processus dédiés ne l'ont pas complétée.
 *
 * Met à jour *remaining_before_expansion.
 **/
static inline bool generate_transaction(database_t* db, const dedicated_process_t processes[], int* remaining_before_expansion) {
   // On demande une transactions à tous les processus
   if (!send_all_command("transaction\n", processes, LAST_PROCESS_TYPE + 1))
      return 0;
   
   wait_end_sync(processes);
   
   // Vu que tous les processus ont fini d'exécutés les
   // requêtes qui leur étaient assignées, on est certain
   // que la taille logique réelle de la BDD est bien
   // *db->lsize pour tous les processus.
   *remaining_before_expansion = db->psize - *db->lsize;
   
   return 1;
}

/**
 * Répartit les requêtes sur base de leur type.
 * Gère aussi l'extension de la mémoire partagée.
 **/
static inline bool dispatch_user_request(database_t* db, const dedicated_process_t processes[], char* buffer, unsigned len, int* remaining_before_expansion) {
   if (strncmp("select ", buffer, sizeof("select ") - 1) == 0) {
      return send_command(&processes[PROCESS_SELECT], buffer, len);
   } else if (strncmp("update ", buffer, sizeof("update ") - 1) == 0) {
      return send_command(&processes[PROCESS_UPDATE], buffer, len);
   } else if (strncmp("insert ", buffer, sizeof("insert ") - 1) == 0) {
      --*remaining_before_expansion;
      if (*remaining_before_expansion <= 0) {
         // L'expansion peut parfois se faire alors que ce
         // n'était pas réellement nécessaire (s'il y a eu des
         // delete ou des insert mal écrits). Cependant, ceci
         // nous évite des synchronisations plus compliquées.
         extend_db_for_all_processes(db, processes);
         *remaining_before_expansion = db->psize - *db->lsize;
      }
      
      return send_command(&processes[PROCESS_INSERT], buffer, len);
   } else if (strncmp("delete ", buffer, sizeof("delete ") - 1) == 0) {
      return send_command(&processes[PROCESS_DELETE], buffer, len);
   } else if (strcmp("transaction\n", buffer) == 0) {
      return generate_transaction(db, processes, remaining_before_expansion);
   } else if (buffer[0] != '\n') {
      printf("Invalid query name.\n");
   }
   
   return 1;
}

/**
 * Synchronise tous les processus dédiés (= transaction)
 * puis sauvegarde la BDD sur le disque.
 *
 * Met à jour *remaining_before_expansion et réinitialise
 * les demandes de sauvegarde de la BDD.
 **/
static inline void sync_save_db(database_t* db, const dedicated_process_t processes[], int* remaining_before_expansion) {
   reset_asked_saving_db();
   if (!generate_transaction(db, processes, remaining_before_expansion)) {
      fprintf(stderr, "Failed to synchronise the processes before saving the file. The DB might be corrupted.\n");
   }
   
   printf("Caught SIGUSR1, comitting database changes to the disk...\n");
   db_save(db, db->path);
}

/**
 * Lit les commandes transmises sur stdin et les répartit
 * aux processus dédiés adéquats.
 **/
static inline void prompt(database_t* db, const dedicated_process_t processes[]) {
   char buffer[QUERY_MAX_SIZE];
   read_state_t read_state;
   unsigned i = 0;
   int remaining_before_expansion = db->psize - *db->lsize;
   
   printf("Welcome to the tiny database! Please enter your command!\n");

   while (1) {
      printf("> ");
      fflush(stdout);
      read_state = read_line(STDIN_FILENO, buffer, QUERY_MAX_SIZE, &i);

      switch (read_state) {
         case READ_ERROR: // Fin propre
         case READ_TOO_LARGE:
            return;
         
         case READ_INTERRUPTED:
            // On a reçu un signal en attendant une entrée
            //  • SIGINT : rien à faire, stdin a déjà été fermé par
            //    le gestionaire de signaux.
            //  • SIGUSR1 : on doit sauvegarder la BDD
            if (is_asked_saving_db()) {
               sync_save_db(db, processes, &remaining_before_expansion);
            }
            break;
         
         case READ_EOF:
            if (i == 0) // Fin propre
               return;
            
            // EOF rencontré avant un retour à la ligne
            //  --> on ajoute un '\n' à la fin et essaie d'exécuter
            //      la requête.
            else if (i < QUERY_MAX_SIZE - 1)
               buffer[i] = '\n', buffer[i + 1] = '\0';
            else
               return;
            
            [[fallthrough]];
         
         case READ_OK:
            if (!dispatch_user_request(db, processes, buffer, i, &remaining_before_expansion)) {
               perror("Sending a command failed");
               return;
            }
            
            i = 0;
            break;
      }
      
      if (is_asked_saving_db()) {
         sync_save_db(db, processes, &remaining_before_expansion);
      }
   }
}

void main_process(database_t* db) {
   dedicated_process_t processes[LAST_PROCESS_TYPE + 1];
   
   if (create_dedicated_processes(processes, db)) {
      setup_principal_interrupt_handler();
      prompt(db, processes);
   }
   
   clean_exit(db, processes);
}
