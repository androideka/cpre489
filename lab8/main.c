#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <limits.h>
#include <inttypes.h>

#include "introduceerror.h"

typedef uint16_t crc;
#define POLY 0x88408

void printBits(size_t const size, void const * const ptr);

int main(int argc, char** argv)
{

    crc remainder = 0;
    int byte = 0;
    char* input = malloc(1024 * sizeof(char));
    uint8_t bit = 8;

    unsigned char i;
    unsigned int data;
    uint16_t crc = 0xffff;
    int length = 0;

    uint8_t test[1024];
    printf("Enter data>");
    scanf("%s", input);
    printf("%s\n", input);


    memcpy(test, (int*)&input, sizeof(input));

    //printBits(sizeof(test), &test);

    length = sizeof(input);

    do{
        for( i = 0, data = (unsigned int)0xff & *input++; i < 8; i++, data >>= 1){
            if( (crc & 0x0001) ^ (data & 0x0001))
                crc = (crc >> 1) ^ POLY;
            else crc >>= 1;
        }
    }while(--length);

    crc = ~crc;
    data = crc;
    crc = (crc << 8) | (data >> 8 & 0xff);


    printBits(sizeof(crc), &crc);

}


void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    for(i = size - 1; i >= 0; i--)
    {
        for(j = 7; j >= 0; j--)
        {
            byte = b[i] & (1 << j);
            byte >>= j;
            printf("%u", byte);
        }
    }
    puts("");
}