#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "AddCongestion.h"

struct sockaddr_in server, client;

int main() {

    char buffer[8];
    int c, fd_sock, client_len;
    size_t buf_len = sizeof(buffer);

    control.sin_family = AF_INET;
    control.sin_port = htons(2021);
    control.sin_addr.s_addr = inet_addr("127.0.0.1");

    fd_sock = socket(AF_INET, SOCK_STREAM, 0);
    int err = bind(fd_control, (struct sockaddr*) &control, sizeof(control));
    if( err < 0 )
    {
        perror("Control bind error");
    }

    listen(fd_sock, 0);

    c = accept(fd_sock, (struct sockaddr*) &client_control, (socklen_t*) &client_len);
    if( c < 0 )
    {
        perror("Accept error");
    }

    for( ; ; )
    {

        err = (int) read(c, buffer, buf_len);
        if( err < 0 )
        {
            perror("Read error");
        }

        if( AddCongestion(10) != 0 )
        {
            continue;
        }

    }

}
