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
#include <unistd.h>


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


int main(int argc, char** argv)
{

	struct arguments *arguments = malloc(sizeof(*arguments));
        argp_parse(&argp, argc, argv, 0, 0, arguments);

	char** buffer = malloc(6 * sizeof(char*));
	
	int err = 0;
	int fd_control, i;
	socklen_t control_len = sizeof(control_client);
	socklen_t subflow_len = sizeof(subflow_1);

	for( i = 0; i < 16; i++ )
	{
		
	}

	control_client.sin_family = AF_INET;
	control_client.sin_port = htons(arguments->source_port);
	control_client.sin_addr.s_addr = inet_addr(arguments->source_IP);

	control_server.sin_family = AF_INET;
	control_server.sin_port = htons(arguments->dest_port);
	control_server.sin_addr.s_addr = inet_addr(arguments->dest_IP);

	fd_control = socket(AF_INET, SOCK_STREAM, 0);
	if( fd_control < 0 )
	{
		perror("Socket error");
	}

	err = connect(fd_control, (struct sockaddr*) &control_server, sizeof(control_server));
	if( err < 0 )
	{
		perror("Connect error");
	}

	printf("Successfully connected to server.\n");
	return 0;

	ack_t* ack;
	ack = malloc(sizeof(ack_t));
	ack_t* last_ack;
	last_ack = malloc(sizeof(ack_t));
	
	for( i = 0; i < 16; i++ )
	{
		memset(buffer, 0, sizeof(buffer));

		err = write(fd_control, &buffer, sizeof(buffer));
		if( err < 0 )
		{
			perror("Write error");
		}

		err = read(fd_control, &buffer, sizeof(buffer));
		if( err < 0 )
		{
			perror("Read error");
		}
	}

	close(fd_control);

	return 0;

}
