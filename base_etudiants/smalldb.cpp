#include "db.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cerrno>
#include <csignal>

#include <unistd.h>

int main(int argc, char const *argv[])
{

  database_t *db = new database_t;
  db->path = argv[1];
  db_load(db, db->path);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0); // Créer le socket
  int opt = 1;
  // Permet la réutilisation du port/de l'adresse
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  // Définit l'adresse et le port d'écoute , réserve le port
  bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  // Commence l'écoute
  listen(server_fd, 3); // maximum 3 connexions en attente
  size_t addrlen = sizeof(address);
  // Ouvre une nouvelle connexion
  int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
  char buffer[1024];
  // Reçoit un message
  read(new_socket, buffer, 1024);
  printf(" Message reçu :%s\n", buffer);
  // Ferme le socket de discussion
  close(new_socket);
  // Ferme le socket d'écoute
  close(server_fd);
  return 0;
}
