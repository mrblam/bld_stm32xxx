/*
 * hex_line_format.h
 *
 *  Created on: Jan 26, 2024
 *      Author: hoanpx
 */

#ifndef INC_APP_HEX_LINE_FORMAT_H_
#define INC_APP_HEX_LINE_FORMAT_H_

typedef enum{
    DataRecord = 0,
    EndOfFileRecord = 1,
    ExtendedSegmentAddressRecord = 2,
    ExtendedLinearAddressRecord = 4,
    StartLinearAddressRecord = 5,
    hex_type_default = 0xff
} hex_record_type;
typedef struct hexLine_t {
    uint8_t     header;
    hex_record_type   type;
    uint8_t     byte_count;
    uint8_t     data_len;
    uint8_t     data_offset;
    uint32_t    address;
    uint32_t    base_address;
    uint8_t     data[32];
    uint8_t     check_sum;
    uint32_t    main_add;
}hexLine;
uint8_t APP_CheckHexLine(hexLine *record, uint8_t *arr, uint32_t line);
uint8_t APP_GetDataHexLine(hexLine *record,uint8_t* buff);
#endif /* INC_APP_HEX_LINE_FORMAT_H_ */

