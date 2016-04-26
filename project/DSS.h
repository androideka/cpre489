//
// Created by androideka on 4/24/16.
//

#ifndef PROJECT_DSS_H
#define PROJECT_DSS_H

#include <inttypes.h>

typedef struct {
    int sub_seq_num;
    int data_seq_num;
    uint32_t data_ACK;
    uint8_t kind;
    uint8_t length;
    uint16_t data_length;
    uint16_t checksum;

} DSS_t;

#endif //PROJECT_DSS_H
