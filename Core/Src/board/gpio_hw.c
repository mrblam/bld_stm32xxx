/*
 * gpio_hw.c
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */

#include <lib_abstract.h>
#include "gpio_hw.h"

void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
}
