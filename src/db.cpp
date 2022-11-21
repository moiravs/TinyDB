#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "db.h"
#include "student.h"
#include "ipc/ipc.h"

void db_add(database_t *db, student_t s) {
    if (is_db_full(db)) {
        // Cas impossible normalement car la BDD est
        // étendue avant les insert
        fprintf(stderr, "Unexpected logical size >= physical size : insert ignored.\n");
        return;
    }
    
    db->data[*db->lsize] = s;
    *db->lsize += 1;
}

void db_delete(database_t *db, student_t *s) {
    for (int i = *db->lsize - 1; i >= 0; --i) {
        if (student_equals(&db->data[i], s)) {
            memcpy(&db->data[i], &db->data[*db->lsize - 1], sizeof(student_t));
            *db->lsize -= 1;
        }
    }
}

void db_save(database_t *db, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("Could not open the DB file");
        exit(1);
    }
    const size_t lsize = *db->lsize;
    if (fwrite(db->data, sizeof(student_t), lsize, f) < lsize) {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
    printf("%ld students saved.\n", *db->lsize);
}

void db_load(database_t *db, const char *path) {
  struct stat info;

  // Ouvrir le fichier et déterminer sa taille
  int fd_db = open(path, O_RDONLY);

  if (fd_db < 0) {
    perror("Impossible to open the DB");
    exit(1);
  } else if (fstat(fd_db, &info) != 0) {
    perror(path);
    exit(1);
  }

   size_t lsize = info.st_size / sizeof(student_t);
   size_t psize = lsize + 100;

   // Créer un fichier en RAM pour y conserver
   // la BDD. Va permettre d'étendre la BDD sans
   // devoir tout réécrire à chaque fois.
   db->fd_shm = memfd_create("db", 0);
   if (db->fd_shm < 0) {
      perror("memfd_create()");
      exit(1);
   }
   
   if (ftruncate(db->fd_shm, psize * sizeof(student_t)) != 0)
      exit(1);
   
   if (!db_resync(db))
      exit(1);
   
   // Écrire le contenu de la BDD dans la mémoire
   int r, i = 0;
   do {
      r = read(fd_db, &db->data[i], (lsize - i) * sizeof(student_t));
      i += r;
      if (r < 0) {
         perror("Failed to read completely the DB");
         exit(1);
      }
   } while (r != 0 && i < (signed)lsize);
   
   db->lsize = static_cast<size_t*>(mmap(NULL, sizeof(*db->lsize), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0));
   if (db->lsize == NULL) {
      perror("mmap()");
      exit(1);
   }
   *db->lsize = lsize;
   db->path = path;
   
   close(fd_db);
   
   printf("%ld students found in the db.\n", *db->lsize);
}


void db_display(database_t *db) {
   char buffer[512];
   
   printf(" -- psize=%ld, lsize=%ld --\n", db->psize, *db->lsize);
   for (size_t i = 0; i < *db->lsize; ++i) {
      student_to_str(buffer, &db->data[i]);
      printf("[%ld/%ld] %s\n", i + 1, *db->lsize, buffer);
   }
}

void db_close(database_t *db) {
   munmap(db->data, db->psize * sizeof(student_t));
   munmap(db->lsize, sizeof(*db->lsize));
   close(db->fd_shm);
}

void db_munmap(database_t *db) {
   munmap(db->data, db->psize);
   db->data = NULL;
}

bool db_resync(database_t *db) {
   struct stat info;
   
   if (db->data != NULL)
      munmap(db->data, db->psize);
   
   if (fstat(db->fd_shm, &info) != 0) {
      perror("fstat()");
      return 0;
   }
   
   // Allouer de la mémoire partagée
   db->data = static_cast<student_t*>(mmap(NULL, info.st_size, PROT_READ | PROT_WRITE,
                   MAP_SHARED, db->fd_shm, 0));
   
   db->psize = info.st_size / sizeof(student_t);
   
   if (db->data == NULL) {
      perror("mmap()");
      return 0;
   }
   
   return 1;
}

bool is_db_full(database_t *db) {
   return *db->lsize >= db->psize;
}

bool db_extend(database_t *db) {
   munmap(db->data, db->psize);
   db->data = NULL;
   if (ftruncate(db->fd_shm, db->psize * sizeof(student_t) * 2) != 0) {
      perror("ftruncate()");
      return 0;
   }
   
   return 1;
}
