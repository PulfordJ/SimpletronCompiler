/*
 * SimpletronCompiler.h
 *
 *  Created on: 26 Feb 2013
 *      Author: jwpulf
 */

#ifndef SIMPLETRONCOMPILER_H_
#define SIMPLETRONCOMPILER_H_

#include <vector>
#include "TableEntry.h"
#include <string>

#include "Simpletron.h"

using namespace std;

class SimpletronCompiler {
public:
	SimpletronCompiler();
	void compileFile(string filename);
private:
	TableEntry getSymbolTableRow(int symbol, int type);
	bool isElementInSymbolTable(int symbol, int type);
	static const int MEMSIZE = 100; // Size of Simpletron RAM.
	vector<TableEntry> symbolTable; // symbol table for compilation.
	int flags[100]; // Flags for second pass.
	int smlInstructs[MEMSIZE]; // The instructions for SML, ordered by array indices.
	int dataCounter; // Point to add next variable to memory.
	int instructionCounter; // Point to add next statement to memory.
	string oneline;


};

#endif /* SIMPLETRONCOMPILER_H_ */
