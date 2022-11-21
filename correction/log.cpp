#include <stdlib.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstring>

#include "query.h"
#include "student.h"

/**
 * @brief Create a log directory if it does not yet exists.
 */
void create_log_directory() {
  struct stat sb;
  if (!stat("logs", &sb) == 0 || !S_ISDIR(sb.st_mode)) {
    if (mkdir("logs", 0700) != 0) {
      perror("mkdir");
      exit(0);
    }
  }
}

void log_query(query_result_t* result) {
  create_log_directory();
  char buffer[512 + QUERY_MAX_SIZE];
  if (result->status == QUERY_SUCCESS) {
    char filename[512];
    char type[256];
    strcpy(type, result->query);
    type[6] = '\0';
    sprintf(filename, "logs/%ld-%s.txt", result->start_ns, type);
    printf("%s\n", filename);
    FILE* f = fopen(filename, "w");
    float duration = (float)(result->end_ns - result->start_ns) / 1.0e6;
    sprintf(buffer, "Query \"%s\" completed in %fms with %ld results.\n", result->query, duration, result->lsize);
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    if (result->lsize > 0) {
      for (size_t i = 0; i < result->lsize; i++) {
        student_to_str(buffer, &result->students[i]);
        fwrite(buffer, sizeof(char), strlen(buffer), f);
        fwrite("\n", sizeof(char), 1, f);
      }
    }
    fclose(f);
  } else if (result->status == UNRECOGNISED_FIELD) {
    fprintf(stderr, "Unrecognized field in query %s\n", result->query);
  }
}
