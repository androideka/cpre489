#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct sockaddr_in ServAddr, ClientAddr;

int main() {

    char buffer[1024];
    int s, fd, pid, ClientAddrLen = sizeof(ClientAddr);
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_port = 7;
    // Home IP: 205.237.185.196
    // Home laptop IP:
    ServAddr.sin_addr.s_addr = inet_addr("205.237.185.196");
    fd = socket(AF_INET, SOCK_STREAM, 0);
    int error = bind(fd, (struct sockaddr*) &ServAddr, sizeof(ServAddr));
    if( error < 0 )
    {
        perror("Bind error.\n");
    }
    listen(fd, 5);

    for( ; ; ) {
        s = accept(fd, (struct sockaddr *) &ClientAddr, (socklen_t *) &ClientAddrLen);
        if (s < 0) {
            perror("Accept error.\n");
        }
        system("ruptime > uptime.txt");

        size_t length = 0;
        FILE *fp = fopen("uptime.txt", "r");
        if (fp != NULL) {
            length = fread(buffer, sizeof(char), 1024, fp);
            if (length == 0) {
                fputs("Error reading file", stderr);
            }
            else {
                buffer[length++] = '\0';
            }
        }

        int err = write(s, buffer, length);
        if (err < 0)
        {
            perror("Write error.\n");
        }

        close(s);
    }
}