/*
SmallDb : Database following the client-server model. The server processes the requests sent by the client.
Section : BA-INFO
Authors : Milan SKALERIC, Moïra VANDERSLAGMOLEN, Andrius EZERSKIS
Date : 07/12/2022
*/

#ifndef _ERRORCODES_HPP
#define _ERRORCODES_HPP

const int NO_ERROR = 0b00000;
const int USAGE_ERROR = 0b00001;
const int MEMORY_ERROR = 0b00010;
const int FILE_ERROR = 0b00100;
const int NETWORK_ERROR = 0b01000;
const int SYSTEM_ERROR = 0b10000;
const int SOCKET_ERROR = 0b110000;
const int LISTEN_ERROR = 0b111000;

const int DISCONNECTED = -1;
const int UNEXPECTED_DISCONNECT = -2;

// NOTE: Should have use enum classes

#endif // _ERRORCODES_H
