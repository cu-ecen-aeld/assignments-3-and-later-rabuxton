/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

#define PORT "9000"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

#define TMP_LOG_FILE	"/var/tmp/aesdsocketdata"

int fd_file = 0;
int fd_listen = 0;
int fd_conn = 0;

void sigint_handler(int s)
{
	printf("Caught signal, exiting\n");
	syslog(LOG_USER | LOG_INFO, "Caught signal, exiting");
	if(fcntl(fd_file, F_GETFD)!=-1) close(fd_file);
	if(fcntl(fd_listen, F_GETFD)!=-1) close(fd_listen);
	if(fcntl(fd_conn, F_GETFD)!=-1) close(fd_conn);
	char cmd[80];
	sprintf(cmd, "rm -rf %s", TMP_LOG_FILE);
	if(system(cmd) != 0)
	{
		printf("Error deleting tmp file\n");
		exit(EXIT_FAILURE);
	}
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	struct addrinfo hints, *servinfo;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	int rv;
	int yes=1;

	// Configure the signal handler
	if (signal (SIGINT | SIGTERM, sigint_handler) == SIG_ERR) 
	{ 
		fprintf (stderr, "Cannot handle SIGINT!\n"); 
		exit (EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	// get address info
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}

	// create the socket
	if ((fd_listen = socket(servinfo->ai_family, servinfo->ai_socktype,
		servinfo->ai_protocol)) == -1) {
		perror("server: socket");
		exit(1);
	}

	// Set the socket options
	if (setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &yes,
			sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	// bind the socket
	if (bind(fd_listen, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		close(fd_listen);
		perror("server: bind");
		exit(1);
	}

	freeaddrinfo(servinfo); // all done with this structure

	// listen
	if (listen(fd_listen, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	if(argc > 1)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			printf("Running server as daemon process\n");
			fflush(stdout);
			if(fork()!=0) exit(0);

			if(setsid()==-1) 
			{
				perror("setsid");
				exit(1);
			} 
		}
	}

	while(1)
	{
		printf("server: waiting for connection...\n");

		sin_size = sizeof their_addr;
		fd_conn = accept(fd_listen, (struct sockaddr *)&their_addr, &sin_size);
		if (fd_conn == -1) {
			perror("accept");
			exit(1);
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		syslog(LOG_USER | LOG_INFO, "Accepted connection from %s", s);

		fd_file = open(TMP_LOG_FILE, O_RDWR | O_CREAT | O_APPEND, 
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH); 

		/* Check for open error */
		if(fd_file == -1)
		{
			syslog(LOG_USER | LOG_ERR, "Error opening %s", TMP_LOG_FILE);
			return 1;
		}

		char *buf = malloc(1);
		int i = 0;
		while(1)
		{
			buf = (char *) realloc(buf, i+2);
			if(recv(fd_conn, &buf[i], 1, 0) == -1)
			{
				printf("Recv error\n");
				free(buf);
				exit(EXIT_FAILURE);
			}
			
			//printf("%c",buf[i]);

			if(buf[i]=='\n')
			{
				write(fd_file, buf, i+1);
				break;
			}
			
			i++;
		}

		free(buf);

		/* Read file from beginning and send to client */
		char c;
		i = 0;
		while(pread(fd_file, &c, 1, i) != 0)
		{
			if (send(fd_conn, &c, 1, 0) == -1)
			{
				perror("send");
				exit(EXIT_FAILURE);
			}

			i++;
		}

		close(fd_file);

		close(fd_conn);
		syslog(LOG_USER | LOG_INFO, "Closed connection from %s", s);
	}

	return 0;
}
