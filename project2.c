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

Process* firstFit();
Process* bestFit();
Process* worstFit();
Process* nextFit();


// Main Function
int main(int argc, char** argv)
{
	// Variables
	int fd;

	char* inCommand, scriptFilename;
	unsigned long int totalMemory;

	Process* memory;
	int memCount = 0;

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
	memory = (Process*)malloc( * sizeof(Process));
	
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
		if(strcmp(command, "REQUEST") == 0)
		{
			// Allocates memory for a process
			
		}
		else if(strcmp(command, "RELEASE") == 0)
		{

		}
		else if(strcmp(command, "LIST") == 0)
		{

		}
		else if(strcmp(command, "FIND") == 0)
		{

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
