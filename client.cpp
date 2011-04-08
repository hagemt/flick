/* This file is part of Flick. See ./COPYING for legal details. */
#include <iostream>
#include <string>
#include "tcp.h"
#include "udp.h"
using namespace std;

int main() {
	string input;
	cout << "Welcome to flick CLI" << endl;
	while (cin >> input) {
		cout << input;
	}
}
