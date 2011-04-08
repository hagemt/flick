/* This file is part of Flick. See ./COPYING for legal details. */
#ifndef _FLICK_CONNECTION_
#define _FLICK_CONNECTION_
#include <netinet/in.h>
#include <iostream>
#include <string>
#include "types.h"

class Connection {
protected:
	int sock; enum Type { TCP, UDP } type;
	sockaddr_in addr; port_num_t port;
public:
	Connection(const std::string & host, port_num_t port_num) {
		// TODO investigate IPv6 hosts? datagram sockets?
		// struct hostent * host_entry = gethostbyname(host.c_str());
		// memcpy((char *)&addr.sin_addr, (char *)host_entry->h_addr, host_entry->h_length);
		addr.sin_family = PF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port = port_num);
	}
	virtual int open_socket() = 0;
	int get_socket() const { return sock; }
	socklen_t get_address_size() const { return sizeof(addr); }
	int get_port_number() const { return port; }
	const sockaddr * get_address() const {
		return (const sockaddr *)(&addr);
	}
	friend std::ostream & operator<<(std::ostream &, Connection &);
};

std::ostream & operator<<(std::ostream & ostr, Connection & c) {
	if (c.open_socket() < 0) {
		return ostr << "ERROR: could not create socket" << std::endl;
	}
	if (bind(c.get_socket(), c.get_address(), c.get_address_size())) {
		return ostr << "ERROR: could not bind to port " << c.get_port_number() << std::endl;
	}
	if (listen(c.get_socket(), MAX_CLIENTS)) {
		return ostr << "ERROR: could not listen on port " << c.get_port_number() << std::endl;
	}
	return ostr << "Socket established!" << std::endl;
};

#endif
