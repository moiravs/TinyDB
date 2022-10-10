#include "query.h"

#include <time.h>

void query_result_init(query_result_t* result, const char* query) {
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->status = QUERY_SUCCESS;
  // Votre code ici
}
