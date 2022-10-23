/**
 * File: writer.c
 * Author: RJ Buxton
 *
 * This application writes the string argv[2] to the file argv[1].
 *
 * Build Native:
 * $ make clean
 * $ make
 *
 * Build with ARM Cross Compiler
 * $ make clean
 * $ make CROSS_COMPILER=aarch64-none-linux-gnu-
 *
 * Usage:
 * $ ./writer path/to/some/file.txt "Hello world!"
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char** argv)
{
	/* Check input args */
	if(argc != 3)
	{
		syslog(LOG_USER | LOG_ERR, "Invalid input arguments");
		return 1;
	}

	syslog(LOG_USER | LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);

	int fd = open(argv[1], O_RDWR | O_CREAT, 
			S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

	/* Check for open error */
	if(fd == -1)
	{
		syslog(LOG_USER | LOG_ERR, "Error opening %s", argv[1]);
		return 1;
	}

	write(fd, argv[2], strlen(argv[2]));
	write(fd, "\n", 1);

	close(fd);

	return 0;
}
