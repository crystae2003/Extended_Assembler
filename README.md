# Extended_Assembler
DONE BY :Hemlata Gautam
         (2022csb1084)


after compiling : wait for few mins;
## DEPENDENCIES-
This C++ program implements branch prediction using single-bit and two-bit techniques for RISC-V instructions.
It reads input from a file, processes branch instructions, calculates prediction accuracy, and generates an output file with branch predictions.
This program requires the C++ standard library and uses the ifstream and ofstream classes for file input/output.

## COMPILATION-
To compile the code, use a C++ compiler such as g++:
- g++ -o branch_prediction branch_prediction.cpp
-Make sure to keep the input file in the same folder of branchprediction.cpp has placed.

## OUTPUT:
The program generates an output file named output.txt containing branch predictions in terms of T,NT.
On the Terminal- it will generate predictions and calculates prediction accuracy for one_bit and two_bit.

Following is the output for my inputfile :-

Branch guessor type       Accuracy (%)
Single Bit guessor       97.16
two Bit guessor          98.38
Always Not taken         56.65
Always taken             43.35





