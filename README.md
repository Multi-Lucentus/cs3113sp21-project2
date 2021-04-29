# Intro to OS - Project 2 (Memory Management)
by Parker Brandt (ID: 113521131)


<h3>How To Run</h3>
In order to run this program, a Makefile is provided with two commands: all and clean</br>
To compile and run the program, type and enter "make all" in the command line</br>
This will create an executable file named "project2"</br>
To run the executable type in the command line "./project2 [MEMORY ALGORITHM] [NUMBER OF BYTES TO ALLOCATE] [Script Filename]"</br>


<h3>Functions</h3>
<i>char** split(char* string, char split)</i></br>
The split function takes in a string (char* string) and splits it along a split character (char split).
The function then returns an array of strings.</br>
For example, if the input string is "This is a test", and the string is split along spaces, then the function will return ["This", "is", "a", "test"].</br>

<i>void insertInArray(Process addProc, Process* array, int index, int length)</i></br>
This function inserts a new Process into an array of processes.</br>
NOTE: A Process is a struct in the program that houses data about a Process' name, size, and start and end indices.</br>
The parameter addProc is the process to be inserted into the parameter array. The parameter index is provided to tell the function where to insert the process into in the array. Lastly, the parameter length is the current length of the array before the insertion.</br>

<i>void removeFromArray(Process* array, int index, int length)</i></br>
This function removes an element from the parameter array provided at the parameter index. The other parameter length is provided to help with looping.</br>


<h3>Memory Algorithm Functions</h3>
<i>int firstFit(Process newProc, Process* procList, int length, unsigned long totalMemory)</i></br>
The First Fit algorithm is a memory allocation algorithm that finds the first hole in memory that is big enough to hold a process and stores it there.</br>
The function performs the first fit algorithm and adds the parameter newProc to the procList array that stores the processes. The parameter length is the current length of the array of processes and totalMemory is the total amount of memory that the processes can be stored in.</br>
The function returns the index where the process was inserted, or on failure, it will return -1.</br></br>

<i>int bestFit(Process newProc, Process* procList, int length, unsigned long totalMemory)</i></br>
The Best Fit algorithm is a memory allocation algorithm such that when allocating memory for a process, the algorithm iterates through the entire memory array to find the smallest hole that is still big enough for the process, and allocates the process to that spot.</br>
The parameters for bestFit() are the same as for firstFit(), and the return value is the same where the function returns the index to where the process was inserted, or -1 on failure</br></br>

<i>int worstFit(Process newProc, Process* procList, int length, unsigned long totalMemory)</i></br>
The Worst Fit algorithm is a memory allocation algorithm that is similar to the Best Fit algorithm, but instead of finding the smallest hole that is big enough for the process, the Worst Fit algorithm finds the largest hole and allocates the process there.</br>
The parameters and return values are the same for Worst Fit as they are for First Fit and Best Fit.</br></br>

<i>int nextFit(Process newProc, Process* procList, int* nextPtr, int length, unsigned long totalMemory)</i></br>
The Next Fit algorithm is a memory allocation algorithm that is similar to the First Fit algorithm, but Next Fit keeps a pointer to the next location that can allocate a process.</br>
Most of the parameters are the same for the Next Fit algorithm as the rest of the algorithms, except Next Fit has the next pointer as a parameter as nextPtr. The return values for the Next Fit function are the same as the previous memory allocation functions.
