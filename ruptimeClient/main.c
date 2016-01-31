#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct sockaddr_in servaddr;
char uptime[1024] = "";

int main() {

    int fd, n;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = 7;
    // HOME IP: 205.237.185.196
    servaddr.sin_addr.s_addr = inet_addr("192.168.1.105");
    int err = 0;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if( fd < 0 )
    {
        perror("Socket error.");
    }
    err = connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if( err < 0 )
    {
        perror("Connect error.");
    }
    n = (int) read(fd, uptime, 1024);
    if( n < 0 )
    {
        perror("Read error.");
    }
    system("host 192.168.1.105 > hostname.txt");
    FILE* hostname = fopen("hostname.txt", "r");
    char* lineptr = "";
    char* token, *temp;
    getline((char **) lineptr, 0, hostname);
    token = strtok(lineptr, " ");
    while( token != NULL )
    {
        temp = token;
        token = strtok(NULL, " ");
        if(token == NULL)
        {
            break;
        }
    }
    printf("%s uptime: %s\n", temp, uptime);

    return 0;
}