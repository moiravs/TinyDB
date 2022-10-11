#include "query.h"

#include <time.h>

void query_result_init(query_result_t *result, const char *query)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->status = QUERY_SUCCESS;
  // TODO select < champ >= <valeur>.Cette requête renvoie la liste des étudiants qui correspondent à l’unique filtre < champ >= <valeur>.
  // TODO insert<fname><lname><section><birthdate>.Cette requête insère un nouvel étudiant dans la base de données en vérifiant que l’ID n’existe pas déjà.Si l’id existe déjà,l’insertion échoue.
  // TODO delete < champ >= <valeur>.Cette requête supprime tous les étudiants qui correspondent au filtre donné.
  // TODO update < filtre >= <valeur> set < champ_modifie >= <valeur_modifiee>.Cette requête modifie tous les étudiants correspondant au filtre < filtre >= <valeur>, en donnant la valeur<valeur_modifiee> au champ<champ_modifie>.
  // TODO transaction.Ce mot - clef déclare le début d’une nouvelle transaction.Plus de détails sont donnés dans la Section
  // Votre code ici*/
}
