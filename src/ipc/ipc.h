/**
 * Gestion des communications inter-processus (IPC).
 *
 * Deux types d'IPC sont utilisés :
 *   • pipes : pour les communications entre le
 *     processus principal et ceux dédiés ;
 *
 *   • signaux : envoyés uniquement par l'utilisateur
 *     (sauf SIGPIPE).
 **/

#ifndef _IPC_H
#define _IPC_H

#include "pipe.h" // Communications internes

#include "sighandler.h" // Signaux de l'utilisateur

#endif
