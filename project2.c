#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define NAME_SIZE 24

// Structs
struct Process
{
	char* processName;
	unsigned long size;
	unsigned long startIndex, endIndex;
};
typedef struct Process Process;


// Function Prototypes
char** split(char* string, char split);

void insertInArray(Process addProc, Process* array, int index, int length);
void removeFromArray(Process* array, int index, int length);

int firstFit(Process newProc, Process* procList, int length, unsigned long totalMemory);
int bestFit(Process newProc, Process* procList, int length, unsigned long totalMemory);
int worstFit(Process newProc, Process* procList, int length, unsigned long totalMemory);
int nextFit(Process newProc, Process* procList, int* nextPtr, int length, unsigned long totalMemory);


// Main Function
int main(int argc, char** argv)
{
	// Variables
	char* inCommand;
	unsigned long totalMemory;
	char* scriptName;

	Process* memory;
	int procCount = 0;

	FILE* fp;

	// Check command-line arguments
	if(argc != 4)
	{
		dprintf(STDERR, "Illegal number of command-line arguments\n");
		exit(EXIT_FAILURE);
	}

	// Initialize variables
	inCommand = argv[1];
	totalMemory = strtoul(argv[2], NULL, 0);
	scriptName = argv[3];

	// Allocate size to the array
	memory = (Process*)malloc(2048 * sizeof(Process));
	
	// Open the file using a file pointer
	fp = fopen(scriptName, "r");
	if(fp == NULL)
		exit(EXIT_FAILURE);

	char* line = (char*)malloc(256 * sizeof(char));
	char** data;

	int* nextPtr = (int*)malloc(16 * sizeof(int));
	(*nextPtr) = 0;

	char* command;
	char* processName;
	unsigned long numBytes;
	while(fscanf(fp, "%[^\n]", line) != EOF)
	{
		// Use fgetc to move past \n
		fgetc(fp);

		// Split the string along spaces
		data = split(line, ' ');
		
		// Decide what action to perform based off of data[0]
		// NOTE: If data[0] == #, then that line is a comment and we can ignore
		command = data[0];

		if(strcmp(command, "REQUEST") == 0)
		{
			// Need to allocate memory for a process
			// Format: REQUEST ProcessName NumberofBytes
			processName = data[1];
			numBytes = strtoul(data[2], NULL, 0);

			// Create the new process to be added
			Process newProc;
			newProc.processName = (char*)malloc(NAME_SIZE * sizeof(char));
			strcpy(newProc.processName, processName);
			newProc.size = numBytes;

			// TODO: Testing
			// dprintf(STDOUT, "New Process: %s - %lu bytes\n", newProc.processName, newProc.size);
			
			// Check what algorithm is being used
			int index;
			if(strcmp(inCommand, "FIRSTFIT") == 0)
			{
				if((index = firstFit(newProc, memory, procCount, totalMemory)) != -1)
				{
					dprintf(STDOUT, "ALLOCATED %s %lu\n", memory[index].processName, memory[index].startIndex);
					procCount++;
				}
				else
					dprintf(STDOUT, "FAIL REQUEST %s %lu\n", newProc.processName, newProc.size);
			}
			else if(strcmp(inCommand, "BESTFIT") == 0)
			{
				if((index = bestFit(newProc, memory, procCount, totalMemory)) != -1)
				{
					dprintf(STDOUT, "ALLOCATED %s %lu\n", memory[index].processName, memory[index].startIndex);
					procCount++;
				}
				else
					dprintf(STDOUT, "FAIL REQUEST %s %lu\n", newProc.processName, newProc.size);
			}
			else if(strcmp(inCommand, "WORSTFIT") == 0)
			{
				if((index = worstFit(newProc, memory, procCount, totalMemory)) != -1)
				{
					dprintf(STDOUT, "ALLOCATED %s %lu\n", memory[index].processName, memory[index].startIndex);
					procCount++;
				}
				else
					dprintf(STDOUT, "FAIL REQUEST %s %lu\n", newProc.processName, newProc.size);
			}
			else if(strcmp(inCommand, "NEXTFIT") == 0)
			{
				if((index = nextFit(newProc, memory, nextPtr, procCount, totalMemory)) != -1)
				{
					dprintf(STDOUT, "ALLOCATED %s %lu\n", memory[index].processName, memory[index].startIndex);
					procCount++;
				}
				else
					dprintf(STDOUT, "FAIL REQUEST %s %lu\n", newProc.processName, newProc.size);
			}
		}
		else if(strcmp(command, "RELEASE") == 0)
		{
			// Releases the memory held by a process
			// Loop through the array to find the right process name
			processName = data[1];

			int hasBeenFound = -1;
			for(int i = 0; i < procCount; i++)
			{
				if(strcmp(memory[i].processName, processName) == 0)
				{
					// The right has been found, need to remove
					dprintf(STDOUT, "FREE %s %lu %lu\n", memory[i].processName, memory[i].size, memory[i].startIndex);

					removeFromArray(memory, i, procCount);
					procCount--;

					hasBeenFound++;
				}
			}

			// If the process has not been found, print an error message
			if(hasBeenFound == -1)
				dprintf(STDOUT, "FAIL RELEASE %s\n", processName);
		}
		else if(strcmp(command, "LIST") == 0)
		{
			// The second part of the command will tell exactly what to do
			char* command2 = data[1];

			if(strcmp(command2, "ASSIGNED") == 0)
			{
				// Go through list and print all assigned process memory
				if(procCount == 0)
					dprintf(STDOUT, "NONE\n");

				for(int i = 0; i < procCount; i++)
				{
					dprintf(STDOUT, "(%s, %lu, %lu) ", memory[i].processName, memory[i].size, memory[i].startIndex);
					if(i == (procCount - 1))
						dprintf(STDOUT, "\n");
				}
			}
			else if(strcmp(command2, "AVAILABLE") == 0)
			{
				// Print out all available memory locations
				// If there are no processes, the whole thing is available
				if(procCount == 0)
					dprintf(STDOUT, "(%lu, 0)\n", totalMemory);
				else if(procCount == 1)
				{
					// If there is just one process, can print it out on its own
					dprintf(STDOUT, "(%lu, %lu)\n", totalMemory - memory[0].size, memory[0].endIndex + 1);
				}
				else
				{
					// Next, need to check if memory is full
					unsigned long remainMem = totalMemory;
					for(int i = 0; i < procCount; i++)
						remainMem -= memory[i].size;
				
					if(remainMem == 0)
						dprintf(STDOUT, "FULL\n");
					else
					{	
						// Search for open memory spots
						unsigned long startSpace = 0;
						unsigned long endSpace;
						for(int i = 0; i < procCount; i++)
						{
							endSpace = memory[i].startIndex;
	
							if((endSpace - startSpace) > 1)
							{
								// Found an open memory spot, print out the location
								dprintf(STDOUT, "(%lu, %lu) ", (endSpace - startSpace - 1), (startSpace + 1));
							}

							startSpace = memory[i].endIndex;
						}

						// Check for memory after processes
						if(startSpace != (totalMemory - 1) && procCount != 1)
							dprintf(STDOUT, "(%lu, %lu)\n", totalMemory - startSpace - 1, startSpace + 1);
						else
							dprintf(STDOUT, "\n");
					}
				}
			}
		}
		else if(strcmp(command, "FIND") == 0)
		{
			// Loop through the array and find a specific process
			processName = data[1];

			int hasBeenFound = -1;
			for(int i = 0; i < procCount; i++)
			{
				if(strcmp(processName, memory[i].processName) == 0)
				{
					dprintf(STDOUT, "(%s, %lu, %lu)\n", memory[i].processName, memory[i].size, memory[i].startIndex);
					hasBeenFound++;
				}
			}

			if(hasBeenFound == -1)
				dprintf(STDOUT, "FAULT\n");
		}
	}

	fclose(fp);

	exit(EXIT_SUCCESS);
}


// Function Declarations
/*
 * Splits a string according to a split character and then returns an
 * array of srings
 */
char** split(char* string, char split)
{
	// Count number of occurrences of the split character
	int splitCount = 0;
	for(int i = 0; i < strlen(string); i++)
		if(string[i] == split)
			splitCount++;

	char** stringArray = (char**)malloc(128 * sizeof(char*));
	for(int i = 0; i < (splitCount + 1); i++)
		stringArray[i] = (char*)malloc(64 * sizeof(char));

	int arrayCount = 1;

	stringArray[0] = strtok(string, &split);

	while(stringArray[arrayCount] != NULL)
	{
		stringArray[arrayCount] = strtok(NULL, &split);
		arrayCount++;
	}

	return stringArray;
}

/*
 * Inserts a Process into a Process array and shifts all other processes 
 */
void insertInArray(Process addProc, Process* array, int index, int length)
{
	if(index < 0 || index > length - 1)
		return;

	// Increase the length of the array and then move all elements after where index is inserted
	array[length].processName = (char*)malloc(NAME_SIZE * sizeof(char));
	
	for(int i = length; i > index; i--)
	{
		// Copy the element from the previous index
		strcpy(array[i].processName, array[i - 1].processName);
		array[i].size = array[i - 1].size;
		array[i].startIndex = array[i - 1].startIndex;
		array[i].endIndex = array[i - 1].endIndex;
	}

	strcpy(array[index].processName, addProc.processName);
	array[index].size = addProc.size;
	array[index].startIndex = addProc.startIndex;
	array[index].endIndex = addProc.endIndex;

	return;
}

/*
 * Removes an element from the given array
 */
void removeFromArray(Process* array, int index, int length)
{
	if(length == 0)
		return;

	for(int i = index; i < length - 1; i++)
	{
		strcpy(array[i].processName, array[i + 1].processName);
		array[i].size = array[i + 1].size;
		array[i].startIndex = array[i + 1].startIndex;
		array[i].endIndex = array[i + 1].endIndex;
	}

	free(array[length - 1].processName);

	return;
}

/*
 * First Fit Algorithm:
 * Finds the first big enough hole in memory and inserts the process there
 * Returns the index to where the process was inserted, or -1 on failure
 */
int firstFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{
	// If this is the first process, can manually add to beginning
	if(length == 0)
	{
		if(newProc.size > totalMemory)
			return -1;
		procList[0].processName = (char*)malloc(NAME_SIZE * sizeof(char));
		strcpy(procList[0].processName, newProc.processName);
		procList[0].size = newProc.size;

		// Set indices of process
		procList[0].startIndex = 0;
		procList[0].endIndex = procList[0].size - 1;

		return 0;
	}

	// Loop through the array and find the next spot to be able to put the process
	int index = -1;

	unsigned long startSpace = 0;
	unsigned long endSpace;
	for(int i = 0; i < length; i++)
	{
		endSpace = procList[i].startIndex;
		
		if((endSpace - startSpace) >= newProc.size)
		{
			index = i;	
			break;
		}

		startSpace = procList[i].endIndex;
	}

	if(index != -1)
	{
		// Set indices of process
		newProc.startIndex = procList[index - 1].endIndex + 1;
		newProc.endIndex = newProc.startIndex + newProc.size - 1;

		// Insert the process at the index
		insertInArray(newProc, procList, index, length);
		return index;
	}
	else
	{
		// If an index couldnt be found, check if process could be added to the end
		if((totalMemory - procList[length - 1].endIndex) >= newProc.size)
		{
			procList[length].processName = (char*)malloc(NAME_SIZE * sizeof(char));
			strcpy(procList[length].processName, newProc.processName);
			procList[length].size = newProc.size;

			// Set indices of process
			procList[length].startIndex = procList[length - 1].endIndex + 1;
			procList[length].endIndex = procList[length].startIndex + procList[length].size - 1;

			return length;
		}
		else
			return -1;
	}
}

/*
 * Best Fit Algorithm:
 * Find every memory hole and allocate the process to the smallest hole that is still big enough
 * Returns the index to which the process was allocated or -1 if failed 
 */
int bestFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{
	// If this is the first process to be added, can just manually add it to the beginning
	if(length == 0)
	{
		// Check to make sure process' size is not bigger than total amount of memory
		if(newProc.size > totalMemory)
			return -1;
		procList[0].processName = (char*)malloc(NAME_SIZE * sizeof(char));
		strcpy(procList[0].processName, newProc.processName);
		procList[0].size = newProc.size;

		// Set the indices of the process
		procList[0].startIndex = 0;
		procList[0].endIndex = procList[0].size - 1;

		// Testing
		// dprintf(STDERR, "Inserted Process %s of size %lu at index 0\n", procList[0].processName, procList[0].size);

		return 0;
	}

	// Loop through the array to find the smallest memory hole that is big enough for the new process
	int smallestIndex = -1;
	unsigned long smallestSize = totalMemory;

	unsigned long startSpace = 0;
	unsigned long endSpace;
	unsigned long testSpace;
	for(int i = 0; i < length; i++)
	{
		endSpace = procList[i].startIndex;
		
		testSpace = endSpace - startSpace;
		
		// If this space is bigger than the needed size for the process and smaller than the previous smallest size, we have found the correct spot
		if(testSpace > newProc.size && testSpace < smallestSize)
		{
			smallestIndex = i;
			smallestSize = testSpace;
		}

		startSpace = procList[i].endIndex;
	}

	// Allocate the process to the found smallest hole
	if(smallestIndex != -1)
	{
		// If it is being inserted at 0 index, need to adjust indices
		if(smallestIndex == 0)
			newProc.startIndex = 0;
		else
			newProc.startIndex = procList[smallestIndex - 1].endIndex + 1;
		
		newProc.endIndex = newProc.startIndex + newProc.size - 1;

		insertInArray(newProc, procList, smallestIndex, length);

		// Testing
		//dprintf(STDERR, "Inserted Process %s of size %lu at indexx %d\n", procList[smallestIndex].processName, procList[smallestIndex].size, smallestIndex);

		return smallestIndex;
	}
	else
	{
		// In this case, an index was never found
		// Can check to see if the process can be inserted after the last process, but if not, return -1
		if((totalMemory - procList[length - 1].endIndex) > newProc.size)
		{
			// Set the new process to the end of the list
			procList[length].processName = (char*)malloc(NAME_SIZE * sizeof(char));
			strcpy(procList[length].processName, newProc.processName);
			procList[length].size = newProc.size;

			// Set the indices
			procList[length].startIndex = procList[length - 1].endIndex + 1;
			procList[length].endIndex = procList[length].startIndex + procList[length].size - 1;

			// Testing
			// dprintf(STDERR, "Inserted Process %s of size %lu at index %d\n", procList[length].processName, procList[length].size, length);

			return length;
		}
		else
		{
			// dprintf(STDERR, "Could not insert\n");
			return -1;
		}
	}
}

/*
 * Worst Fit Algorithm:
 * Loops through all of the memory holes and inserts the process at the biggest memory hole
 * Very similar to best fit algorithm, but we find the largest hole instead
 * Returns the index of where the process was inserted or -1 on failure
 */
int worstFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{
	if(length == 0)
	{
		if(newProc.size > totalMemory)
			return -1;
		procList[0].processName = (char*)malloc(NAME_SIZE * sizeof(char));
		strcpy(procList[0].processName, newProc.processName);
		procList[0].size = newProc.size;

		// Set indices
		procList[0].startIndex = 0;
		procList[0].endIndex = procList[0].size - 1;

		return 0;
	}

	// Loop through the array to find the largest memory hole
	int biggestIndex = -1;
	unsigned long biggestSize = 0;

	unsigned long startSpace = 0;
	unsigned long endSpace;
	unsigned long testSpace;
	for(int i = 0; i < length; i++)
	{
		endSpace = procList[i].startIndex;
		
		testSpace = endSpace - startSpace;

		if(testSpace >= newProc.size && testSpace > biggestSize)
		{
			biggestIndex = i;
			biggestSize = testSpace;
		}

		startSpace = procList[i].endIndex;
	}

	if(biggestIndex != -1)
	{
		// Compare with the end of memory to see if that is larger
		if(biggestSize < testSpace && newProc.size >= testSpace)
		{
			biggestSize = testSpace;
			biggestIndex = length - 1;
		}

		// Adjust the indices of the new process and then insert into the array
		if(biggestIndex == 0)
			newProc.startIndex = 0;
		else
			newProc.startIndex = procList[biggestIndex - 1].endIndex + 1;
		newProc.endIndex = newProc.startIndex + newProc.size - 1;

		insertInArray(newProc, procList, biggestIndex, length);

		return biggestIndex;
	}
	else
	{
		// An index was never found in memory to insert
		// Check to see if proc can be inserted at end, and if not return -1
		if((totalMemory - procList[length - 1].endIndex) >= newProc.size)
		{
			// Set new process to end of list
			procList[length].processName = (char*)malloc(NAME_SIZE * sizeof(char));
			strcpy(procList[length].processName, newProc.processName);
			procList[length].size = newProc.size;

			// Set the indices
			procList[length].startIndex = procList[length - 1].endIndex + 1;
			procList[length].endIndex = procList[length].startIndex + procList[length].size - 1;

			return length;
		}
		else
			return -1;
	}
}

/*
 * Next Fit Algorithm:
 * Similar to first fit in the way that the algorithm finds the next spot to put a process
 * But a pointer is kept to the next available spot
 */
int nextFit(Process newProc, Process* procList, int* nextPtr, int length, unsigned long totalMemory)
{
	int index = *nextPtr;

	// If index and length are both 0, can just add to first element
	if(length == 0)
	{
		// Check to make sure the size of the process doesnt exceed the whole array
		if(newProc.size > totalMemory)
		{
			(*nextPtr) = -1;
			return -1;
		}
		
		// Set the process to the first in the array
		procList[0].processName = (char*)malloc(NAME_SIZE * sizeof(char));
		strcpy(procList[0].processName, newProc.processName);
		procList[0].size = newProc.size;

		// Set indices
		procList[0].startIndex = 0;
		procList[0].endIndex = procList[0].startIndex + procList[0].size - 1;

		// Increment the next pointer
		(*nextPtr)++;

		return 0;
	}

	// Search through the array from index till end to see if there is a spot available
	int foundIndex = -1;
	
	// Since the index could potentially be -1, have to check
	unsigned long startSpace;
	int i;
	if(index == -1)
	{
		i = 0;
		startSpace = 0;
	}
	else
	{
		i = index;
		startSpace = procList[index - 1].endIndex;
	}

	unsigned long endSpace;
	unsigned long testSpace;
	for(int j = i; j < length; j++)
	{
		endSpace = procList[j].startIndex;

		testSpace = endSpace - startSpace;
		
		if(testSpace >= newProc.size)
			foundIndex = j;
	}

	if(foundIndex != -1)
	{
		// Insert the process at this point, but adjust indices first
		newProc.startIndex = procList[foundIndex - 1].endIndex + 1;
		newProc.endIndex = newProc.startIndex + newProc.size - 1;

		insertInArray(newProc, procList, foundIndex, length);
	}
	else
	{
		// Need to check if the process can just be put at the end
		if((totalMemory - procList[length - 1].endIndex) >= newProc.size)
		{
			// Insert the process at the end
			procList[length].processName = (char*)malloc(NAME_SIZE * sizeof(char));
			strcpy(procList[length].processName, newProc.processName);
			procList[length].size = newProc.size;

			// Adjust indices
			procList[length].startIndex = procList[length - 1].endIndex + 1;
			procList[length].endIndex = procList[length].startIndex + procList[length].size - 1;

			foundIndex = length;
		}
		else
		{
			// Process cannot be inserted
			(*nextPtr) = -1;
			return -1;
		}
	}
	
	(*nextPtr) = foundIndex + 1;

	return foundIndex;
}
