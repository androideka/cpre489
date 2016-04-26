#include <argp.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../DSS.h"


static char* doc = "CprE 489 Project - Multi-Path TCP";

// list of options supported
static struct argp_option options[] =
{
	{"source-IP", 's', "SOURCEIP", 0, "IP number of incoming traffic"},
	{"source-port", 'o', "SOURCEPORT", 0, "Port of incoming traffic"},
	{"dest-IP", 'd', "DESTIP", 0, "Which IP to reroute incoming traffic"},
	{"dest-port", 'p', "DESTPORT", 0, "Which port to send data to"},
	//{0}
};


/// argument structure to store the results of command line parsing
struct arguments
{
    char* source_IP;
    int source_port;
    char* dest_IP;
    int dest_port;
};


/**
 * @brief     Callback to parse a command line argument
 * @param     key
 *                 The short code key of this argument
 * @param     arg
 *                 The argument following the code
 * @param     state
 *                 The state of the arg parser state machine
 * @return    0 if successfully handled the key, ARGP_ERR_UNKNOWN if the key was unknown
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct arguments *arguments = state->input;
    switch(key)
    {
        case 's':
            arguments->source_IP= arg;
            break;
        case 'o':
            arguments->source_port = atoi(arg);
            break;
        case 'd':
            arguments->dest_IP = arg;
            break;
        case 'p':
            arguments->dest_port = atoi(arg);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/// The arg parser object
static struct argp argp = {&options, parse_opt, 0, &doc};

typedef struct {
	uint16_t sn;
	uint8_t data[5];
} packet_t;

typedef struct {
	uint16_t sn;
} ack_t;

struct sockaddr_in control_client, control_server, subflow_1, subflow_2, subflow_3;
struct sockaddr_un pipe1, pipe2, pipe3;

int main(int argc, char** argv)
{

	struct arguments *arguments = malloc(sizeof(*arguments));
        argp_parse(&argp, argc, argv, 0, 0, arguments);

	int err = 0;
	int fd_control, i, u_fd_s1, u_fd_s2, u_fd_s3, d1, d2, d3;
	socklen_t control_len = sizeof(control_client);
	socklen_t subflow_len = sizeof(subflow_1);
	char data[992] = "";
	int dif1, dif2, dif3, a, b, c;

	char repeat[62] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for( i = 0; i < 16; i++ )
	{
		strcat(data, repeat);
	}

	control_client.sin_family = AF_INET;
	control_client.sin_port = htons(arguments->source_port);
	control_client.sin_addr.s_addr = inet_addr(arguments->source_IP);

	control_server.sin_family = AF_INET;
	control_server.sin_port = htons(arguments->dest_port);
	control_server.sin_addr.s_addr = inet_addr(arguments->dest_IP);

	int u_fds[3] = { u_fd_s1, u_fd_s2, u_fd_s3 };

	struct sockaddr_in in_socks[3] = { subflow_1, subflow_2, subflow_3 };
	struct sockaddr_un un_socks[3] = { pipe1, pipe2, pipe3 };

	d1 = socket(AF_INET, SOCK_STREAM, 0);
	d2 = socket(AF_INET, SOCK_STREAM, 0);
	d3 = socket(AF_INET, SOCK_STREAM, 0);

	u_fd_s1 = socket(AF_UNIX, SOCK_STREAM, 0);
	u_fd_s2 = socket(AF_UNIX, SOCK_STREAM, 0);
	u_fd_s3 = socket(AF_UNIX, SOCK_STREAM, 0);

	dif1 = socket(AF_UNIX, SOCK_STREAM, 0);
	dif2 = socket(AF_UNIX, SOCK_STREAM, 0);
	dif3 = socket(AF_UNIX, SOCK_STREAM, 0);

	subflow_1.sin_family = AF_INET;
	subflow_1.sin_port = htons(++arguments->dest_port);
	subflow_1.sin_addr.s_addr = inet_addr(arguments->dest_IP);

	subflow_2.sin_family = AF_INET;
	subflow_2.sin_port = htons(++arguments->dest_port);
	subflow_2.sin_addr.s_addr = inet_addr(arguments->dest_IP);

	subflow_3.sin_family = AF_INET;
	subflow_3.sin_port = htons(++arguments->dest_port);
	subflow_3.sin_addr.s_addr = inet_addr(arguments->dest_IP);

	memset(&pipe1, 0, sizeof(pipe1));
	memset(&pipe2, 0, sizeof(pipe2));
	memset(&pipe3, 0, sizeof(pipe3));

	pipe1.sun_family = AF_UNIX;
	pipe2.sun_family = AF_UNIX;
	pipe3.sun_family = AF_UNIX;

	socklen_t pipe_len = sizeof(pipe1);

	char path1[] = "pipe1";
	char path2[] = "pipe2";
	char path3[] = "pipe3";

	mkdir(path1, 077);
	mkdir(path2, 077);
	mkdir(path3, 077);

	strcat(path1, "/u_fd_s1");
	strcat(path2, "/u_fd_s2");
	strcat(path3, "/u_fd_s3");

	strcpy(pipe1.sun_path, path1);
	strcpy(pipe2.sun_path, path2);
	strcpy(pipe3.sun_path, path3);

	unlink("pipe1/u_fd_s1");
	unlink("pipe2/u_fd_s2");
	unlink("pipe3/u_fd_s3");

	//strncpy(pipe1.sun_path, "pipe1", sizeof(pipe1.sun_path) - 1);
	//strncpy(pipe2.sun_path, "pipe2", sizeof(pipe2.sun_path) - 1);
	//strncpy(pipe3.sun_path, "pipe3", sizeof(pipe3.sun_path) - 1);

	err = bind(u_fd_s1, (struct sockaddr*) &pipe1, (strlen(pipe1.sun_path) + sizeof(pipe1.sun_family)));
	if( err < 0 )
	{
		perror("Pipe 1 bind error");
	}

	err = bind(u_fd_s2, (struct sockaddr*) &pipe2, (strlen(pipe2.sun_path) + sizeof(pipe2.sun_family)));
	if( err < 0 )
	{
		perror("Pipe 2 bind error");
	}

	err = bind(u_fd_s3, (struct sockaddr*) &pipe3, (strlen(pipe3.sun_path) + sizeof(pipe3.sun_family)));

	if( err < 0 )
	{
		perror("Pipe 3 bind error");
	}

	chmod(path1, 0777);
	chmod(path2, 0777);
	chmod(path3, 0777);

	err = listen(u_fd_s1, 0);
	if ( err < 0 )
	{
		perror("Listen error");
	}
	err = listen(u_fd_s2, 0);
	if ( err < 0 )
	{
		perror("Listen error");
	}
	err = listen(u_fd_s3, 0);
	if ( err < 0 )
	{
		perror("Listen error");
	}

	fd_control = socket(AF_INET, SOCK_STREAM, 0);
	if( fd_control < 0 )
	{
		perror("Socket error");
	}

	err = connect(fd_control, (struct sockaddr*) &control_server, sizeof(control_server));
	if( err < 0 )
	{
		perror("Connect control error");
	}

	err = connect(d1, (struct sockaddr*) &subflow_1, sizeof(subflow_1));
	if( err < 0 )
	{
		perror("Connect d1 error");
	}

	err = connect(d2, (struct sockaddr*) &subflow_2, sizeof(subflow_2));
	if( err < 0 )
	{
		perror("Connect d2 error");
	}

	err = connect(d3, (struct sockaddr*) &subflow_3, sizeof(subflow_3));
	if( err < 0 )
	{
		perror("Connect d3 error");
	}
	else
	{
		printf("Successfully connected control and subflows to server.\n");
	}

	i = 0;

	pid_t first_child, second_child, third_child;
	first_child = fork();

	printf("Hello\n");

	if( first_child > 0 )
	{
		second_child = fork();
		if( second_child > 0 )
		{
			// Parent process
			err = connect(dif1, (struct sockaddr*) &pipe1, sizeof(pipe1));
			if( err < 0 )
			{
				perror("Unix socket 1 connect error");
			}

			err = connect(dif2, (struct sockaddr*) &pipe2, sizeof(pipe2));
			if( err < 0 )
			{
				perror("Unix socket 2 connect error");
			}

			err = connect(dif3, (struct sockaddr*) &pipe3, sizeof(pipe3));
			if( err < 0 )
			{
				perror("Unix socket 3 connect error");
			}


			for( i = 0; i < 248; i++ )
			{
				char byte[4];
				strncpy( byte, data + i*4, 4 );
				if( i % 3 == 0 )
				{
					// Subflow 1
					printf("Writing 4 bytes to subflow 1\n");
					err = write(dif1, byte, sizeof(byte));
					if ( err < 0 )
					{
						perror("Unix socket 1 write error");
					}
				}
				else if ( i % 3 == 1 )
				{
					// Subflow 2
					printf("Writing 4 bytes to subflow 2\n");
					err = write(dif2, byte, sizeof(byte));
					if ( err < 0 )
					{
						perror("Unix socket 2 write error");
					}
				}
				else
				{
					// Subflow 3
					printf("Writing 4 bytes to subflow 3\n");
					err = write(dif3, byte, sizeof(byte));
					if ( err < 0 )
					{
						perror("Unix socket 3 write error");
					}
				}

				DSS_t* dss;
				dss = malloc(sizeof(DSS_t));
				dss->sub_seq_num = i % 3;
				dss->data_length = 4;
				dss->data_seq_num = i*4;

				printf("Sending DSS for packet sent on %d with sequence number %d\n", dss->sub_seq_num, dss->data_seq_num);

				err = write(fd_control, &dss, sizeof(DSS_t));
				if( err < 0 )
				{
					perror("Control write error");
				}
			}
			sleep(5);
		}
		else if ( second_child < 0 )
		{
			// Error
			perror("Fork failed.");
		}
		else
		{
			// Second child
			char buffer[4];
			a = accept(u_fd_s2, (struct sockaddr*) &pipe2, &pipe_len);
			for( ; ; )
			{
				printf("Accepted connection from 2.\n");
				if( a < 0 )
				{
					perror("Accept 2 error");
				}

				err = read(a, buffer, 4);
				if( err < 0 )
				{
					perror("Read 2 error");
				}

				printf("Bytes from 2: %s\n", buffer);

				err = write(d2, buffer, 4);
				if( err < 0 )
				{
					perror("Write 2 error");
				}
			}
		}
	}
	else if ( first_child < 0 )
	{
		// Error
		perror("Something went wrong.");
	}
	else
	{
		third_child = fork();
		if( third_child > 0 )
		{
			// First child
			char buffer[4];
			b = accept(u_fd_s1, (struct sockaddr*) &pipe1, &pipe_len);
			for( ; ; )
			{
				printf("Accepted connection from 1.\n");
				if( b < 0 )
				{
					perror("Accept 1 error");
				}

				err = read(b, buffer, 4);
				if( err < 0 )
				{
					perror("Read 1 error");
				}

				printf("Bytes from 1: %s\n", buffer);

				err = write(d1, buffer, 4);
				if( err < 0 )
				{
					perror("Write 1 error");
				}
			}
		}
		else if ( third_child < 0 )
		{
			// Error
			perror("Uh oh. shouldn't be here.");
		}
		else
		{
			// Third child
			char buffer[4];
			c = accept(u_fd_s3, (struct sockaddr*) &pipe3, &pipe_len);
			for( ; ; )
			{
				printf("Accepted connection from 3.\n");
				if( c < 0 )
				{
					perror("Accept error");
				}

				err = read(c, buffer, 4);
				if( err < 0 )
				{
					perror("Read 3 error");
				}

				printf("Bytes from 3: %s\n", buffer);

				err = write(d3, buffer, 4);
				if( err < 0 )
				{
					perror("Write 3 error");
				}
			}
		}
	}

	return 42;
}
