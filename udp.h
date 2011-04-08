/* This file is part of Flick. See ./COPYING for legal details. */
#ifndef _FLICK_UDP_
#define _FLICK_UDP_
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"

class UDP_Connection : public Connection {
public:
	UDP_Connection(const std::string & host, port_num_t port_num) :
		Connection(host, port_num) { type = UDP; }
	int open_socket() {
		return sock = socket(addr.sin_family, SOCK_DGRAM, 0);
	}
};

#endif
