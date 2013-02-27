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

inline int precedence(char sign) {
	if (sign == '*' || sign == '/' || sign == '%')
		return 5;
	if (sign == '+' || sign == '-')
		return 3;
	else
		return 0;

}

inline bool isOperator(char c) {
	return c == '*' || c == '/' || c == '%' || c == '+' || c == '-';
}

queue<string> infixToPostfix(string infix) {
	queue<string> postfix;
	unsigned int i;

	stack<char> myStack;
	myStack.push('(');
	infix += ')';

	//create vector of infix tokens from string.
	istringstream iss(infix);
	deque<string> tokens;

	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<deque<string> >(tokens));

	while(!tokens.empty()) {
		string currStr = tokens.front();
		tokens.pop_front();

		cout << "Current string is: " << currStr << endl;
		if (util::isStringNumber(currStr)) {
			cout << "String " << currStr << "Is a number." << endl;
			postfix.push(currStr);
		}
		else if (currStr.at(0) == '(')
			myStack.push('(');
		else if (isOperator(currStr.at(0))) {
			while (isOperator(myStack.top())
					&& precedence(myStack.top()) >= precedence(currStr.at(0))) {
				//Simpleist way to convert char to string.
				string s;
				s += myStack.top();
				postfix.push(s);
				myStack.pop();
			}
			//TODO: Pop operators (if there are any) at the top o the stack while they have equal or higher precedence than the current operator, then insert the popped operators in postfix.
			myStack.push(currStr.at(0));
		} else if (currStr.at(0) == ')') {
			char stackChar = myStack.top();
			myStack.pop();
			while (stackChar != '(') {
				//Simpleist way to convert char to string.
				string s;
				s += stackChar;
				postfix.push(s);
				stackChar = myStack.top();
				myStack.pop();
			}
		}

		//Post fix to infix here.
		/**
		 * Push a left parenthesis ( onto the stack
		 * Append a right parenthesis to the end of infix.
		 * while stack not empty, read infix from left to right and do following.
		 * 		If char in infix is digit copy to next element of postfix.
		 * 		if char is left parenthesis, push onto the stack.
		 * 		If operator
		 * 			pop operators (if they are any) if higher precedence than the current operator, and insert the popped operators in postfix.
		 * 			push the current char in infix onto the stack
		 * 		If the current character in infix is a right parenthesis
		 * 			pop operators from the top of the stack and insert them in postfix until a left parenthesis is at the top of the stack.
		 * 			pop (and discard) the left parenthesis from the stack
		 *
		 */
	}
	return postfix;
}
/*
 int main() {
 string infix = "(6 + 2) * 5 - 8 / 4";

 cout << "State of infix: " << infix << endl;
 cout << "State of postfix: " << infixToPostfix(infix);

 return 0;
 }
 */
