/*
 * board.c
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */
#include "lib_abstract.h"
#include "gpio_hw.h"
#include "uart_hw.h"
#include "clock_hw.h"
#include "board.h"

void BOARD_Init(void){
//    DRV_Config_SystemClock();
    MX_USART2_UART_Init();
    MX_GPIO_Init();
}

