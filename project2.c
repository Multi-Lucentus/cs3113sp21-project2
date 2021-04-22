#include <stdio.h>
#include <stdlib.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2


// Structs
struct Process
{
	char* processName;
	long size;
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
	char* inCommand, scriptFilename;
	unsigned long int totalMemory;

	Process* memory;

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
	memory = (Process*)malloc(totalMemory * sizeof(Process));
	
	// Check to see what command was issued
	if(strcmp(inCommand, "BESTFIT") == 0)
	{

	}
	else if(strcmp(inCommand, "FIRSTFIT") == 0)
	{

	}
	else if(strcmp(inCommand, "NEXTFIT") == 0)
	{

	}
	else if(strcmp(inCommand, "WORSTFIT") == 0)
	{

	}
	else
	{
		// Error
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
