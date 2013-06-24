SimpletronCompiler
==================
Introduction:
-------------
  This is an implementation for the Simpletron interpreter and compiler from C++: How to Program (Dietel and Dietel, 8th edition). I have written some recommendations for those looking to extend this (Based on academic experience and experience at ARM since writing it).

Recommendations for cloneing: 
----------------------------
* Apply the builder and factory patterns for cleaner more easily extensible source code.
* Seperate parsing from lexical analysis
* Replace the stack placed parser with a parse tree; with the right grammar operation precedence can be easily achieved.

Background History:
-------------------
I recently had an interview at ARM in their compiler division. I wanted to brush up on C++ and remembered this excercise from the book so I built it and demonstrated it in the interview. I am currently in a contract for that job as I'm typing this :).
