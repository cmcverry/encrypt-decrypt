#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// Generates of string of random uppercase characters and the space character of size length parameter
void generateKey(int length)
{
	int size = length + 1;
	// Initializaes empty string
	char key[size];
	memset(key, '\0', size);

	// Creates seed for rand()
	srand(time(0));
	int i = 0;
	// Loops through string indices, randomly assigning character values
	for(i;i < length; i++)
	{
		// random int values range from 0 - 26
		int randomNum = rand() % 27;

		// if random value is 26, assigns space character to string at index i
		if (randomNum == 26)
		{
			key[i] = ' ';
		}
		// else assigns random uppercase character
		else
		{
			key[i] = 'A' + randomNum;
		}
	}
	// Outputs string with new line character to stdout
	printf("%s\n", key);
}


int main(int argc, char* argv[])
{

	int length = atoi(argv[1]);
	generateKey(length);
		
	return EXIT_SUCCESS;
}