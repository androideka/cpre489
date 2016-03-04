#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct sockaddr_in control, client, server;

int main(int argc, char** argv) {

    char buffer[2048];
    int err, buf_len = sizeof(buffer);
    char* server_ip = argv[1];
    int fd_control, fd_data, s;
    socklen_t server_len = sizeof(server);
    char file_name[100];
    char command[100];
    int port;

    control.sin_family = AF_INET;
    control.sin_port = htons(2021);
    control.sin_addr.s_addr = inet_addr(server_ip);

    fd_control = socket(AF_INET, SOCK_STREAM, 0);
    if( fd_control < 0 )
    {
        perror("Socket 1 error");
    }
    int reuse = 1;
    if( setsockopt(fd_control, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0 )
    {
        perror("Setsockopt addr failed");
    }
    if( setsockopt(fd_control, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0 )
    {
        perror("Setsockopt port failed");
    }

    err = connect(fd_control, (struct sockaddr*) &control, sizeof(control));
    if( err < 0 )
    {
        perror("Connect 1 error");
    }

    printf("$> ");
    scanf(" %s %d", &command, &port);
    sprintf(buffer, "%s %d", &command, port);

    client.sin_family = AF_INET;
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    fd_data = socket(AF_INET, SOCK_STREAM, 0);
    if( fd_data < 0 )
    {
        perror("Socket 2 error");
    }
    err = bind(fd_data, (struct sockaddr*) &client, sizeof(client));
    if( err < 0 )
    {
        perror("Bind 2 error");
    }

    err = write(fd_control, buffer, sizeof(buffer));
    if( err < 0 )
    {
        perror("Write 1 error");
    }
    memset(buffer, 0, sizeof(buffer));

    listen(fd_data, 0);

    s = accept(fd_data, (struct sockaddr*) &server, (socklen_t*) &server_len);
    if( s < 0 )
    {
        perror("Accept error");
    }

    err = (int) read(s, buffer, buf_len);
    if( err < 0 )
    {
        perror("Read 1 error");
    }

    close(s);

    puts(buffer);
    memset(buffer, 0, sizeof(buffer));
    char temp[100];

    printf("$> ");
    scanf("%s %s %d", &command, &file_name, &port);
    sprintf(temp, "%s %s %d", command, file_name, port);
    memcpy(&buffer[0], temp, strlen(temp));

    err = write(fd_control, buffer, sizeof(buffer));
    if( err < 0 )
    {
        perror("Write 2 error");
    }

    client.sin_port = htons(port);

    fd_data = socket(AF_INET, SOCK_STREAM, 0);
    if( fd_data < 0 )
    {
        perror("Socket 3 error");
    }

    err = bind(fd_data, (struct sockaddr*) &client, sizeof(client));
    if( err < 0 )
    {
        perror("Bind 3 error");
    }

    printf("Buffer: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));
    listen(fd_data, 0);

    int d  = accept(fd_data, (struct sockaddr*) &server, (socklen_t*) &server_len);
    if( d < 0 )
    {
        perror("Accept error");
    }

    printf("HELLO\n");

    err = (int) read(d, buffer, buf_len);
    if( err < 0 )
    {
        perror("Read 2 error");
    }
    printf("FILE RECEIVED\n");

    FILE* file = fopen(file_name, "w");
    if( file )
    {
        fwrite(buffer, err * sizeof(char), 1, file);
        int i = 0;
        for( i = err; i < buf_len; i++ )
        {
            buffer[i] = '\0';
        }
    }
    else
    {
        printf("File not opened properly");
    }

    fclose(file);
    close(d);
    close(fd_data);
    close(fd_control);

    return 0;

}