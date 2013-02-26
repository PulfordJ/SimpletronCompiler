#ifndef INTOPOSTFIX_H_
#define INTOPOSTFIX_H_

#endif

//============================================================================
// Name        : InToPostFixConverter.cpp
// Author      : John Pulford
// Version     :
// Copyright   : GNU
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stack>
#include <cctype>

using namespace std;

inline int precedence(char sign);

inline bool isOperator(char c);

string infixToPostfix(string infix);
