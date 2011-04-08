/* This file is part of Flick. See ./COPYING for legal details. */
#include <iostream>
#include <sstream>
#include <X11/Xlib.h>
#include "tcp.h"
#include "udp.h"
using namespace std;

/* Literals */
static const char version_string[] =
	"Flick (server) alpha\n"
	"Copyright (C) 2011 Asher Glick, Tor E Hagemann, and Contributors\n"
	"BSD License: 2-clause <http://en.wikipedia.org/wiki/BSD_licenses>\n"
	"This is free software: you are free to change and redistribute it.\n"
	"There is NO WARRANTY, to the extent permitted by law.\n";
static const char help_string[] =
	"USAGE: ./flick [-hv] [-b bind_address] [-p port]\n"
	"Flick is currently in its early development. :-O Crashes? Oh noes!\n"
	"Please report issues: <https://github.com/hagemt/flick/issues>\n";

/* Functions */
bool get_dimensions(istream &, unsigned int &, unsigned int &);

int main(int argc, char ** argv) {
	int opt;
	string addr("localhost");
	port_num_t port = DEFAULT_PORT;

	/* Parse arguments using GNU getopt. (define --help and --version) */
	while ((opt = getopt(argc, argv, "hb:p:v")) != -1) {
		switch (opt) {
		case 'h':
			cout << help_string << endl;
			return 0;
		case 'b':
			addr = optarg;
		/* Allow users to specify an alternate unprivilaged port. */
		case 'p':
			istringstream(optarg) >> port;
			/* Restrict usable ports to the range [1024, 65535]. */
			if (port < 1024 || port > 65535) {
				cerr << "ERROR: Invalid port." << endl;
				return 1;
			}
			break;
		case 'v':
			cout << version_string << endl;
			return 0;
		default:
			return 1;
		}
	}

	Connection server(addr, port, Connection::TCP);
	for (string s; server >> s; cout << s << endl);

	server.start_server(); sleep(60);
	for (string s; server >> s; cout << s << endl);

	Connection client(addr, port, Connection::TCP);
	if (client << addr) {
		client.start_client(); sleep(60);
		for (string s; client >> s; cout << s << endl);
	}

	server.stop_server(); sleep(60);
	for (string s; server >> s; cout << s << endl);


	Display * d = XOpenDisplay(0);		/* Open the default display. */
	Window * r = NULL;			/* Get the universal parent. */
	unsigned int height, width;		/* Allocate height and width. */
	while (get_dimensions(cin, height, width)) {
		if (d) {
			cout << height << "x" << width << endl;
			if (r == NULL) { r = &DefaultRootWindow(d); }
			Window w = XCreateSimpleWindow(d, *r, 0, 0, height, width, 0, 0, 0);
			/* Create a window on Display *d with parent Window r
			 * at (0,0), size 'h' by 'w', place on top, no border,
			 * with black background, and then render it on *d. */
			XMapWindow(d, w);
			XFlush(d); r = &w;
		} else {
			cerr << "ERROR: Unable to open sample window." << endl;
		}
	}

	return 0;
}

bool get_dimensions(istream & in, unsigned int & h, unsigned int & w) {
	string s;
	if (in >> s && s != "quit" && s != "exit") {
		if (s == "spawn") {
			return (in >> h >> w);
		} else {
			return false;
		}
	}
	return false;
}

