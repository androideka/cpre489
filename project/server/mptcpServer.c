#include <argp.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


static char* doc = "CprE 489 Final Project - MPTCP Server";

// list of options supported
static struct argp_option options[] =
{
	{"source-IP", 's', "SOURCEIP", 0, "IP number of incoming traffic"},
	{"source-port", 'p', "SOURCEPORT", 0, "Port of incoming traffic"},
	//{0}
};


/// argument structure to store the results of command line parsing
struct arguments
{
    char* source_IP;
    int source_port;
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
        case 'p':
            arguments->source_port = atoi(arg);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/// The arg parser object
static struct argp argp = {&options, parse_opt, 0, &doc};

struct sockaddr_in server, client, sub1, sub2, sub3;
typedef struct {
    uint16_t sn;
    uint8_t data[5];
} packet_t;

typedef struct {
    uint16_t sn;
} ack_t;


int main(int argc, char** argv) {

    struct arguments *arguments = malloc(sizeof(*arguments));
    argp_parse(&argp, argc, argv, 0, 0, arguments);

    char buffer[16];
    int control_sock, fd_server, client_len = sizeof(client);

    server.sin_family = AF_INET;
    server.sin_port = htons(arguments->source_port);
    server.sin_addr.s_addr = inet_addr(arguments->source_IP);

    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    int err = bind(fd_server, (struct sockaddr*) &server, sizeof(server));
    if( err < 0 )
    {
        perror("Control bind error");
    }

    listen(fd_server, 0);

    control_sock = accept(fd_server, (struct sockaddr*) &client, (socklen_t*) &client_len);
    if( control_sock < 0 )
    {
        perror("Accept error");
    }

    for( ; ; )
    {
        memset(buffer, 0, sizeof(buffer));
        err = (int) read(control_sock, buffer, sizeof(buffer));
        if( err < 0 )
        {
            perror("Read error");
        }

        err = (int) write(control_sock, &buffer, sizeof(buffer));
    }

}
