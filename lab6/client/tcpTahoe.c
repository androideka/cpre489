#include <arpa/inet.h>
#include <dirent.h>
#include <netinet.h>
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
}

struct sockaddr_in sender, receiver;
struct timeval tv;

int main(int argc, char** argv){
	
	

}


packet_t packet(uint16_t sn, uint8_t data1, uint8_t data2,
		uint8_t data3, uint8_t data4, uint8_t data5){

	packet_t packet;
	packet.sn = sn;
	packet.data[0] = data1;
	packet.data[1] = data2;
	packet.data[2] = data3;
	packet.data[3] = data4;
	packet.data[4] = data5;
	return packet;

}
