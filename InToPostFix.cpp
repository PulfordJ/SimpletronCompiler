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
#include "InToPostFix.h"

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

string infixToPostfix(string infix) {
	string postfix = "";
	unsigned int i;

	stack<char> myStack;
	myStack.push('(');
	infix += ')';

	for (i = 0; !myStack.empty() && i < infix.length(); i++) {
		char currChar = infix.at(i);

		if (isdigit(currChar))
			postfix += currChar;
		else if (currChar == '(')
			myStack.push('(');
		else if (isOperator(currChar)) {
			while (isOperator(myStack.top())
					&& precedence(myStack.top()) >= precedence(currChar)) {
				postfix += myStack.top();
				myStack.pop();
			}
			//TODO: Pop operators (if there are any) at the top o the stack while they have equal or higher precedence than the current operator, then insert the popped operators in postfix.
			myStack.push(currChar);
		} else if (currChar == ')') {
			char stackChar = myStack.top();
			myStack.pop();
			while (stackChar != '(') {
				postfix += stackChar;
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
