/*
 * board.h
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */

#ifndef INC_BOARD_BOARD_H_
#define INC_BOARD_BOARD_H_
/******************************************/
/*Define abstract function*/
#define DRV_UART2_Send(_value)                              (HAL_UART_Transmit(&huart2, (UBYTE*)&_value, 1, 50))
#define DRV_Delay(_value)                                   (HAL_Delay(_value))
#define DRV_FLASH_ProgramWord(_add,_data64)                 (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,_add,_data64))
#define DRV_UART_Transmit(huart,_string,_length,_timeout)   (HAL_UART_Transmit(huart,_string,_length,_timeout))
/******************************************/
void BOARD_Init(void);

/******************************************/
#endif /* INC_BOARD_BOARD_H_ */
