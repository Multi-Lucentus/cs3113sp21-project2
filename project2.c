#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2


// Structs
struct Process
{
	char* processName;
	long size;
	long startIndex, endIndex;
};
typedef struct Proccess Process;


// Function Prototypes
char* readline(int fd);
char** split(char* str, char split);

void insertInArray(Process addProc, Process* array, int index);

void firstFit(Process newProc, Process* procList);
void bestFit(Process newProc, Process* procList);
void worstFit(Process newProc, Process* procList);
void nextFit(Process newProc, Process* procList);


// Main Function
int main(int argc, char** argv)
{
	// Variables
	int fd;

	char* inCommand, scriptFilename;
	unsigned long int totalMemory;

	Process* memory;
	int procCount = 0;

	// Check command-line arguments
	if(argc != 4)
	{
		dprintf(STDERR, "Illegal number of command-line arguments\n");
		exit(EXIT_FAILURE);
	}

	// Initialize variables
	memcpy(inCommand, argv[1]);
	memcpy(scriptFilename, argv[3]);
	totalMemory = atoi(argv[2]);

	// TESTING
	dprintf(STDERR, "Input command: %s\n", inCommand);
	dprintf(STDERR, "Total memory: %d\n", totalMemory);
	dprintf(STDERR, "Script filename: %s\n", scriptFilename);

	// Allocate size to the array
	memory = (Process*)malloc(64 * sizeof(Process));
	
	// Create the file descriptor of the file we are using
	fd = open(scriptFilename, O_RDONLY, 0);

	
	char* line;
	char* command;
	char* processName;
	long memAmount;

	char** data;
	int arrLength;
	
	while((line = readline(fd)) != NULL)
	{
		// Parse the line
		data = split(line, ' ');
	
		// Format should be Command, Process Name, and Amount of Memory if there are 3 elements
		arrLength = sizeof(data) / sizeof(data[0]);

		command = data[0];
		processName = data[1];

		if(arrLength == 3)
			memAmount = atoi(data[2]);

		// Check what command was issued

		// TODO: Need to check if it is a comment and then ignore
		if(strcmp(command, "REQUEST") == 0)
		{
			// Allocates memory for a process
			// Create a new process and initialize its data
			Process newProcess;
			strcpy(newProcess.processName, processName);
			newProcess.size = memAmount;
			
			// Print out information to STDOUT
			dprintf(STDOUT, "ALLOCATED %s %d\n", newProcess.processName, newProcess.size);

			// TODO: Determine where to put the process
			// Use the provided cmd line command to determine which algorithm to use
			if(strcmp(inCommand, "FIRSTFIT") == 0)
			{
				firstFit(newProcess, memory);
			}
			else if(strcmp(inCommand, "BESTFIT") == 0)
			{
				bestFit(newProcess, memory);
			}
			else if(strcmp(inCommand, "WORSTFIT") == 0)
			{
				worstFit(newProcess, memory);
			}
			else if(strcmp(inCommand, "NEXTFIT") == 0)
			{
				nextFit(newProcess, memory);
			}
			else
			{
				// Error
				dprintf(STDERR, "Illegal command provided in argv[1]\n");
				exit(EXIT_FAILURE);
			}

			procCount++;
		}
		else if(strcmp(command, "RELEASE") == 0)
		{
			// Releases memory held by a process
			// Loop through all processes until the one with the right process name is found
			for(int i = 0; i < procCount; i++)
			{
				if(strcmp(memory[i].name, processName) == 0)
				{

				}
			}

			procCount--;
		}
		else if(strcmp(command, "LIST") == 0)
		{
			// Two available commands: AVAILABLE or ASSIGNED
			// Second command is stored in processName variable
			if(strcmp(processName, "AVAILABLE") == 0)
			{

			}
			else if(strcmp(processName, "ASSIGNED") == 0)
			{
			
			}
			else
			{
				// Error
				dprintf(STDERR, "Illegal command after LIST command\n");
				exit(EXIT_FAILURE);
			}
		}
		else if(strcmp(command, "FIND") == 0)
		{
			// Locates the starting address and size allocated by the process name given

		}
		else
		{
			// Error - Illegal command
			dprintf(STDERR, "Illegal command in script file\n");
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}


// Function Declarations
/*
 * Reads a line from the file pointed to by the file descriptor
 * Returns the line read
 */
char* readline(int fd)
{
	char* buffer = (char*)malloc(64 * sizeof(char));
	int bufCount = 0;

	char c;
	while(read(fd, &c, 1) > 0)
	{
		// Check if the character is newline (\n)
		if(c != '\n')
		{
			buffer[bufCount] = c;
			bufCount++;
		}
		else
		{
			buffer[bufCount] = '\0';
			break;
		}
	}

	return buffer;
}

/*
 * Splits a string according to the split character parameter
 * Returns an array of strings
 */
char** split(char* str, char split)
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
 */
void insertInArray(Process addProc, Process* array, int index)
{
	int arrLength = sizeof(array) / sizeof(array[0]);

	// TODO: Could use realloc?
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
}


/*
 * First Fit Algorithm:
 * Allocate the process to the first hole that is big enough
 */
void firstFit(Process newProc, Process* procList)
{
	// First, get the length of the process list
	int procCount = sizeof(procList) / sizeof(procList[0]);

	if(procCount == 0)
	{
		// Just need to put the process at the start of the list
		procCount[0] = newProc;
		
		// Set the indices of the new process
		procCount[0].startIndex = 0;
		procCount[0].endIndex = size - 1;
	}
	else
	{
		// Need to find the first big enough hole in the memory
		long startSpace = 0;
		long endSpace;

		for(int i = 0; i < procCount; i++)
		{
			endSpace = procList[i].startIndex;

			if((endSpace - startSpace) >= newProc.size)
			{
				
			}

			startSpace = procList[i].endIndex;
		}
	}
}
