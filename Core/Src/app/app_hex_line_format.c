/*
 * hex_line_format.c
 *
 *  Created on: Jan 26, 2024
 *      Author: hoanpx
 */
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include "app_hex_line_format.h"

static uint8_t digit_to_hex(const char hex) {
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    else if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    else if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;
    else
        return 0xff;
}
static uint8_t calculate_checksum(const char *hexLine) {
    if (hexLine == NULL || hexLine[0] != ':' || strlen(hexLine) < 11) {
        return 0;
    }
    size_t byteCount = (strlen(hexLine) - 5) / 2;
    uint8_t checksum = 0;
    for (size_t i = 1; i <= byteCount; i++) {
        char byteString[3] = {hexLine[i*2 - 1], hexLine[i*2], '\0'};
        uint8_t byte = (uint8_t)strtol(byteString, NULL, 16);
        checksum += byte;
    }
    checksum = (~checksum) + 1;
    return checksum;
}
uint8_t APP_CheckHexLine(hexLine *record, uint8_t *arr, uint32_t line){
    uint8_t str_len = strlen((char*)arr);
    if(arr[0] != ':'){
        return 1;
    }
    uint8_t checksum = calculate_checksum((char*)arr);
    record->check_sum = (digit_to_hex(arr[str_len-4]) << 4) | digit_to_hex(arr[str_len-3]);
    if(record->check_sum != checksum){
        return 1;
    }
    record->type = (digit_to_hex(arr[7]) << 4) | digit_to_hex(arr[8]);
    record->data_len = (digit_to_hex(arr[1]) << 4) | digit_to_hex(arr[2]);
    switch (record->type){
    case DataRecord:
        record->address = (digit_to_hex(arr[3]) << 12) |
                          (digit_to_hex(arr[4]) << 8) |
                          (digit_to_hex(arr[5]) << 4) |
                          (digit_to_hex(arr[6]));
        record->address |= record->base_address;
        for (uint8_t i = 0;i<record->data_len;i++){
            record->data[i] = digit_to_hex(arr[9 + 2*i]) << 4 | digit_to_hex(arr[10 + 2*i]);
        }
        break;
    case EndOfFileRecord:
        break;
    case ExtendedSegmentAddressRecord:
        break;
    case ExtendedLinearAddressRecord:
        record->base_address = (digit_to_hex(arr[9]) << 28) |
                               (digit_to_hex(arr[10]) << 24) |
                               (digit_to_hex(arr[11]) << 20) |
                               (digit_to_hex(arr[12]) << 16);
        break;
    case StartLinearAddressRecord:
//        :0400000508000EA140
        break;
    default:
        break;
    }

    return 0;
}
uint8_t APP_GetDataHexLine(hexLine *record,uint8_t* buff){
    uint32_t i;
    if (record->type == DataRecord) {
        for (i = 0; (i < record->data_len); i++) {
            buff[i] = record->data[i];
        }
    }
    return 0;
}
