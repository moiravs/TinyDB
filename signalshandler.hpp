/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#ifndef _SIGNALSHANDLER_HPP
#define _SIGNALSHANDLER_HPP
#include "db.hpp"

/**
 * @brief  Setup all the signals for client and for server
 * @param  client: false if setting up for server, true if setting up for client
 */
void setup_principal_interrupt_handler(bool client);

/**
 * @brief  Setup the SIGINT for the server (to close all the clients)
 */
void server_interrupt_handler(void);

/**
 * @brief  Setup the SIGINT for the client, the SIGPIPE and the SIGURS1 for client and server
 */
void principal_interrupt_handler();

#endif
