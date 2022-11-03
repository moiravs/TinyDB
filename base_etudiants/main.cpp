/*
Projet 1 du cours *systèmes d'exploitation*, INFO-F201
Auteurs : Moïra Vanderslagmolen, Andrius Ežerskis, Hasan Yildirim
Description du projet *TinyDB* :
  Base de données formée à partir d'un fichier .bin et reprenant l'identité des étudiants, ainsi que leur cursus
*/
#include "process.hpp"
#include "db.hpp"
#include "query.hpp"
#include "parsing.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>

database_t *db;
const char *db_path;
int fd_insert[2], fd_select[2], fd_update[2], fd_delete[2], fd_response[2]; // children write in the fd response pipe when they finished their query


int main(int argc, char const *argv[])
{

  db_path = argv[1];

  db = (database_t *)mmap(NULL, (sizeof(database_t)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  db->db_init();
  std::cout << "Loading your tiny tiny database..." << std::endl;
  db->db_load(db_path);
  std::cout << "Done !" << std::endl;
  // fd : file descriptor (fd0 : standard input (stdin), fd_insert : standard output (stdout), fd2 : standard error (stderr))
  // pipe(fd) : creates both the reading and writing ends of the pipe
  // fd[0] : standard input, fd[1] : standard output
  pipe(fd_insert);
  pipe(fd_select);
  pipe(fd_update);
  pipe(fd_delete);
  pipe(fd_response);

  // no blocking pipe for response
  fcntl(fd_response[0], F_SETFL, fcntl(fd_response[0], F_GETFL) | O_NONBLOCK);

  signal(SIGINT, signal_handling);  // handles the signal Ctrl + C and terminates program
  signal(SIGUSR1, signal_handling); // handles abnormal program termination
  main_process();

  close_application(false);
}
