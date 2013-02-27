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
 * @param strLineNo - the line number the Simple goto command references
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
 * @return
 */
int SimpletronCompiler::postFixToSML(queue<string> postfixStack) {
	stack<int> evalStack; // Holds addresses of variables to be evaluated in SML.

	//Evaluate postfix expressions, numbers refer to addresses in memory.
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

			//Decide which type of instruction should be applied to the accumulator in the Simpletron.
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

			// Temporarily store result, don't need this once evaluation completed so room for optimisation.
			smlInstructs[++instructionCounter] = Simpletron::STORE * 100
					+ --dataCounter;

			evalStack.push(dataCounter);
		}
	}
	// Last address in evalStack is where result is stored after calculation.

	return evalStack.top();

	/*
	 While you have not reached the end of the string, read the expression from left to right.
	 If the current character is a digit,
	 Push its integer value onto the stack (the integer value of a digit character is its
	 value in the computer’s character set minus the value of '0' in the
	 computer’s character set).

	 Otherwise, if the current character is an operator,
	 Pop the two top elements of the stack into variables x and y.
	 Calculate y operator x.
	 Push the result of the calculation onto the stack.
	 2) When you reach the end of the string, pop the top value of the stack. This is the result
	 of the postfix expression.
	 */

}

bool SimpletronCompiler::isStringVar(string str) {
	/*
	 return str.size() == 1 && str.at(0) >= 'A'
	 && str.at(0) <= 'Z';
	 */
	if (str.size() == 1 && str.at(0) >= 'a' && str.at(0) <= 'z') {
		cout << str << " is an alphabetical character." << endl;
		return true;
	} else {
		cout << str << " is not an alphabetical character." << endl;
		return false;
	}
}

bool SimpletronCompiler::isElementInSymbolTable(int symbol, char type) {

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

TableEntry SimpletronCompiler::getSymbolTableRow(int symbol, char type) {
	/*
	 * If type = 'V' make char
	 * if type = 'C' make int
	 */

	if (type == 'V') {
		// Convert symbol to char and search for that.
	}

	// Search for variable, if found return.
	for (unsigned int i = 0; i < symbolTable.size(); i++) {
		if (symbolTable[i].getSymbol() == symbol
				&& symbolTable[i].getType() == type) {
			return symbolTable[i];
		}
	}

	cout << endl << "Adding nonexisting variable " << symbol << " of type "
			<< (char) type << endl;
	if (type == 'C')
		cout << "MATCHING TYPE, DREAM COME TRUE";
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

		cout << endl << "Adding line number to symbol table" << endl;
		// Store line number in table if it doesn't exist
		if (!(this->isElementInSymbolTable(symbol, 'L'))) {
			TableEntry te(symbol, 'L', instructionCounter + 1);
			symbolTable.push_back(te);
			cout << "Symbol: " << te.getSymbol() << " ( "
					<< char(te.getSymbol()) << " ) " << " Type: "
					<< te.getType() << " Location: " << (int) te.getLocation()
					<< endl;
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
			//string infixExp;
			ostringstream infixStream;
			cout << endl << "Analysing expression: ";
			while (!tokens.empty()) {
				string str = tokens.front();
				cout << str;
				tokens.pop_front();
				bool isOp = true;
				if (util::isStringNumber(str)) {
					//Process constant.
					cout << "CONSTANT FOUND";
					// find location of variable, convert to int format and append.
					infixStream
							<< ((int) this->getSymbolTableRow(atoi(str.c_str()),
									'C').getLocation());

				} else if (SimpletronCompiler::isStringVar(str)) {
					//Process Variable.
					cout << "VARIABLE FOUND";
					infixStream
							<< ((int) this->getSymbolTableRow(str.at(0), 'V').getLocation());
				}

				// If operator append the operator
				else {
					// Store operator.
					infixStream << str;
				}
				/*
				 cout << "Location for char: " << str.at(0) << " Of type: "
				 << 'V' << " is "
				 << (int) this->getSymbolTableRow(str.at(0), 'V').getLocation()
				 << endl;
				 */
				infixStream << " ";
			}

			string infixExp;
			infixExp = infixStream.str();
			queue<string> postfix = infixToPostfix(infixExp);
			cout << endl << "Infix is: " << infixExp;
			/*
			 cout << endl << "Postfix is: ";

			 //TODO: comment out this test code.
			 while (!postfix.empty()) {
			 cout << postfix.front() << " ";
			 postfix.pop();
			 }
			 */

			// Load from result address
			int result = this->postFixToSML(postfix); // Store for later to preserve instruction order.
			smlInstructs[++instructionCounter] = Simpletron::LOAD * 100
					+ result; // Generate SML steps to calculate result and return address result will be stored in.

			//TODO: Investigate potential for optimization.
			// Store result in variable location.
			smlInstructs[++instructionCounter] = Simpletron::STORE * 100
					+ varRow.getLocation();

			cout << endl;

		}

		else if (command == "goto") // Unconditional goto, not same as goto with if.
				{
			// and do branch zero check on condition
			smlInstructs[++instructionCounter] = Simpletron::BRANCH * 100
					+ this->gotoToSML(tokens.front());
		}

		else if (command == "print") {
			// Convert to write command and use symbol table to find location of variable.
			cout << endl << "PRINT STATEMENT HAS BEGUN" << endl;
			/*
			 int z = 0;
			 while(!tokens.empty()){
			 cout << "Number: " << z << " str value: " << tokens.front().c_str() << " atoi value: " << atoi(tokens.front().c_str());
			 if (tokens.front().size() == 1)
			 cout << " char value: " << tokens.front().at(0);
			 cout << endl;
			 }
			 */

			cout << "Variable being checked in print statement is: "
					<< atoi(tokens.front().c_str()) << endl;
			smlInstructs[++instructionCounter] =
					Simpletron::WRITE * 100
							+ this->getSymbolTableRow(tokens.front().at(0), 'V').getLocation();

		}

		else if (command == "if") {
			// Get LHS variable
			char var = tokens.front().at(0);
			tokens.pop_front();

			TableEntry varTe = this->getSymbolTableRow(var, 'V');
			cout << "SUSPECTED FAULT HERE" << endl;
			cout << "Location for var: " << var << " Of type: " << 'V' << " is "
					<< (int) varTe.getLocation() << endl;

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
			cout << "Flag value: " << flags[i];
			//Find Simple line number and use it to find SML line number.
			for (int j = 0; j < symbolTable.size(); j++) {
				cout << "Symbol: " << symbolTable[j].getSymbol() << " ( "
						<< char(symbolTable[j].getSymbol()) << " ) "
						<< " Type: " << symbolTable[j].getType()
						<< " Location: " << symbolTable[j].getLocation()
						<< endl;
				if (symbolTable[j].getSymbol() == flags[i])
					cout << endl << "Symbols are a match!" << endl;
				cout << "Symbol from table: " << symbolTable[j].getSymbol()
						<< endl;
				cout << "Matching flag: " << flags[i] << endl;
				if (symbolTable[j].getSymbol() == flags[i]
						&& symbolTable[i].getType() == 'L') { // line found
					smlInstructs[i] += symbolTable[j].getLocation();
					cout << endl << "Compiled location is : "
							<< symbolTable[j].getLocation() << endl;
					break;
				}
				cout << "Symbol entry not a match..." << endl;
			}
		}
	}

	// Print compiled instructions out to screen.
	cout << endl << "Printing out resulting instructions" << endl;
	for (int i = 0; i <= instructionCounter; i++) {
		cout << setfill('0') << setw(5) << showpos << internal
				<< smlInstructs[i] << endl;
	}

	//TODO: Print end objectTable
	cout << endl << "Printing out symbol table" << endl;

	for (std::vector<TableEntry>::iterator it = symbolTable.begin();
			it != symbolTable.end(); ++it) {
		cout << "Symbol: " << it->getSymbol() << " ( " << char(it->getSymbol())
				<< " ) " << " Type: " << it->getType() << " Location: "
				<< it->getLocation() << endl;
	}

}
