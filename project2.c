#include <stdio.h>
#include <stdlib.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2



char* readline(int fd);
char** split(char* str, char split);


int main(int argc, char** argv)
{
	// Variables
	char* inCommand, scriptFilename;
	int totalMemory;


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

	
	exit(EXIT_SUCCESS);
}
