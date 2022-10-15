#include "utils.h"

#include <unistd.h>

#include "query.h"
#include "student.h"

size_t safe_read(int fd, void* buffer, size_t nbytes) {
  ssize_t nbytes_read = read(fd, buffer, nbytes);
  if (nbytes_read < 0) {
    perror("read error: ");
    exit(1);
  }
  return (size_t)nbytes_read;
}

size_t safe_write(int fd, const void* buffer, size_t nbytes) {
  ssize_t bytes_written = write(fd, buffer, nbytes);
  if (bytes_written < 0) {
    perror("write: ");
    exit(1);
  }
  return (size_t)bytes_written;
}

void log_query(query_result_t* result) {
  char buffer[512];
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
      for (int i = 0; i < result->lsize; i++) {
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
