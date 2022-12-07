#ifndef _SIGNALSHANDLER_HPP
#define _SIGNALSHANDLER_HPP
#include "db.hpp"

/**
 * Met en place le gestionnaire de signaux pour
 * le processus principal :
 *   • SIGINT = fermeture propre 
 *   • SIGUSR1 = sauvegarder la BDD
 *   • SIGPIPE = fermeture propre
 */
void setup_principal_interrupt_handler(bool client);

/**
 * Met en place le gestionnaire de signaux côté serveur :
 *  • SIGINT = sauvegarder la BDD
 *  • 
**/
void server_interrupt_handler(void);
void principal_interrupt_handler();



#endif
