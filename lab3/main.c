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

struct sockaddr_in addr;
struct sockaddr_in client_addr, server_addr;

int main(int argc, char* argv[]) {

    struct arguments *arguments = malloc(sizeof(*arguments));
    argp_parse(&argp, argc, argv, 0, 0, arguments);

<<<<<<< HEAD
    char buffer[2048];
    size_t len = sizeof(buffer);
    int rcv_len, fd, ClientAddrLen = sizeof(client_addr);
    int server_len = sizeof(server_addr);
    int packet_count = 0;

    addr.sin_family = AF_INET;
    addr.sin_port = arguments->source_port;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");//"192.168.1.104");
=======
    printf("%s, %s, %d, %d\n", arguments->source_IP, arguments->dest_IP, arguments->source_port, arguments->dest_port);

    char buffer[2048];
    int rcv_len, fd;
    socklen_t client_len = sizeof(client_addr);
    socklen_t server_len = sizeof(server_addr);
    socklen_t addr_len = sizeof(addr);
    int s, packet_count = 0;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(arguments->source_port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
>>>>>>> fb9980d1f6ca62f949d7488026b39b77b19e3fe8

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(arguments->dest_port);
    server_addr.sin_addr.s_addr = inet_addr(arguments->dest_IP);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if( fd < 0 )
    {
	perror("Socket error");
    }

    int error = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr));
    if( error < 0 )
    {
        perror("Bind error");
    }

    for( ; ; )
    {
<<<<<<< HEAD
	printf("Waiting for messages...\n");
        rcv_len = (int) recvfrom(fd, buffer, len, 0, (struct sockaddr *) &client_addr, &ClientAddrLen);
	printf("Message received\n");
=======
	printf("Waiting on port %d...\n", arguments->source_port);
	size_t len = sizeof(buffer);
        rcv_len = recvfrom(fd, buffer, len, 0, (struct sockaddr *) &client_addr, &client_len);
	printf("Packet received\n");
>>>>>>> fb9980d1f6ca62f949d7488026b39b77b19e3fe8
        if( rcv_len < 0 )
        {
            perror("Accept error");
        }
        if( client_addr.sin_addr.s_addr == inet_addr(arguments->source_IP) )
        {
            if( arguments->loss_rate == 0 )
            {
<<<<<<< HEAD
		printf("Sending UDP packet\n");
                error = (int) sendto(fd, buffer, len, 0, (struct sockaddr *) &server_addr, &server_len);
=======
                error = sendto(fd, buffer, len, 0, (struct sockaddr *) &server_addr, server_len);
>>>>>>> fb9980d1f6ca62f949d7488026b39b77b19e3fe8
                if( error < 0 )
                {
                    perror("Write error");
                }
            }
            else if( arguments->loss_rate < 100 )
            {
                if( (rand() % 100) > arguments->loss_rate )
                {
<<<<<<< HEAD
                    error = (int) sendto(fd, buffer, len, 0, (struct sockaddr *) &server_addr, &server_len);
=======
                    error = sendto(fd, buffer, len, 0, (struct sockaddr *) &server_addr, server_len);
>>>>>>> fb9980d1f6ca62f949d7488026b39b77b19e3fe8
                    if( error < 0 )
                    {
                        perror("Write error");
                    }
                }
            }
        }
    }
}
