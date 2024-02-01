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
#define DRV_UART_Transmit(huart,_string,_length,_timeout)   (HAL_UART_Transmit(huart,_string,_length,_timeout))
#define DRV_UART_Receive_IT(_huart,_data,_len)              (HAL_UART_Receive_IT(_huart,_data,_len))
#define DRV_UART_DeInit(_huart)                             (HAL_UART_DeInit(_huart))

#define DRV_GetREVID()                                      (HAL_GetREVID())
#define DRV_GetUIDw0()                                      (HAL_GetUIDw0())
#define DRV_GetUIDw1()                                      (HAL_GetUIDw1())
#define DRV_GetUIDw2()                                      (HAL_GetUIDw2())
#define DRV_GetDEVID()                                      (HAL_GetDEVID())

#define DRV_Delay(_value)                                   (HAL_Delay(_value))

#define DRV_GPIO_ReadPin(_port,_pin)                        (HAL_GPIO_ReadPin(_port,_pin))
#define DRV_GPIO_DeInit(_port,_pin)                         (HAL_GPIO_DeInit(_port,_pin))

#define DRV_RCC_DeInit()                                    (HAL_RCC_DeInit())

#define BOOT_BTN_PORT                                       GPIOA
#define BOOT_BTN_PIN                                        GPIO_PIN_0
/******************************************/
void BOARD_Init(void);

/******************************************/
#endif /* INC_BOARD_BOARD_H_ */
