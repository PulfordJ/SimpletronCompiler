/*
 * Simpletron.h
 *
 *  Created on: 24 Feb 2013
 *      Author: john
 */

#ifndef SIMPLETRON_H_
#define SIMPLETRON_H_

class Simpletron {
public:
	Simpletron();
	void printState();
private:
	const int MEMSIZE = 100;
	const int ENDLOOP = -99999;

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
};

#endif /* SIMPLETRON_H_ */
