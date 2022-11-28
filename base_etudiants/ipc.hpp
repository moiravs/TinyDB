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

#ifndef _IPC_HPP
#define _IPC_HPP


#include "signalshandler.hpp" // Signaux de l'utilisateur

#endif
