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
#include <sstream>
#include "InToPostFix.h"
#include "Utilities.h"
#include <algorithm>
#include <iterator>

using namespace std;

/**
 * Find precedence of operator.
 * @param sign - operator to find precedence of.
 * @return int - Precedence of operator.
 */
inline int precedence(char sign) {
	if (sign == '*' || sign == '/' || sign == '%')
		return 5;
	if (sign == '+' || sign == '-')
		return 3;
	else
		return 0;

}

/**
 * Check if value is operator.
 * @param c - character to inspect.
 * @return true if operator, otherwise false.
 */
inline bool isOperator(char c) {
	return c == '*' || c == '/' || c == '%' || c == '+' || c == '-';
}

/**
 * Convert infix expression to postfix.
 * @param infix - infix expression
 * @return queue - queue of strings for postfix evaluation.
 */
queue<string> infixToPostfix(string infix) {
	queue<string> postfix;
	unsigned int i;

	stack<char> myStack;
	myStack.push('(');
	infix += ')';

	//create vector of infix tokens from string.
	istringstream iss(infix);
	deque<string> tokens; // Because queues don't play well with copy.

	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<deque<string> >(tokens));

	while(!tokens.empty()) {
		string currStr = tokens.front();
		tokens.pop_front();

		if (util::isStringNumber(currStr)) { // Deal with numbers
			postfix.push(currStr);
		}
		else if (currStr.at(0) == '(')
			myStack.push('(');
		else if (isOperator(currStr.at(0))) {
			while (isOperator(myStack.top())
					&& precedence(myStack.top()) >= precedence(currStr.at(0))) { // This handles implicit precedence given by certain operators in an expression.
				//Simplest way to convert char to string.
				string s;
				s += myStack.top();
				postfix.push(s);
				myStack.pop();
			}
			myStack.push(currStr.at(0));
		} else if (currStr.at(0) == ')') {
			char stackChar = myStack.top();
			myStack.pop();
			while (stackChar != '(') {
				//Simplest way to convert char to string.
				string s;
				s += stackChar;
				postfix.push(s);
				stackChar = myStack.top();
				myStack.pop();
			}
		}
	}
	return postfix;
}
