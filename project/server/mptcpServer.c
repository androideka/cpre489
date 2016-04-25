#include <argp.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "../DSS.h"


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
struct sockaddr_un pipe1, pipe2, pipe3;

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
    int control_sock, fd_server, client_len = sizeof(client), sub_len = sizeof(sub1);
    int u_fd_s1, u_fd_s2, u_fd_s3, d1, d2, d3, d1_sock, d2_sock, d3_sock;

    server.sin_family = AF_INET;
    server.sin_port = htons(arguments->source_port);
    server.sin_addr.s_addr = inet_addr(arguments->source_IP);

    sub1.sin_family = AF_INET;
    sub1.sin_port = htons(++arguments->source_port);
    sub1.sin_addr.s_addr = inet_addr(arguments->source_IP);

    sub2.sin_family = AF_INET;
    sub2.sin_port = htons(++arguments->source_port);
    sub2.sin_addr.s_addr = inet_addr(arguments->source_IP);

    sub3.sin_family = AF_INET;
    sub3.sin_port = htons(++arguments->source_port);
    sub3.sin_addr.s_addr = inet_addr(arguments->source_IP);

    unlink("pipe1");
    unlink("pipe2");
    unlink("pipe3");

    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    int err = bind(fd_server, (struct sockaddr*) &server, sizeof(server));
    if( err < 0 )
    {
        perror("Control bind error");
    }

    d1 = socket(AF_INET, SOCK_STREAM, 0);
    d2 = socket(AF_INET, SOCK_STREAM, 0);
    d3 = socket(AF_INET, SOCK_STREAM, 0);

    err = bind(d1, (struct sockaddr*) &sub1, sizeof(sub1));
    if( err < 0 )
    {
        perror("d1 bind error");
    }

    err = bind(d2, (struct sockaddr*) &sub2, sizeof(sub2));
    if( err < 0 )
    {
        perror("d2 bind error");
    }

    err = bind(d3, (struct sockaddr*) &sub3, sizeof(sub3));
    if( err < 0 )
    {
        perror("d3 bind error");
    }

    u_fd_s1 = socket(AF_UNIX, SOCK_STREAM, 0);
    u_fd_s2 = socket(AF_UNIX, SOCK_STREAM, 0);
    u_fd_s3 = socket(AF_UNIX, SOCK_STREAM, 0);

    pipe1.sun_family = AF_UNIX;
    pipe2.sun_family = AF_UNIX;
    pipe3.sun_family = AF_UNIX;

    strncpy(pipe1.sun_path, "pipe1", sizeof(pipe1.sun_path) - 1);
    strncpy(pipe2.sun_path, "pipe2", sizeof(pipe2.sun_path) - 1);
    strncpy(pipe3.sun_path, "pipe3", sizeof(pipe3.sun_path) - 1);

    bind(u_fd_s1, (struct sockaddr*) &pipe1, sizeof(pipe1));
    bind(u_fd_s2, (struct sockaddr*) &pipe2, sizeof(pipe2));
    bind(u_fd_s3, (struct sockaddr*) &pipe3, sizeof(pipe3));

    listen(fd_server, 0);
    listen(d1, 0);
    listen(d2, 0);
    listen(d3, 0);

    control_sock = accept(fd_server, (struct sockaddr*) &client, (socklen_t*) &client_len);
    if( control_sock < 0 )
    {
        perror("Control accept error");
    }

    d1_sock = accept(d1, (struct sockaddr*) &sub1, (socklen_t*) &sub_len);
    if( d1_sock < 0 )
    {
        perror("d1 accept error");
    }

    d2_sock = accept(d2, (struct sockaddr*) &sub2, (socklen_t*) &sub_len);
    if( d2_sock < 0 )
    {
        perror("d2 accept error");
    }

    d3_sock = accept(d3, (struct sockaddr*) &sub3, (socklen_t*) &sub_len);
    if( d3_sock < 0 )
    {
        perror("d3 accept error");
    }


    pid_t first_child, second_child, third_child;
    first_child = fork();

    if( first_child > 0 )
    {
        // Parent process
        second_child = fork();
        if( second_child > 0 )
        {
            // Parent process
        }
        else if ( second_child < 0 )
        {
            // Error
        }
        else
        {
            // Second child
        }
    }
    else if ( first_child < 0 )
    {
        // Error
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
        }
        else
        {
            // Third child
        }
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
