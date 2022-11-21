#include <cstdio>
#include <csignal>

#include <unistd.h>

#include "sighandler.h"
#include "../db.h"
#include "ipc.h"

static volatile sig_atomic_t should_quit = 0;
static volatile sig_atomic_t asked_saving_db = 0;

static void principal_interrupt_handler(int sig) {
   if (sig == SIGINT) {
      printf("\nCaught CTRL+C signal, exiting Tiny DB.\n");
      close(STDIN_FILENO);
   } else if (sig == SIGUSR1) {
      asked_saving_db = 1;
   } else if (sig == SIGPIPE) {
      printf("\nCaught unexpected SIGPIPE signal, exiting Tiny DB.\n");
      close(STDIN_FILENO);
   }
}

void setup_principal_interrupt_handler(void) {
   signal(SIGINT, principal_interrupt_handler); // Fermer
   signal(SIGPIPE, principal_interrupt_handler); // Fermer
   signal(SIGUSR1, principal_interrupt_handler); // Sauvegarder la BDD
   
   // La fonction read() n'est pas interrompue sur tous les
   // systèmes par défaut quand SIGUSR1 est reçu. Une solution
   // est d'utiliser sigaction() (en cas d'erreur, on se contente
   // du gestionnaire de signal mis en place avec signal()).
   
   struct sigaction action;
   
   action.sa_handler = principal_interrupt_handler;
   action.sa_flags = 0;
   if (sigemptyset(&action.sa_mask) < 0) {
      perror("sigemptyset()");
   } else if (sigaction(SIGUSR1, &action, NULL) < 0) {
      perror("sigaction()");
   }
}

static void dedicated_interrupt_handler(int sig) {
   if (sig == SIGPIPE) {
      // Utilisé pour fermer proprement le processus
      should_quit = 1;
   }
}

void setup_dedicated_interrupt_handler(void) {
   signal(SIGINT, SIG_IGN);
   signal(SIGUSR1, SIG_IGN);
   signal(SIGPIPE, dedicated_interrupt_handler);
}

bool is_quitting_asked(void) {
   return should_quit;
}

bool is_asked_saving_db(void) {
   return asked_saving_db;
}

void reset_asked_saving_db(void) {
   asked_saving_db = 0;
}
