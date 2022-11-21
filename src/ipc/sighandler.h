#ifndef _SIGHANDLER_H
#define _SIGHANDLER_H

/**
 * Met en place le gestionnaire de signaux pour
 * le processus principal :
 *   • SIGINT = fermeture propre 
 *   • SIGUSR1 = sauvegarder la BDD
 *   • SIGPIPE = fermeture propre
 */
void setup_principal_interrupt_handler(void);

/**
 * Met en place le gestionnaire de signaux pour
 * un processus dédié :
 *   • SIGINT = ignoré 
 *   • SIGUSR1 = ignoré
 *   • SIGPIPE = fermeture propre
 */
void setup_dedicated_interrupt_handler(void);

/**
 * Permet à un processus dédié de savoir qu'il
 * a reçu un SIGPIPE durant l'une de ses écritures.
 **/
bool is_quitting_asked(void);

/**
 * SIGUSR1 a été envoyé pour demander de sauvegarder
 * la base de données actuelle dans un fichier.
 **/
bool is_asked_saving_db(void);

/**
 * Remet à faux la demande de sauvegarde de la base
 * de données dans un fichier.
 **/
void reset_asked_saving_db(void);

#endif
