/*
 * Simpletron.cpp
 *
 *  Created on: 24 Feb 2013
 *      Author: john
 */

#include "Simpletron.h"
#include <iostream>
#include <iomanip>

using namespace std;

const char *Simpletron::FATAL_ERROR_MESSAGE =
		"*** Simpletron execution abnormally terminated ***";

Simpletron::Simpletron() :
		//Initialize special registers.
		instructionRegister(0), operationCode(0), operand(0), action(0), accumulator(
				0), instructionCounter(0) {
	for (int i = 0; i < MEMSIZE; i++) {
		memory[i] = 0;
	}

}

void Simpletron::printError() {
	cout << FATAL_ERROR_MESSAGE;
	printState();
	instructionCounter = MEMSIZE - 1;
}

void Simpletron::printState() {
	cout << "REGISTERS:" << endl;

	// List registers.
	const int REGWIDTH = 20;
	cout << setw(REGWIDTH) << left << "accumulator" << showpos << setw(5) << setfill('0') << internal << accumulator << endl
			<< setfill(' ') << setw(REGWIDTH + 3) << left << "instructionCounter" << noshowpos << setw(2) << setfill('0') << internal << instructionCounter << endl
			<< setfill(' ') << setw(REGWIDTH) << left << "instructionRegister" << showpos << setw(5) << setfill('0') << internal << instructionRegister << endl
			<< setfill(' ') << setw(REGWIDTH + 3) << left << "operationCode" << noshowpos << setw(2) << setfill('0') << internal << operationCode << endl
			<< setfill(' ') << setw(REGWIDTH + 3) << left << "operand" << setw(2) << setfill('0') << internal << operand << endl
			<< endl
			<< setfill(' ') << "MEMORY:" << endl << "  ";

	// print column names for memory dump.
	for (int i = 0; i < 10; i++) {
		cout << " " << setw(5) << right << i;
	}

	cout << endl;

	for (int i = 0; i < MEMSIZE; i += 10) {
		//Generate tens row list.
		cout << setfill(' ') << setw(2) << i << showpos;
		for (int j = 0; j < 10; j++) {
			cout << " " << setw(5) << setfill('0') << internal << memory[i + j];
		}
		cout << noshowpos << endl;
	}
}

void Simpletron::arithResultCheck() {
	if (accumulator < -9999 || accumulator > +9999) {
		cout << endl << "*** Attempt to divide by zero ***" << endl;
		printError();
	}

}

void Simpletron::inputInstructions() {

	cout << "*** Welcome to Simpletron! ***" << endl << "*** "
			<< "Please enter your program one instruction" << " ***" << endl
			<< "*** " << "(or data word) at a time. I will type the" << " ***"
			<< endl << "*** " << "location number and a question mark (?)."
			<< " ***" << endl << "*** "
			<< "You then type the word for that location." << " ***" << endl
			<< "*** " << "Type the sentinel -99999 to stop entering" << " ***"
			<< endl << "*** " << "your program." << " ***" << endl << endl; // prints !!!Hello World!!!

	// Read in instructions
	while (instructionCounter < MEMSIZE) {
		cout << setfill('0') << setw(2) << instructionCounter << " ? ";
		cin >> action;
		cout << endl;

		// Error checking
		while (action != ENDINPUTLOOP && (action < -9999 || action > +9999)) {
			cout << "Invalid instruction, reenter ? ";
			cin >> action;
			cout << endl;
		}

		if (action == ENDINPUTLOOP)
			break;
		memory[instructionCounter] = action;
		instructionCounter++;
	}

	cout << setfill(' ') << "*** Program loading completed ***" << endl;

}

void Simpletron::execute() {
	cout << "*** Program execution begins ***" << endl;

	instructionCounter = 0;
// Execute typed program.
	while (instructionCounter < MEMSIZE) {
		instructionRegister = memory[instructionCounter];
		operationCode = instructionRegister / 100;
		operand = instructionRegister % 100;
		switch (operationCode) {

		// Essentially the part of the Simpletron that interprets the SML programming language.
		case READ:
			cout << "? ";
			cin >> memory[operand];
			cout << endl;
			break;
		case WRITE:
			cout << memory[operand];
			break;

		case LOAD:
			accumulator = memory[operand];
			break;
		case STORE:
			memory[operand] = accumulator;
			break;

			// Handle arithmetic
		case ADD:
			accumulator += memory[operand];
			arithResultCheck();
			break;
		case SUBTRACT:
			accumulator -= memory[operand];
			arithResultCheck();
			break;
		case DIVIDE:
			// Catch divide by 0 exception.
			if (memory[operand] != 0) {
				accumulator /= memory[operand];
				arithResultCheck();
			} else {
				cout << "*** Attempt to divide by zero ***" << endl;
				printError();
			}
			break;
		case MULTIPLY:
			accumulator *= memory[operand];
			arithResultCheck();
			break;

		case BRANCH:
			instructionCounter = operand;
			--instructionCounter;
			break;
		case BRANCHNEG:
			if (accumulator < 0) {
				instructionCounter = operand;
				--instructionCounter;
			}
			break;
		case BRANCHZERO:
			if (accumulator == 0) {
				instructionCounter = operand;
				--instructionCounter;
			}
			break;
		case HALT:
			cout << endl << "*** Simpletron execution terminated ***" << endl;
			printState();
			instructionCounter = MEMSIZE - 1;
			break;
		}
		++instructionCounter;
	}
}

