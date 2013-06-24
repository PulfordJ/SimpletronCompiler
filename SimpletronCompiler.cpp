/*
 * SimpletronCompiler.cpp
 *
 *  Created on: 26 Feb 2013
 *      Author: jwpulf
 */

#include "SimpletronCompiler.h"
#include "InToPostFix.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <iomanip>
#include "Utilities.h"

using namespace std;

/**
 * Returns the location in SML that should be jumped to, or 0 and makes a note to check again on the second pass.
 * @param strLineNo - the line number the Simple goto command references.
 * @return locationNo - the location the SML statement should point to, or 0 if not yet defineable.
 */
int SimpletronCompiler::gotoToSML(string strLineNo) {
	int locationNo;

	//Get line number if statement should go to if true.
	int lineNo = atoi(strLineNo.c_str());

	// Check if line this points to has been compiled
	if (this->isElementInSymbolTable(lineNo, 'L')) {
		locationNo = this->getSymbolTableRow(lineNo, 'L').getLocation();
	} else {
		// Return dummy value, mark in flags array for second pass to calculate.
		locationNo = 0;
		flags[instructionCounter] = lineNo;
	}
	return locationNo;
}

/**
 * Converts postFix stack of operators and addresses to SML instruction sequence.
 * @param postfixStack - postfix stack of operators and addresses.
 * @return address where result of expression is stored.
 */
int SimpletronCompiler::postFixToSML(queue<string> postfixStack) {
	stack<int> evalStack; // Holds addresses of variables to be evaluated in SML.

	// Evaluate postfix expressions, numbers refer to addresses in memory.
	while (!postfixStack.empty()) {

		string currStr = postfixStack.front();
		postfixStack.pop();
		if (util::isStringNumber(currStr)) {
			int position = atoi(currStr.c_str());
			evalStack.push(position);
		}
		// Must be an operator, evaluate.
		else {
			char op = currStr.at(0);
			// Convert postfix operations into SML commands.
			int rOperand = evalStack.top();
			evalStack.pop();

			int lOperand = evalStack.top();
			evalStack.pop();

			smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
					+ lOperand;

			// Decide which type of instruction should be applied to the accumulator in the Simpletron.
			switch (op) {
			case '+':
				smlInstructs[++instructionCounter] = Simpletron::ADD * 100
						+ rOperand;
				break;
			case '-':
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ rOperand;
				break;
			case '/':
				smlInstructs[++instructionCounter] = Simpletron::DIVIDE * 100
						+ rOperand;
				break;
			case '*':
				smlInstructs[++instructionCounter] = Simpletron::MULTIPLY * 100
						+ rOperand;
				break;
			}

			// Temporarily store result, don't need this once evaluation completed so room for optimization.
			smlInstructs[++instructionCounter] = Simpletron::STORE * 100
					+ --dataCounter;

			evalStack.push(dataCounter);
		}
	}
	// Last address in evalStack is where result is stored after calculation, could have made this the address of variable so room for optimization.

	return evalStack.top();

}

/**
 * Check if a string is a variable value.
 * @param str - String to compare.
 * @return true if alphabetic character, otherwise false.
 */
bool SimpletronCompiler::isStringVar(string str) {

	return str.size() == 1 && str.at(0) >= 'a' && str.at(0) <= 'z';
}

/**
 * Checks if an element of a specific type exists in the symbol table.
 * @param symbol - Value of symbol.
 * @param type - type of symbol (L, V or C) i.e Line Number, Variable or Constant.
 * @return true if entry for symbol found, otherwise false.
 */
bool SimpletronCompiler::isElementInSymbolTable(int symbol, char type) {

	bool symbolFound = false;

	for (int i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i].getSymbol() == symbol
				&& symbolTable[i].getType() == type) {
			symbolFound = true;
			break;
		}
	}
	return symbolFound;

}

/*
 * Returns row containing symbol and type
 * @param symbol - Value of symbol.
 * @param type - type of symbol (L, V or C) i.e Line Number, Variable or Constant.
 * @return table entry containing the symbol and type.
 */
TableEntry SimpletronCompiler::getSymbolTableRow(int symbol, char type) {

	// Search for variable, if found return.
	for (unsigned int i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i].getSymbol() == symbol
				&& symbolTable[i].getType() == type) {

			return symbolTable[i];
		}
	}

	// If variable doesn't exist then add it.
	TableEntry te(symbol, type, --dataCounter);
	symbolTable.push_back(te);
	if (type == 'C') { // First time a constant is found instruction needs to be written to store it in location (NOT IN BOOK)
		smlInstructs[dataCounter] = symbol;
	}
	return te;

}

/**
 * Initialize flags array and set instance variable values.
 */
SimpletronCompiler::SimpletronCompiler() {
	// Flags for second pass, -1 if no pass needed.
	for (unsigned int i = 0; i < MEMSIZE; i++) {
		flags[i] = -1;
		smlInstructs[i] = 0;
	}
	dataCounter = MEMSIZE; // Point last variable was added to memory.
	instructionCounter = -1; // Last statement was added to memory.

}

/**
 * This method compiles a file specified by filename and outputs it to stream.
 * @param filename - name of file to find input code from.
 * @param out - name of stream to output compiled code from.
 */
void SimpletronCompiler::compileFile(string filename, ostream& out) {

	ifstream in;
	in.open(filename.c_str()); //open file to compile
	if (!in) {
		cout << "infile not found";
		exit(1);
	}

	while (getline(in, oneline)) {

		// Tokenise line into a queue.

		istringstream iss(oneline);
		deque<string> tokens; // Using deque because queue doesn't work well with copy.

		copy(istream_iterator<string>(iss), istream_iterator<string>(),
				back_inserter<deque<string> >(tokens));

		//Take queue and parse into symbolTable
		int symbol = atoi(tokens.front().c_str());
		tokens.pop_front();

		// Store line number in table if it doesn't exist
		if (!(this->isElementInSymbolTable(symbol, 'L'))) {
			TableEntry te(symbol, 'L', instructionCounter + 1);
			symbolTable.push_back(te);
		}

		string command = tokens.front();
		tokens.pop_front();

		// React to each command

		if (command == "input") {
			// Next value is a variable
			char var = tokens.front().at(0);
			tokens.pop_front();

			// Convert to SML read and get location it should be read to
			smlInstructs[++instructionCounter] = Simpletron::READ * 100
					+ this->getSymbolTableRow(var, 'V').getLocation();

		}

		else if (command == "let") {

			// Get variable being assigned a value
			char var = tokens.front().at(0);
			tokens.pop_front();
			tokens.pop_front(); // Consume '='

			TableEntry varRow = this->getSymbolTableRow(var, 'V');

			ostringstream infixStream;
			while (!tokens.empty()) {
				string str = tokens.front();
				tokens.pop_front();
				if (util::isStringNumber(str)) { //Process constants.

					// find location of variable, convert to int format and append.
					infixStream
							<< ((int) this->getSymbolTableRow(atoi(str.c_str()),
									'C').getLocation());

				} else if (SimpletronCompiler::isStringVar(str)) { //Process Variables.
					infixStream
							<< ((int) this->getSymbolTableRow(str.at(0), 'V').getLocation());
				}

				// If operator append the operator
				else {
					// Store operator.
					infixStream << str;
				}
				infixStream << " "; //TODO Rigid syntax requires space seperators between registers, could be improved with stack.
			}

			queue<string> postfix = infixToPostfix(infixStream.str()); // Generate postfix queue from generated infix-with-addresses expression.

			// Load from result address
			int result = this->postFixToSML(postfix); // Store for later to preserve instruction order.
			smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
					+ result; // Generate SML steps to calculate result and return address result will be stored in.

			//TODO: Investigate potential for optimization.
			// Store result in variable location.
			smlInstructs[++instructionCounter] = Simpletron::STORE * 100
					+ varRow.getLocation();

		}

		else if (command == "goto") // Unconditional goto, not same as goto with if.
				{
			// and do branch zero check on condition
			smlInstructs[++instructionCounter] = Simpletron::BRANCH * 100
					+ this->gotoToSML(tokens.front());
		}

		else if (command == "print") {
			// Convert to write command and use symbol table to find location of variable.
			smlInstructs[++instructionCounter] =
					Simpletron::WRITE * 100
							+ this->getSymbolTableRow(tokens.front().at(0), 'V').getLocation();

		}

		else if (command == "if") {
			// Get LHS variable
			char var = tokens.front().at(0);
			tokens.pop_front();

			TableEntry varTe = this->getSymbolTableRow(var, 'V');

			// Next value is a comparison
			string comparison = tokens.front();
			tokens.pop_front();

			// Get RHS variable
			char var2 = tokens.front().at(0);
			tokens.pop_front();

			TableEntry var2Te = this->getSymbolTableRow(var2, 'V');

			if (comparison == "==") {
				// Load LHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ varTe.getLocation();
				// subtract RHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ var2Te.getLocation();

				tokens.pop_front(); // remove "goto" statement.
				// and do branch zero check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHZERO
						* 100 + this->gotoToSML(tokens.front());
			} else if (comparison == "<") {
				// Load LHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ varTe.getLocation();
				// subtract RHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ var2Te.getLocation();

				tokens.pop_front(); // remove "goto" statement.
				// and do branch neg check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHNEG * 100
						+ this->gotoToSML(tokens.front());
			} else if (comparison == ">") {
				// Load RHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ var2Te.getLocation();
				// subtract LHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ varTe.getLocation();

				tokens.pop_front(); // remove "goto" statement.
				// and do branch neg check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHNEG * 100
						+ this->gotoToSML(tokens.front());
			} else if (comparison == "<=") {
				// Load LHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ varTe.getLocation();
				// subtract RHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ var2Te.getLocation();

				tokens.pop_front(); // remove "goto" statement.
				// and do branch neg check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHNEG * 100
						+ this->gotoToSML(tokens.front());
				smlInstructs[++instructionCounter] = Simpletron::BRANCHZERO
						* 100 + this->gotoToSML(tokens.front());
			} else if (comparison == ">=") {
				// Load RHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ var2Te.getLocation();
				// subtract LHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ varTe.getLocation();

				tokens.pop_front(); // remove "goto" statement.
				// and do branch neg check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHNEG * 100
						+ this->gotoToSML(tokens.front());
				smlInstructs[++instructionCounter] = Simpletron::BRANCHZERO
						* 100 + this->gotoToSML(tokens.front());
			} else if (comparison == "!=") { // Achievable by checking both less than and greater than.
				// Load LHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ varTe.getLocation();
				// subtract RHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ var2Te.getLocation();

				tokens.pop_front(); // remove "goto" statement.
				// and do branch neg check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHNEG * 100
						+ this->gotoToSML(tokens.front());

				// Load RHS into accumulator,
				smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
						+ var2Te.getLocation();
				// subtract LHS from it
				smlInstructs[++instructionCounter] = Simpletron::SUBTRACT * 100
						+ varTe.getLocation();
				// and do branch neg check on condition
				smlInstructs[++instructionCounter] = Simpletron::BRANCHNEG * 100
						+ this->gotoToSML(tokens.front());

			}

		}

		else if (command == "end") {
			// End translates directly into a HALT command.
			smlInstructs[++instructionCounter] = Simpletron::HALT * 100;
		}

	}

	in.close(); //closes in file

	// SECOND PASS OVER
	for (int i = 0; i <= instructionCounter; i++) {
		//If location of instruction unfinished...
		if (flags[i] != -1) {
			//Find Simple line number and use it to find SML line number.
			for (int j = 0; j < symbolTable.size(); j++) {
				if (symbolTable[j].getSymbol() == flags[i]
						&& symbolTable[i].getType() == 'L') { // line found
					smlInstructs[i] += symbolTable[j].getLocation();
					break;
				}
			}
		}
	}

	// Print compiled instructions out to stream
	for (int i = 0; i < MEMSIZE; i++) {
		if (smlInstructs[i] != 0)
			out << setfill('0') << setw(2) << noshowpos << i << setfill('0')
					<< setw(5) << showpos << internal << smlInstructs[i]
					<< endl;
	}

	/*
	 //TODO: Print end objectTable
	 cout << endl << "Printing out symbol table" << endl;

	 for (std::vector<TableEntry>::iterator it = symbolTable.begin();
	 it != symbolTable.end(); ++it) {
	 cout << "Symbol: " << it->getSymbol() << " ( " << char(it->getSymbol())
	 << " ) " << " Type: " << it->getType() << " Location: "
	 << it->getLocation() << endl;
	 }
	 */

}
