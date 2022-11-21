/**
 * L'application est répartie en 1 processus principal (le processus initial) et
 * 4 processus dédiés dont les informations sont contenues dans des données de
 * type dedicated_process_t.
 *
 * Au niveau des communications :
 *
 *  • stdin : lecture des commandes (select, insert, delete, update, transaction),
 *    uniquement lu par le processus principal. Fermer stdin ferme proprement le
 *    processus. La fermeture de stdin est utilisée pour forcer une fermeture
 *    propre du processus principal
 *
 *  • pipes anonymes :
 *     • 4 pipes de commandes (processus principal -> dédié) pour les commandes
 *       de l'utilisateur mais aussi celles de synchronisation (munmap et resync)
 *     • 4 pipes de synchronisation (processus dédié -> principal). Les pipes
 *       de synchronisation permettent :
 *         1. d'attendre que tous les processus dédiés soient arrivé à la fin
 *            d'une transaction ;
 *         2. rafraîchir la mémoire partagée pour l'étendre.
 *
 *  • signaux :
 *      • SIGINT : fermeture propre (principal) -- ignoré (dédié) 
 *      • SIGUSR1 : écrire les changements dans le fichier (principal) --
 *        ignoré (dédié)
 *      • SIGPIPE : fermeture propre (principal et dédié).
 *
 *  • mémoire partagée :
 *      • db.lsize : la taille logique est conservée en mémoire partagée
 *        anonyme. La taille physique est augmentée localement lors de
 *        l'extension de la mémoire.
 *      • db.fd_shm : référence un fichier anonyme conservé uniquement en RAM
 *        où le contenu de la base de données est conservé. Il est utilisé
 *        comme paramètre de mmap() pour que tous les processus référencent la
 *        même mémoire partagée (permet aussi d'agrandir la mémoire partagée
 *        puis de laisser les processus dédiés y réaccéder).
 *      • db.data : données de la base de données (issu d'un mmap() sur db.fd_shm).
 *
 * Commande
 * --------
 *   Les commandes sont transmises telles quelles sur le pipe de commande du
 *   processus dédié associé (y compris le '\n' final, servant de délimiteur entre
 *   deux commandes écrites sur le pipe).
 *
 * Transaction
 * -----------
 *   Les transactions sont initiées via le pipe de commande en écrivant
 *   "transaction\n". Chaque processus dédié reçoit cette commande. Le processus
 *   principal se met ensuite en attente jusqu'à ce que tous les processus
 *   dédiés aient écrit 1 octet sur le pipe de synchronisation.
 *
 * Extension de la mémoire partagée
 * --------------------------------
 *   La mémoire partagée est étendue en plusieurs étapes. Pour simplifier
 *   les mécanismes de synchronisation entre les processus, l'extension de
 *   la mémoire est réalisée sur demande du processus principal.
 *
 *   Une estimation de la différence entre la taille physique et la taille 
 *   logique de la BDD est utilisée pour détecter quand il pourrait y avoir 
 *   besoin d'agrandir la taille physique. Cette estimation est conservée dans 
 *   remaining_before_expansion mais elle peut être différente de la taille 
 *   logique (ex. : si un insert a échoué, un delete a été effectué ou si tous 
 *   les inserts n'ont pas encore été traités par le processus dédié).
 *
 *   La valeur remaining_before_expansion est mise à jour lorsque les
 *   processus se synchronisent (via une transaction ou une demande de munmap,
 *   cf ci-dessous pour le munmap).
 *
 *   Lorsque le processus principal détecte qu'une extension de la mémoire est
 *   nécessaire, l'extension s'effectue ainsi :
 *
 *     1. Envoi d'une demande d'munmap (via la commande "munmap\n" sur le pipe
 *        de commande de chaque processus dédié).
 *
 *        Sens : processus principal --[pipe]--> processus dédiés
 *
 *     2. munmap() dans tous les processus dédiés à la réception de "munmap\n".
 *
 *     3. Les processus dédiés indiquent qu'ils ont munmap() au processus
 *        principal (écrivent sur le pipe de synchronisation).
 *
 *        Sens : processus dédiés --[pipe]--> processus principal
 *
 *     4. munmap() dans le processus principal, extension via ftruncate() du
 *        fichier db.fd_shm et mmap() sur db.fd_shm.
 *
 *     5. mmap() réeffectué par les autres processus dédiés via la commande
 *        "resync\n". Le mmap() est fait sur db.fd_shm pour que tous les
 *        processus puissent accéder à la même mémoire partagée.
 *
 *        Sens : principal --[pipe]--> dédiés
 *
 *   Il est important de noter que db.fd_shm reste ouvert en continu et sert de
 *   fichier commun à tous les processus d'une instance, permettant de toujours
 *   ouvrir la même mémoire partagée avec mmap().
 *
 *   Ce fichier est créé à l'aide de memfd_create() qui a l'avantage de créer un
 *   fichier anonyme (pas de risques que plusieurs instances se partagent la même
 *   mémoire partagée ainsi) et est en RAM uniquement (ce qui la rend plus rapide
 *   d'accès). La fonction shm_open() est aussi disponible en tant qu'alternative
 *   intéressante à memfd_create() mais il faut gérer les conflits de noms car la
 *   mémoire partagée est alors nommée.
 **/

#include "process/process.h"
#include "db.h"

int main(int argc, char const *argv[]) {
   database_t db;
   const char *database_file;
   
   if (argc > 1) {
      database_file = argv[1];
   } else {
      fprintf(stderr, "Mandatory parameter (db file) missing.\n");
      return 1;
   }
   
   db_load(&db, database_file);
   main_process(&db);
   
   return 0;
}
