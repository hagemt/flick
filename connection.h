/* This file is part of Flick. See ./COPYING for legal details. */
#ifndef _FLICK_CONNECTION_
#define _FLICK_CONNECTION_
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include "types.h"

class Connection {
	/* Representation of a TCP/UDP client/server connection */
	int sock; sockaddr_in addr; port_num_t port;
	/* Used to manage the server's thread-pool */
	struct thread_t { pthread_t tid; thread_t * next; };
	thread_t * thread_pool, * current;
	pthread_mutex_t acceptMutex, bufferMutex;
	/* State-handlers */
	bool server_running; char * buffer;

	/* Used for I/O between clients and servers */
	std::stringstream io;

	/* Used to redirect communications back to the proper */
	static void * accept_connection(void * socket) {
		Connection * c = reinterpret_cast<Connection *>(socket);
		// TODO switch (type)
		c->handle_tcp_client();
		return socket;
	}

	void handle_tcp_client() {
		sockaddr_in client; socklen_t client_length;
		do {
			/* Allow each thread to handle a new connection */
			pthread_mutex_lock(&acceptMutex);
			int comm = accept(sock, (sockaddr *)(&client), &client_length);
			pthread_mutex_unlock(&acceptMutex);
			io << "Connection detected!" << std::endl;
			for (ssize_t n;;) {
				pthread_mutex_lock(&bufferMutex);
				if ((n = recv(comm, buffer, BUFFER_SIZE - 1, 0)) > 0) {
					buffer[n] = '\0';
					io << buffer << std::endl;
				} else {
					pthread_mutex_unlock(&bufferMutex);
					break;
				}
				pthread_mutex_unlock(&bufferMutex);
			}
			close(comm);
		} while (server_running);
	}
public:
	/* Identifying type for underlying socket communications */
	enum Type { TCP = SOCK_STREAM, UDP = SOCK_DGRAM };
	/* Create a connection of the specified type (may become client or server) */
	Connection(const std::string & host, port_num_t port_num, Connection::Type type) {
		/* Assign default values */
		sock = -1; port = port_num;
		server_running = false;
		thread_pool = current = NULL;
		buffer = new char[BUFFER_SIZE];
		pthread_mutex_init(&acceptMutex, NULL);
		pthread_mutex_init(&bufferMutex, NULL);
		/* Get address info */
		addrinfo hints, * info;
		memset(&hints, 0, sizeof(addrinfo));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = type;
		// TODO port_num -> correct cstring?
		if (!getaddrinfo(host.c_str(), "21384", &hints, &info)) {
			/* Get the first known address, TODO screen IPv6? */
			for (addrinfo * i = info; i != NULL; i = i->ai_next) {
				if ((sock = socket(i->ai_family, i->ai_socktype, i->ai_protocol)) > 0) {
					break;
				}
			}
			if (sock > 0) {
				/* Copy over the relevant data */
				memcpy(&addr.sin_addr, &info->ai_addr, info->ai_addrlen);
				addr.sin_family = info->ai_family;
				addr.sin_port = htons(port);
			} else {
				io << "ERROR: failed to create socket" << std::endl;
			}
		}
		/* Cleanup */
		freeaddrinfo(info);
	}

	virtual ~Connection() {
		if (server_running) { stop_server(); }
		if (sock >= 0) { close(sock); }
		pthread_mutex_destroy(&acceptMutex);
		pthread_mutex_destroy(&bufferMutex);
		delete[] buffer;
	}

	int start_client() {
		int comm = connect(sock, (const sockaddr *)(&addr), sizeof(addr));
		io << "Starting client..." << std::endl;

		for (ssize_t n;;) {
			pthread_mutex_lock(&bufferMutex);
			if ((n = recv(comm, buffer, BUFFER_SIZE - 1, 0)) > 0) {
				buffer[n] = '\0';
				io << buffer << std::endl;
			} else {
				pthread_mutex_unlock(&bufferMutex);
				break;
			}
			pthread_mutex_unlock(&bufferMutex);
		}
		return sock;
	}

	int start_server() {
		if (bind(sock, (const sockaddr *)(&addr), sizeof(addr))) {
			io << "ERROR: failed bind to port " << port << std::endl;
		}
		if (listen(sock, MAX_CLIENTS)) {
			io << "ERROR: failed to listen on port " << port << std::endl;
		}
		io << "Starting server on port " << port << std::endl;
		for (int i = 0; i < MAX_CLIENTS; ++i) {
			/* Advance entries in the pool */
			if (current) { current = current->next = new thread_t(); }
			else { thread_pool = current = new thread_t(); }
			current->next = NULL;
			/* Call handler to accept each connection */
			if (pthread_create(&current->tid, NULL, &Connection::accept_connection, this)) {
				io << "Thread creation failed" << std::endl;
			} else {
				io << "Thread [" << current->tid << "] created" << std::endl;
			}
		}
		return sock;
	}

	int stop_server() {
		if (server_running) {
			for (current = thread_pool; current; current = current->next) {
				if (current->tid && pthread_detach(current->tid)) {
					io << "Thread [" << current->tid << "] failed to detach" << std::endl;
				} else {
					io << "Thread [" << current->tid << "] detached" << std::endl;
				}
			}
			if (sock >= 0) {
				if (shutdown(sock, SHUT_RDWR)) {
					io << "Problem shutting down socket" << std::endl;
				}
				server_running = false;
			}
			if (thread_pool) {
				while ((current = thread_pool->next)) {
					thread_pool->next = current->next;
					delete current;
				}
				delete thread_pool;
				thread_pool = current = NULL;
			}
		}
		return sock;
	}

	bool operator>>(std::string & s) { return io >> s; }
	bool operator<<(const std::string & s) { return io << s; }
};

#endif
