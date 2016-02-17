#include <argp.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <argp.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


static char* doc = "CprE 489 Lab 3 - UDP Experiments";

// list of options supported
static struct argp_option options[] =
{
	{"source-IP", 's', "SOURCEIP", 0, "IP number of incoming traffic"},
	{"source-port", 'o', "SOURCEPORT", 0, "Port of incoming traffic"},
	{"dest-IP", 'd', "DESTIP", 0, "Which IP to reroute incoming traffic"},
    {"dest-port", 'p', "DESTPORT", 0, "Which port to send data to"},
    {"loss-rate", 'l', "LOSSRATE", 0, "What loss rate to impose on the traffic"}
	//{0}
};


/// argument structure to store the results of command line parsing
struct arguments
{
    char* source_IP;
    int source_port;
    char* dest_IP;
    int dest_port;
    int loss_rate;
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
        case 'l':
            arguments->loss_rate = atoi(arg);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/// The arg parser object
static struct argp argp = {&options, parse_opt, 0, &doc};

struct sockaddr_in addr, client_addr, server_addr;

int main(int argc, char* argv[]) {

    struct arguments *arguments = malloc(sizeof(*arguments));
    argp_parse(&argp, argc, argv, 0, 0, arguments);

    char buffer[1024];
    int rcv_len, fd, ClientAddrLen = sizeof(client_addr);
    int server_len = sizeof(server_addr);
    int packet_count = 0;

    addr.sin_family = AF_INET;
    addr.sin_port = arguments->source_port;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = arguments->dest_port;
    server_addr.sin_addr.s_addr = inet_addr(arguments->dest_IP);

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    int error = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if( error < 0 )
    {
        perror("Bind error");
    }

    listen(fd, 0);

    for( ; ; )
    {
        rcv_len = (int) recvfrom(fd, buffer, 1024, 0, (struct sockaddr *) &client_addr, &ClientAddrLen);
        if( rcv_len < 0 )
        {
            perror("Accept error");
        }
        if( client_addr.sin_addr.s_addr == inet_addr(arguments->source_IP) )
        {
            if( arguments->loss_rate == 0 )
            {
                error = (int) sendto(fd, buffer, 1024, 0, (struct sockaddr *) &server_addr, &server_len);
                if( error < 0 )
                {
                    perror("Write error");
                }
            }
            else if( arguments->loss_rate < 100 )
            {
                if( (rand() % arguments->loss_rate)
                    == (rand() % arguments->loss_rate) )
                {
                    error = (int) sendto(fd, buffer, 1024, 0, (struct sockaddr *) &server_addr, &server_len);
                    if( error < 0 )
                    {
                        perror("Write error");
                    }
                }
            }
        }
    }
}
