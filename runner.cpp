#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <string.h>

#include <string>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		return -1;
	}

	int crtarg = 1; // skip over program name
	std::string command = "";
	command = command + argv[crtarg]; // append the program to run
	crtarg++;

	while (crtarg < argc)
		command = command + " " + argv[crtarg++];

	//printf("%s\n", command.c_str());

	int retcode = system(command.c_str());
	
	int status = 0;
	if (WIFEXITED(retcode))
	{
		status = WEXITSTATUS(retcode);
	}
	else status = -1;
	
	printf("Process has finished. Exit code %d.\n", status);

	return 0;
}
