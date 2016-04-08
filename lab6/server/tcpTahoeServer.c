#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct sockaddr_in server, client;
typedef struct {
    uint16_t sn;
    uint8_t data[5];
} packet_t;

typedef struct {
    uint16_t sn;
} ack_t;

int add_congestion(double p);
double double_rand(double min, double max);

int main(int argc, char** argv) {

    char buffer[16];
    int c, fd_sock, sn, client_len = sizeof(client);
    size_t buf_len = sizeof(buffer);

    double p = atof(argv[1]);

    server.sin_family = AF_INET;
    server.sin_port = htons(2021);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    fd_sock = socket(AF_INET, SOCK_STREAM, 0);
    int err = bind(fd_sock, (struct sockaddr*) &server, sizeof(server));
    if( err < 0 )
    {
        perror("Control bind error");
    }

    listen(fd_sock, 0);

    c = accept(fd_sock, (struct sockaddr*) &client, (socklen_t*) &client_len);
    if( c < 0 )
    {
        perror("Accept error");
    }

    for( ; ; )
    {
        memset(buffer, 0, sizeof(buffer));
        err = (int) read(c, buffer, buf_len);
        if( err < 0 )
        {
            perror("Read error");
        }

        if( add_congestion(p) != 0 )
        {
            printf("Dropping packet\n");
            continue;
        }

        packet_t* packet;
        packet = malloc(sizeof(packet_t));
        packet = buffer;

        ack_t* ack;
        ack = malloc(sizeof(ack_t));
        ack->sn = packet->sn;

        sleep(1);

        printf("Sending ack %" PRIu16 "\n", ack->sn);
        err = (int) write(c, &ack, sizeof(ack_t));


    }

}

int add_congestion(double p)
{
    double random1 = double_rand(0.0, 99.9);
    if( random() % 100 < p )
    {
        return 1;
    }
    return 0;
}

double double_rand(double min, double max)
{
    double range = max - min;
    double div = RAND_MAX / range;
    return min + (rand() / div);
}