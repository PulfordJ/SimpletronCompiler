//============================================================================
// Name        : Simpletron.cpp
// Author      : John Pulford
// Version     :
// Copyright   : GNU
// Description : Hello World in C<<<<, Ansi-style
//============================================================================

#include <iostream>
#include <iomanip>

using namespace std;

int main() {
	const int MEMSIZE = 100;
	cout << "*** Welcome to Simpletron! ***" << endl
		<< "*** " << "Please enter your program one instruction" << " ***" << endl
		<< "*** "<<"(or data word) at a time. I will type the"<<" ***" << endl
		<< "*** "<<"location number and a question mark (?)."<<" ***" << endl
		<< "*** "<<"You then type the word for that location."<<" ***" << endl
		<< "*** "<<"Type the sentinel -99999 to stop entering"<<" ***" << endl
		<< "*** "<<"your program."<<" ***"; // prints !!!Hello World!!!


	// Array to store program inside.
	int memory[MEMSIZE] = {0};

	int action;
	// Read in instructions
	for(int i = 0; i < MEMSIZE; i++) {
		cout << setfill('0') << setw(2) << i << " ? ";
		cin >> action;
		cout << endl;
	}
	return 0;
}
