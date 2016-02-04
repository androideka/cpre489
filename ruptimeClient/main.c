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
    int i = 0;
    char* ip_addresses[3];
    ip_addresses[0] = "205.237.185.196";
    ip_addresses[1] = "129.186.252.44";
    ip_addresses[2] = "127.0.0.1";
    for( i = 0; i < 3; i++ ) {
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = 7;
        // HOME IP: 205.237.185.196
        servaddr.sin_addr.s_addr = inet_addr(ip_addresses[i]);
        int err = 0;
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            perror("Socket error.");
        }
        err = connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        if (err < 0) {
            perror("Connect error.");
        }
        n = (int) read(fd, uptime, 1024);
        if (n < 0) {
            perror("Read error.");
        }
        char* command = "host ";
        strcat(command, ip_addresses[i]);
        strcat(command, " > hostname.txt");
        system(command);
        FILE *hostname = fopen("hostname.txt", "r");
        char *lineptr = "";
        char *token, *temp;
        getline((char **) lineptr, 0, hostname);
        token = strtok(lineptr, " ");
        while (token != NULL) {
            temp = token;
            token = strtok(NULL, " ");
            if (token == NULL) {
                break;
            }
        }
        system("rm hostname.txt");
        printf("%s uptime: %s\n", temp, uptime);
    }
    return 0;
}
