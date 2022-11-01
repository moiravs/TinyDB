/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Cursus : BA2-INFO
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/

#include "utils.hpp"

#include <unistd.h>
#include <iostream>
#include "query.hpp"
#include "student.hpp"

size_t safe_read(int fd, void *buffer, size_t nbytes)
{
  ssize_t nbytes_read = read(fd, buffer, nbytes);
  if (nbytes_read < 0)
  {
    perror("read error: ");
    exit(1);
  }
  return (size_t)nbytes_read;
}

size_t safe_write(int fd, const void *buffer, size_t nbytes)
{
  ssize_t bytes_written = write(fd, buffer, nbytes);
  if (bytes_written < 0)
  {
    perror("write: ");
    exit(1);
  }
  return (size_t)bytes_written;
}
