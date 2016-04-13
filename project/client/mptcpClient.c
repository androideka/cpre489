#include <arpa/inet.h>
#include <inttypes.h>
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

typedef struct {
	uint16_t sn;
} ack_t;

struct sockaddr_in sender, receiver;
struct timeval tv;

packet_t make_packet(uint16_t sn, uint8_t data1, uint8_t data2,
					 uint8_t data3, uint8_t data4, uint8_t data5);

int main(int argc, char** argv)
{
	char** buffer = malloc(6 * sizeof(char*));
	int err, buf_len = sizeof(buffer);
	int fd_sock, ss_thresh = 16;
	int cwnd = 1;
	int curr_sn;
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

	tv.tv_sec = 3;
	if( setsockopt(fd_sock, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(tv)) < 0 )
	{
		perror("Set timeout failed");
	}

	err = connect(fd_sock, (struct sockaddr*) &receiver, sizeof(receiver));
	if( err < 0 )
	{
		perror("Connect error");
	}
	int i = 0;
	ack_t* ack;
	ack = malloc(sizeof(ack_t));
	ack_t* last_ack;
	last_ack = malloc(sizeof(ack_t));
	int dup_count = 0;
	last_ack->sn = 0;
	while( i < 40 )
	{
		memset(buffer, 0, sizeof(buffer));
		printf("%d\n", cwnd);
		uint8_t data1 = (rand() % 52);
		uint8_t data2 = (rand() % 52);
		uint8_t data3 = (rand() % 52);
		uint8_t data4 = (rand() % 52);
		uint8_t data5 = (rand() % 52);

		packet_t packet;
		uint16_t sn = i * sizeof(packet_t);
		packet = make_packet(sn,
							 data1, data2, data3, data4, data5);

		err = write(fd_sock, &packet, sizeof(packet));
		if( err < 0 )
		{
			perror("Write error");
		}

		err = read(fd_sock, buffer, buf_len);
		if( err < 0 )
		{
			perror("Read error or socket timeout");
			ss_thresh = cwnd / 2;
			cwnd = 1;
			continue;
		}

		ack = buffer;

		//printf("New ACK sn: %" PRIu16 ", last ACK sn: %" PRIu16"\n", ack->sn, last_ack->sn);

		if( ack->sn != sn )
		{
			// New ACK
			if( cwnd < ss_thresh )
			{
				if( cwnd * 2 > ss_thresh )
					cwnd = ss_thresh;
				else
					cwnd *= 2;
			}
			else
			{
				cwnd++;
			}
		}
		else if( ++dup_count == 3 )
		{
			dup_count = 0;
			ss_thresh = cwnd / 2;
			cwnd = 1;
			continue;
		}

		last_ack->sn = ack->sn;
		i++;

	}

	close(fd_sock);

	return 0;

}


packet_t make_packet(uint16_t sn, uint8_t data1, uint8_t data2,
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
