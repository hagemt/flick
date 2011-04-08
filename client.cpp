/* This file is part of Flick. See ./COPYING for legal details. */
/*
 This code is written by Asher Glick and Tor Hagemann
 */
#include <iostream>
#include <string>
#include <vector>
#include "tcp.h" // TCP protocols
#include "udp.h" // UDP protocols
using namespace std;

struct remoteUser;

// Function Prototypes
vector<remoteUser> scanNetwork();

// Main function
int main() {
	// Setup configurations
	// fork udp ping program
	// initilize tcp socket listner

	//  Begin UI
	cout << "Welcome to flick CLI" << endl;
	string input;

	vector<remoteUser> activeUserList;
	vector<remoteUser> connectedUserList;

	while (true) {
		getline(cin, input);
		if (input == "Scan Network") {
			activeUserList = scanNetwork();
		}

		else {
			cout << "Functions that can be used are:" << endl;
			cout << " scan network" << endl;
			//cout << " connect to computer" << endl;
			//cout << " send window" << endl;
			//cout << " take all windows back" << endl;
		}
	}
}

/*
 remote user struct
 */
struct remoteUser {
	string hostname;
	string ipaddress;
};

/* Scan Network *\
|  
 \*/
vector<remoteUser> scanNetwork() {
	// send message out to the broadcast address
	// wait for an ammount of time untill you get messages form the forked server process (on the remote machines
	// populate the remote user list variable with the responces
	// return the remote user list
}
