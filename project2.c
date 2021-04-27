#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2


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

void insertInArray(Process addProc, Process* array, int index);
void removeFromArray(Process* array, int index);

int firstFit(Process newProc, Process* procList, int length, unsigned long totalMemory);
int bestFit(Process newProc, Process* procList, int length, unsigned long totalMemory);
int worstFit(Process newProc, Process* procList, int length, unsigned long totalMemory);
int nextFit(Process newProc, Process* procList, int length, unsigned long totalMemory);


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

	// TESTING
	dprintf(STDERR, "Input command: %s\n", inCommand);
	dprintf(STDERR, "Total memory: %ld\n", totalMemory);
	dprintf(STDERR, "Script filename: %s\n", scriptName);

	// Allocate size to the array
	memory = (Process*)malloc(128 * sizeof(Process));
	
	// Open the file using a file pointer
	fp = fopen(scriptName, "r");

	char* line = (char*)malloc(64 * sizeof(char));
	char** data;

	char* command;
	char* processName;
	unsigned long numBytes;
	while(fscanf(fp, "%[^\n]", line) != EOF)
	{
		// Use fgetc to move past \n
		fgetc(fp);

		// TESTING
		dprintf(STDERR, "Line: %s\n", line);
		
		// Split the string along spaces
		data = split(line, ' ');

		int length = 0;
		while(data[length] != NULL)
			length++;

		// Decide what action to perform based off of data[0]
		// NOTE: If data[0] == #, then that line is a comment and we can ignore
		command = data[0];

		if(strcmp(command, "REQUEST") == 0)
		{
			// Allocate memory for a process
			// data[1] = process name, data[2] = num bytes
			processName = data[1];
			numBytes = strtoul(data[2], NULL, 0);

			// Create a new process
			Process newProc;
			newProc.processName = (char*)malloc(16 * sizeof(char));
			strcpy(newProc.processName, processName);
			newProc.size = numBytes;

			// Check what algorithm is being used from the command-line
			if(strcmp(inCommand, "FIRSTFIT") == 0)
			{
				// TESTING
				dprintf(STDERR, "REQUEST FIRSTFIT\n");
				
				if(firstFit(newProc, memory, procCount, totalMemory) == 0)
				{
					dprintf(STDOUT, "ALLOCATED %s %lu\n", memory[procCount].processName, memory[procCount].startIndex);
					procCount++;
				}
				else
					dprintf(STDOUT, "FAIL REQUEST %s %lu\n", newProc.processName, newProc.size);
			}
			else if(strcmp(inCommand, "BESTFIT") == 0)
			{
				// TESTING
				dprintf(STDERR, "REQUEST BESTFIT\n");	
				
				if(bestFit(newProc, memory, procCount, totalMemory) == 0)
				{
					dprintf(STDOUT, "ALLOCATED %s %lu\n", memory[procCount].processName, memory[procCount].startIndex);
					procCount++;
				}
				else
					dprintf(STDOUT, "FAIL REQUEST %s %lu\n", newProc.processName, newProc.size);
			}
			else if(strcmp(inCommand, "WORSTFIT") == 0)
			{

			}
			else if(strcmp(inCommand, "NEXTFIT") == 0)
			{

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
					// The right process has been found, need to remove it
					dprintf(STDOUT, "FREE %s %lu %lu\n", memory[i].processName, memory[i].size, memory[i].startIndex);
					
					removeFromArray(memory, i);
					procCount--;

					hasBeenFound++;
				}
			}

			// Check to make sure the process has been found, if it hasnt print an error message
			if(hasBeenFound == -1)
				dprintf(STDOUT, "FAIL RELEASE %s\n", processName);
		}
		else if(strcmp(command, "LIST") == 0)
		{
			// Get the second part of the command
			char* command2 = data[1];

			if(strcmp(command2, "ASSIGNED") == 0)
			{
				// Go through the list and print all assigned process memory
				if(procCount == 0)
					dprintf(STDOUT, "NONE\n");

				for(int i = 0; i < procCount; i++)
				{
					dprintf(STDOUT, "(%s, %lu, %lu)", procList[i].processName, procList[i].size, procList[i].startIndex);
					if(i == (procCount - 1))
						dprintf(STDOUT, "\n");
				}
				

			}
			else if(strcmp(command2, "AVAILABLE") == 0)
			{
				
			}		
		}
		else if(strcmp(command, "FIND") == 0)
		{
			// Find a process
			processName = data[1];
			
			int hasFound = -1;
			for(int i = 0; i < procCount; i++)
			{
				if(strcmp(processName, procList[i].processName) == 0)
				{
					// Print info about the process
					dprintf(STDOUT, "(%s, %lu, %lu)\n", procList[i].processName, procList[i].size, procList[i].startIndex);
					hasFound++;
				}
			}

			// If the process couldn't be found, print out FAULT
			if(hasFound == -1)
				dprintf(STDERR, "FAULT\n");
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

	char** stringArray = (char**)malloc((splitCount + 1) * sizeof(char*));
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
 * TODO: Get rid of new arrays, pass in procCount
 */
void insertInArray(Process addProc, Process* array, int index)
{
	int arrLength = sizeof(array) / sizeof(array[0]);

	// Create a new array to copy the elements into
	Process* newArr = (Process*)malloc((arrLength + 1) * sizeof(Process));

	// Copy the array
	Process tempProc;
	for(int i = 0; i < arrLength; i++)
	{
		if(i < index)
		{
			// Elements will be the same
			newArr[i] = array[i];
		}
		else if(i == index)
		{
			tempProc = array[i];
			newArr[i] = addProc;
		}
		else
		{
			newArr[i] = tempProc;
			tempProc = array[i];
		}
	}

	// Adjust the pointer to the provided array
	array = newArr;

	return;
}

/*
 * Removes an element from the given array
 */
void removeFromArray(Process* array, int index)
{
	int arrLength = sizeof(array) / sizeof(array[0]);

	Process* newArr = (Process*)malloc((arrLength - 1) * sizeof(Process));

	// Copy all array elements except for the indicated index
	for(int i = 0; i < arrLength; i++)
	{
		if(i < index)
			newArr[i] = array[i];
		else
			newArr[i] = array[i + 1];
	}

	// Make the provided array pointer point to the new array
	free(array);
	array = newArr;

	return;
}


/*
 * First Fit Algorithm:
 * Allocate the process to the first hole that is big enough
 * Return 0 on success, or 1 on error
 */
int firstFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{
	// TESTING
	dprintf(STDERR, "FIRST FIT: %s %lu, Length: %d\n", newProc.processName, newProc.size, length);

	// Check if this is the first process being added or not
	if(length == 0)
	{
		// Just need to put the process at the start of the list
		// Also need to make sure the process isn't bigger than the amount of memory
		if(newProc.size > totalMemory)
			return -1;
		procList[0].processName = (char*)malloc(16 * sizeof(char));
		strcpy(procList[0].processName, newProc.processName);
		procList[0].size = newProc.size;
		
		// Set the indices of the new process
		procList[0].startIndex = 0;
		procList[0].endIndex = procList[0].size - 1;

		return 0;
	}
	else
	{
		// Need to find the first big enough hole in the memory
		long startSpace = 0;
		long endSpace;

		int hasFoundPlace = -1;
		for(int i = 0; i < length; i++)
		{
			endSpace = procList[i].startIndex;

			if((endSpace - startSpace) >= newProc.size)
			{
				newProc.startIndex = startSpace;
				newProc.endIndex = startSpace + newProc.size;
				insertInArray(newProc, procList, i);
				hasFoundPlace++;
				break;
			}

			startSpace = procList[i].endIndex;
		}

		// Check if the process did not find a place
		if(hasFoundPlace == -1)
		{
			// Get the remaining memory after last process
			unsigned long remainingMem = totalMemory - procList[length - 1].endIndex;
			
			if(newProc.size > remainingMem)
			{
				// There is no place for this process
				return -1;
			}
			else
			{
				procList[length].processName = (char*)malloc(16 * sizeof(char));
				strcpy(procList[length].processName, newProc.processName);
				procList[length].size = newProc.size;

				procList[length].startIndex = procList[length - 1].endIndex + 1;
				procList[length].endIndex = procList[length].startIndex + procList[length].size - 1;
			}
		}
	}

	return 0;
}

/*
 * Best Fit Algorithm:
 * Find every memory hole and allocate to the smallest hole that is still big enough
 */
int bestFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{
	// If length is 0, can just add it to beginning
	if(length == 0)
	{
		// Check to make sure that the process' size is not bigger than the total memory
		if(newProc.size > totalMemory)
			return -1;
		procList[0].processName = (char*)malloc(16 * sizeof(char));
		strcpy(procList[0].processName, newProc.processName);
		procList[0].size = newProc.size;

		// Set indices of process
		procList[0].startIndex = 0;
		procList[0].endIndex = procList[0].size - 1;

		return 0;
	}

	// Loop through array to find smallest memory hole that is big enough for the new process
	int smallestIndex = -1;
	long smallestSize = 0;

	long startSpace = 0;
	long endSpace;
	for(int i = 0;  i < length; i++)
	{
		endSpace = procList[i].startIndex;

		if((endSpace - startSpace) >= newProc.size && (endSpace - startSpace) < smallestSize)
			smallestIndex = i;
	}
	
	// If an index was never found, return -1
	// TODO: Also check for end of array
	if(smallestIndex == -1)
	{
		// Check if the end of the array has enough size
		if((totalMemory - procList[length - 1].endIndex) >= newProc.size)
		{
			// Set the new process to the end of the list
			procList[length].processName = (char*)malloc(16 * sizeof(char));
			strcpy(procList[length].processName, newProc.processName);
			procList[length].size = newProc.size;

			// Set the indices
			procList[length].startIndex = procList[length - 1].endIndex + 1;
			procList[length].endIndex = procList[length].startIndex + procList[length].size - 1;

			return 0;
		}
		else
			return -1
	}
	
	// Insert into the array at smallestIndex
	insertInArray(newProc, procList, smallestIndex);

	return 0;
}

/*
 * Worst Fit Algorithm:
 * Find every memory hole and allocate to the biggest hole that is big enough
 * Very similar to bestFit, but looking for biggest hole instead of smalled
 */
int worstFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{
	if(length == 0)
	{
		if(newProc.size > totalMemory)
			return -1;
		procList[0].processName = (char*)
	}
}

/*
 * Next Fit Algorithm:
 * Very similar to FirstFit, but we keep a next pointer to the next element 
 */
int nextFit(Process newProc, Process* procList, int length, unsigned long totalMemory)
{

}
