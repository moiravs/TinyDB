/**
 * Gestion des communications inter-processus via
 * pipes.
 **/

#ifndef _PIPE_H
#define _PIPE_H

#include <signal.h>

#include "../process/process.h"

enum { TRANSACTION_OK = '1', TRANSACTION_TEMPORARY_IGNORE = '2' };

typedef enum {
   READ_INTERRUPTED,
   READ_EOF,
   READ_ERROR,
   READ_TOO_LARGE,
   READ_OK
} read_state_t;

/**
 * Écrit dans buffer (à partir de l'indice *i) la première
 * ligne du flux référencé par fd. Si aucun '\n' n'est lu
 * après (size - *i) bytes, la fonction s'arrête aussi et
 * retourne READ_TOO_LARGE.
 *
 * @return
 *   • READ_OK : si le caractère '\n' a été rencontré avant d'
 *     atteindre la taille limite size.
 *   • READ_TOO_LARGE : si la ligne était plus longue que
 *     (size - *i) bytes.
 *   • READ_ERROR : si une erreur de lecture s'est produite.
 *   • READ_EOF : si la fin de fichier a été détectée.
 *   • READ_INTERRUPTED : si une interruption est survenue
 *     durant la lecture. *i est augmenté du nombre de
 *     caractères lus avant l'interruption.
 **/
read_state_t read_line(int fd, char* buffer, size_t size, unsigned *i);

/**
 * Ferme tous les pipes inutiles des n premiers processus
 * dédiés.
 **/
void close_main_process_pipes(const dedicated_process_t processes[], unsigned n);

bool send_command(const dedicated_process_t* destination, const char* message, size_t size);

/**
 * Broadcast la commande cmd aux n premiers processus
 * de processes sur le pipe dédié aux commandes.
 *
 * @return 0 en cas d'erreur, 1 en cas de succès
 **/
bool send_all_command(const char* cmd, const dedicated_process_t processes[], unsigned n);


//// Synchronisation générale ////


/**
 * Permet à un processus dédié d'indiqué qu'il a lu une
 * commande de synchronisation (transaction, commit ou
 * resync).
 **/
void notify_sync_done(const dedicated_process_t* process);

/**
 * Premet au processus principal d'attendre que tous les
 * processus dédiés aient fini leurs commandes avant la
 * transaction.
 **/
void wait_end_sync(const dedicated_process_t processes[]);

#endif
