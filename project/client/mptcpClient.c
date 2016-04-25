#include <argp.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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

	unlink("pipe1");
	unlink("pipe2");
	unlink("pipe3");

	struct sockaddr_in in_socks[3];
	in_socks[0] = subflow_1;
	in_socks[1] = subflow_2;
	in_socks[2] = subflow_3;

	struct sockaddr_un un_socks[3];
	un_socks[0] = pipe1;
	un_socks[1] = pipe2;
	un_socks[2] = pipe3;

	u_fd_s1 = socket(AF_UNIX, SOCK_STREAM, 0);
	u_fd_s2 = socket(AF_UNIX, SOCK_STREAM, 0);
	u_fd_s3 = socket(AF_UNIX, SOCK_STREAM, 0);

	strncpy(pipe1.sun_path, "pipe1", sizeof(pipe1.sun_path) - 1);
	strncpy(pipe2.sun_path, "pipe2", sizeof(pipe2.sun_path) - 1);
	strncpy(pipe3.sun_path, "pipe3", sizeof(pipe3.sun_path) - 1);

	for( i = 0; i < 3; i++ )
	{
		un_socks[i].sun_family = AF_UNIX;
		in_socks[i].sin_family = AF_INET;
		in_socks[i].sin_port = htons(++arguments->dest_port);
		in_socks[i].sin_addr.s_addr = inet_addr(arguments->dest_IP);
	}

	bind(u_fd_s1, (struct sockaddr*) &pipe1, sizeof(pipe1));
	bind(u_fd_s2, (struct sockaddr*) &pipe2, sizeof(pipe2));
	bind(u_fd_s3, (struct sockaddr*) &pipe3, sizeof(pipe3));

	d1 = socket(AF_INET, SOCK_STREAM, 0);
	d2 = socket(AF_INET, SOCK_STREAM, 0);
	d3 = socket(AF_INET, SOCK_STREAM, 0);
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

	err = connect(d1, (struct sockaddr*) &in_socks[0], sizeof(in_socks[0]));
	if( err < 0 )
	{
		perror("Connect d1 error");
	}

	err = connect(d2, (struct sockaddr*) &in_socks[1], sizeof(in_socks[1]));
	if( err < 0 )
	{
		perror("Connect d2 error");
	}

	err = connect(d3, (struct sockaddr*) &in_socks[2], sizeof(in_socks[2]));
	if( err < 0 )
	{
		perror("Connect d3 error");
	}

	printf("Successfully connected to server.\n");

	if(err < 0)
	{
		perror("Something bad happened along the way.");
	}

	i = 0;

	pid_t first_child, second_child, third_child;
	first_child = fork();

	if( first_child > 0 )
	{
		// Parent process
		second_child = fork();
		if( second_child > 0 )
		{
			// Parent process
			for( i = 0; i < 248; i++ )
			{
				char byte[4];
				memcpy( byte, data[i*4], 4 );
			}
		}
		else if ( second_child < 0 )
		{
			// Error
			perror("Fork failed.");
		}
		else
		{
			// Second child
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
		}
		else if ( third_child < 0 )
		{
			// Error
			perror("Uh oh. shouldn't be here.");
		}
		else
		{
			// Third child
		}
	}



	for( i = 0; i < 248; i++ )
	{
		if( pid0 > 0 )
		{
			// Parent process
			DSS_t* data_seq_sig = malloc(sizeof(DSS_t));
			data_seq_sig->sub_seq_num = (uint32_t) (i % 3);
			//printf("%d\n", data_seq_sig->sub_seq_num);
			// Write to fd_control

		}
		else
		{
			// Children/subflows
			// write to d#
		}
	}

	return 0;
}
