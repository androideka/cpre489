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
    servaddr.sin_addr.s_addr = inet_addr("205.237.185.196");

    fd = socket(AF_INET, SOCK_STREAM, 0);
    connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    n = (int) read(fd, uptime, 1024);
    system("host 205.237.185.196 > hostname.txt");
    FILE* hostname = fopen("hostname.txt", "r");
    char* lineptr = "";
    char* token;
    getline((char **) lineptr, 0, hostname);
    token = strtok(lineptr, " ");
    while( token != NULL )
    {
        token = strtok(NULL, " ");
    }
    printf("%s uptime: %s\n", token, uptime);

    return 0;
}