#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct sockaddr_in control, data, client_control, client_data;

int main() {

    char buffer[2048];
    char file_buffer[2048];
    int c, d, fd_control, fd_data, client_len;
    size_t buf_len = sizeof(buffer);

    control.sin_family = AF_INET;
    control.sin_port = htons(2021);
    control.sin_addr.s_addr = inet_addr("127.0.0.1");

    fd_control = socket(AF_INET, SOCK_STREAM, 0);
    int err = bind(fd_control, (struct sockaddr*) &control, sizeof(control));
    if( err < 0 )
    {
        perror("Control bind error");
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

    listen(fd_control, 0);

    for( ; ; )
    {
        c = accept(fd_control, (struct sockaddr*) &client_control, (socklen_t*) &client_len);
        if( c < 0 )
        {
            perror("Accept 1 error");
        }

        err = (int) read(c, buffer, buf_len);
        if( err < 0 )
        {
            perror("Read error");
        }
        char* command = strtok(buffer, " ");
        /*if( command != "LIST" )
        {
            char* error_message = "Please use LIST to view the directory contents.";
            err = write(fd_control, error_message, sizeof(error_message));
            if( err < 0 )
            {
                perror("LIST error message error");
            }
        }*/
        char* port = strtok(0, " ");
        printf("%s, %s\n", command, port);

        client_data.sin_family = AF_INET;
        client_data.sin_port = htons(atoi(port));
        client_data.sin_addr.s_addr = client_control.sin_addr.s_addr;

        data.sin_family = AF_INET;
        data.sin_port = htons(2020);
        data.sin_addr.s_addr = inet_addr("127.0.0.1");

        fd_data = socket(AF_INET, SOCK_STREAM, 0);
        err = bind(fd_data, (struct sockaddr*) &data, sizeof(data));
        if( err < 0 )
        {
            perror("Data bind 1 error");
        }
        DIR *dir;
        struct dirent *ep;
        char dir_list[1028];

        dir = opendir("./");
        if( dir != NULL )
        {
            while( (ep = readdir(dir)) != NULL )
            {
                //printf("%s\n", ep->d_name);
                strcat(dir_list, ep->d_name);
                strcat(dir_list, " ");
            }
            closedir(dir);
        }
        else
        {
            perror("Directory open error");
        }

        d = connect(fd_data, (struct sockaddr*) &client_data, sizeof(client_data));
        if( d < 0 )
        {
            perror("Client connect 1 error");
        }
        err = write(fd_data, dir_list, sizeof(buffer));
        close(fd_data);

        /*d = accept(fd_control, (struct sockaddr*) &client_control, (socklen_t*) &client_len);
        if( d < 0 )
        {
            perror("Accept 2 error");
        }*/

        memset(buffer, 0, sizeof(buffer));
        err = (int) read(c, buffer, buf_len);
        if( err < 0 )
        {
            perror("Read 2 error");
        }

        command = strtok(buffer, " ");
        char* file_name = strtok(NULL, " ");
        port = strtok(NULL, " ");
        printf("%s, %s, %s\n", command, file_name, port);

        client_data.sin_port = htons(atoi(port));

        int fd_data2 = socket(AF_INET, SOCK_STREAM, 0);
        /*err = bind(fd_data2, (struct sockaddr*) &data, sizeof(data));
        if( err < 0 )
        {
            perror("Bind 2 error");
        }*/

        FILE* file = fopen(file_name, "r");
        if( file != NULL )
        {
            printf("Opening file %s\n", file_name);
            size_t new_len = fread(file_buffer, sizeof(char), sizeof(file_buffer), file);
            if( new_len == 0 )
            {
                perror("File read error");
            }
            file_buffer[new_len++] = '\0';
            fclose(file);
        }

        d = connect(fd_data2, (struct sockaddr*) &client_data, sizeof(client_data));
        if( d < 0 )
        {
            perror("Client connect 2 error");
        }

        err = write(fd_data2, file_buffer, sizeof(file_buffer));
        if( err < 0 )
        {
            perror("Final write error");
        }

        printf("Done\n");
        close(d);
        close(c);

    }

}
