/*
 * SimpletronCompiler.cpp
 *
 *  Created on: 26 Feb 2013
 *      Author: jwpulf
 */

#include "SimpletronCompiler.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <queue>
#include <stdlib.h>

using namespace std;

bool SimpletronCompiler::isElementInSymbolTable(int symbol, int type) {

	bool symbolFound = false;


	//TODO: ITERATE THROUGH EACH ENTRY IN TABLE TO FIND IF SYMBOL ROW EXISTS.
	for (int i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i].getSymbol() == symbol
				&& symbolTable[i].getType() == type) //TODO: Complete statement
		{
			symbolFound = true;
			break;
		}
	}
	return symbolFound;

}

TableEntry SimpletronCompiler::getSymbolTableRow(int symbol, int type) {
	// Search for variable, if found return.
	for (unsigned int i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i].getSymbol() == symbol
				&& symbolTable[i].getType() == type)
		{
			return symbolTable[i];
		}
	}

	// If variable doesn't exist then add it.
	TableEntry te(symbol, type, --dataCounter);
	symbolTable.push_back(te);
	return te;

}

SimpletronCompiler::SimpletronCompiler() {
	// Flags for second pass, -1 if no pass needed.
	for (unsigned int i = 0; i < MEMSIZE; i++)
		flags[i] = -1;
	dataCounter = MEMSIZE; // Point last variable was added to memory.
	instructionCounter = -1; // Last statement was added to memory.

}

void SimpletronCompiler::compileFile(string filename) {

	ifstream in;
	in.open(filename.c_str()); //open file to compile
	if (!in)
		cout << "infile not found";

	while (getline(in, oneline)) {

		// Parse line to a queue.

		istringstream iss(oneline);
		deque<string> tokens;

		copy(istream_iterator<string>(iss), istream_iterator<string>(),
				back_inserter<deque<string> >(tokens));

		/*
		// Debugging function, read all tokens from front.
		int p = 0;
		while (!tokens.empty())
		{
			++p;
			cout << "Token number: " << p << "Token: " << tokens.front() << endl;
			tokens.pop_front();
		}
		*/

		//Take queue and parse into symbolTable
		int symbol = atoi(tokens.front().c_str());
		tokens.pop_front();

		// Store line number in table if it doesn't exist

		if (!(this->isElementInSymbolTable(symbol, 'L'))) {
				TableEntry te(symbol, 'L', ++instructionCounter);
				symbolTable.push_back(te);
		}

		string command = tokens.front();
		tokens.pop_front();


		// React to each command
		if (command == "input")
		{
			// Next value is a variable
			char var = (char) atoi(tokens.front().c_str());
			tokens.pop_front();

			if (this->isElementInSymbolTable(var, 'V')) {

			}
			else
			{
				// If variable doesn't exist then add it.
				TableEntry te(var, 'V', --dataCounter);
				symbolTable.push_back(te);
				smlInstructs[++instructionCounter] = Simpletron::READ*100 + te.getLocation();
			}
			//

		}

		else if (command == "rem") --instructionCounter;

		else if (command == "let"){
			// Get variable being assigned a value
			char var = (char) atoi(tokens.front().c_str());
						tokens.pop_front();
						tokens.pop_front(); // Consume '='


			TableEntry varRow = this->getSymbolTableRow(var, 'V');

			string infixExp;

			while (!tokens.empty()) {
				infixExp += tokens.front();
				tokens.pop_front();
			}



		}

		else if (command == "if"){
			// Get LHS variable
			char var = (char) atoi(tokens.front().c_str());
			tokens.pop_front();

			TableEntry varTe =  this->getSymbolTableRow(var, 'V');


			// Next value is a comparison
			string comparison = tokens.front();
			tokens.pop_front();



			// Get RHS variable
			char var2 = (char) atoi(tokens.front().c_str());
			tokens.pop_front();

			TableEntry var2Te = this->getSymbolTableRow(var2, 'V');

			int locationName;
			if (tokens.front() == "goto"){
				tokens.pop_front();

				//Get line number if statement should go to if true.
				int lineNo = atoi(tokens.front().c_str());
				tokens.pop_front();

				// Check if line this points to has been compiled
				if (this->isElementInSymbolTable(lineNo, 'L')) {
					locationName = this->getSymbolTableRow(lineNo, 'L').getLocation();
				}
				else
				{
					locationName = 0;
					flags[instructionCounter] = lineNo;
				}
			}

			if (comparison == "==")
			{
				// Load x into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD*100 + varTe.getLocation();
				// subtract y from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT*100 + var2Te.getLocation();

				// and do branch zero check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHZERO*100 + locationName;
				//TODO: Get branch location from remaining part of statement.
			}

		}

	}


	in.close(); //closes in file
}
