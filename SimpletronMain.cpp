//============================================================================
// Name        : Simpletron.cpp
// Author      : John Pulford
// Version     :
// Copyright   : GNU
// Description : Hello World in C<<<<, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <queue>
#include <vector>

#include <stdlib.h>

#include "Simpletron.h"
#include "SimpletronCompiler.h"
#include "TableEntry.h"

using namespace std;

int main() {
	SimpletronCompiler sc;



	ofstream out;
	out.open("outfile.txt");

	sc.compileFile("infile.txt", out);
	out.close();

	ifstream in("outfile.txt");

	Simpletron st;

	st.streamInput(in);
	st.execute();

	return 0;

}
