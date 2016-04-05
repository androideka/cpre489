#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
	uint16_t sn;
	uint8_t data[5];
} packet_t;

struct sockaddr_in sender, receiver;
struct timeval tv;

int main(int argc, char** argv)
{
	char* buffer;
	int err, buf_len = sizeof(buffer);
	int fd_sock;
	socklen_t receiver_len = sizeof(receiver);

	sender.sin_family = AF_INET;
	sender.sin_port = htons(2020);
	sender.sin_addr.s_addr = inet_addr("127.0.0.1");

	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(2021);
	receiver.sin_addr.s_addr = inet_addr("127.0.0.1");

	fd_sock = socket(AF_INET, SOCK_STREAM, 0);
	if( fd_sock < 0 )
	{
		perror("Socket error");
	}

	buffer = "RANDOM";

	err = connect(fd_sock, (struct sockaddr*) &receiver, sizeof(receiver));
	if( err < 0 )
	{
		perror("Connect error");
	}

	err = write(fd_sock, buffer, sizeof(buffer));
	if( err < 0 )
	{
		perror("Write error");
	}
	memset(buffer, 0, sizeof(buffer));

	close(fd_sock);

	return 0;

}


packet_t packet(uint16_t sn, uint8_t data1, uint8_t data2,
		uint8_t data3, uint8_t data4, uint8_t data5)
{
	packet_t packet;
	packet.sn = sn;
	packet.data[0] = data1;
	packet.data[1] = data2;
	packet.data[2] = data3;
	packet.data[3] = data4;
	packet.data[4] = data5;
	return packet;
}
