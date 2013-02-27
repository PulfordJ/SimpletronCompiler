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
#include <cctype>
#include <queue>

using namespace std;

inline int precedence(char sign);

inline bool isOperator(char c);

queue<string> infixToPostfix(string infix);
