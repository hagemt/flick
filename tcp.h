/* This file is part of Flick. See ./COPYING for legal details. */
#ifndef _FLICK_TCP_
#define _FLICK_TCP_
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"

class TCP_Connection : public Connection {
public:
	// TODO type of type?
	TCP_Connection(const std::string & host, port_num_t port_num) :
		Connection(host, port_num) { type = TCP; }
	int open_socket() {
		return sock = socket(addr.sin_family, SOCK_STREAM, 0);
	}
};

#endif
