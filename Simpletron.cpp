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
	const int ENDLOOP = -99999;
	cout << "*** Welcome to Simpletron! ***" << endl << "*** "
			<< "Please enter your program one instruction" << " ***" << endl
			<< "*** " << "(or data word) at a time. I will type the" << " ***"
			<< endl << "*** " << "location number and a question mark (?)."
			<< " ***" << endl << "*** "
			<< "You then type the word for that location." << " ***" << endl
			<< "*** " << "Type the sentinel -99999 to stop entering" << " ***"
			<< endl << "*** " << "your program." << " ***" << endl << endl; // prints !!!Hello World!!!

	// Array to store program inside.
	int memory[MEMSIZE] = { 0 };

	int action = 0;
	int accumulator = 0;
	int instructionCounter = 0;

	// Read in instructions
	while (instructionCounter < MEMSIZE) {
		cout << setfill('0') << setw(2) << instructionCounter << " ? ";
		cin >> action;
		cout << endl;
		if (action == ENDLOOP)
			break;
		memory[instructionCounter] = action;
		instructionCounter++;
	}

	cout << setfill(' ') << "*** Program loading completed ***" << endl
			<< "*** Program execution begins ***" << endl;

	//Set operation code constants.
	const int READ = 10;
	const int WRITE = 11;

	const int LOAD = 20;
	const int STORE = 21;

	const int ADD = 30;
	const int SUBTRACT = 31;

	const int DIVIDE = 32;
	const int MULTIPLY = 33;

	const int BRANCH = 40;
	const int BRANCHNEG = 41;
	const int BRANCHZERO = 42;
	const int HALT = 43;

	//Initialise special registers.
	int instructionRegister = 0, operationCode = 0, operand = 0;

	instructionCounter = 0;
	// Execute typed program.
	while (instructionCounter < MEMSIZE) {
		instructionRegister = memory[instructionCounter];
		operationCode = instructionRegister / 100;
		operand = instructionRegister % 100;
		switch (operationCode) {

		// Essentially the part of the Simpletron that interprets the SML programming language.
		case READ:
			cout << "?";
			cin >> memory[operand];
			cout << endl;
			break;
		case LOAD:
			accumulator = memory[operand];
			break;
		case ADD:
			accumulator += memory[operand];
			break;
		case BRANCH:
			instructionCounter = operand;
			break;
		case BRANCHNEG:
			if (accumulator < 0) instructionCounter = operand;
			break;
		case BRANCHZERO:
			if (accumulator == 0) instructionCounter = operand;
			break;
		case HALT:
			cout << endl << "*** Simpletron execution terminated ***" << endl
					<< "REGISTERS:" << endl;

			// List registers.
			const int REGWIDTH = 20;
			cout << setw(REGWIDTH) << left << "accumulator" << showpos << setw(5) << setfill('0') << accumulator << endl
					<< setfill(' ') << setw(REGWIDTH + 3) << left << "instructionCounter" << noshowpos << setw(2) << setfill('0') << instructionCounter << endl
					<< setfill(' ') << setw(REGWIDTH) << left << "instructionRegister" << showpos << setw(5) << setfill('0') << instructionRegister << endl
					<< setfill(' ') << setw(REGWIDTH + 2) << left << "operationCode" << noshowpos << setw(3) << setfill('0') << operationCode << endl
					<< setfill(' ') << setw(REGWIDTH + 3) << left << "operand" << setw(2) << setfill('0') << operand << endl
					<< endl << setfill(' ')
					<< "MEMORY:" << endl << "  ";

			// print column names for memory dump.
			for (int i = 0; i < 10; i++) {
				cout << " " << setw(5) << right << i;
			}

			cout << endl;

			for (int i = 0; i < MEMSIZE; i += 10) {
				//Generate tens row list.
				cout << setfill(' ') << setw(2) << i << showpos ;
				for (int j = 0; j < 10; j++) {
					cout << " " << setw(5) << setfill('0') << internal << memory[i+j];
				}
				cout << noshowpos << endl;
			}

			break;
		}

		instructionCounter++;
	}
	return 0;
}
