/*
 * bootloader.c
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */

/*********************************************/
#include <stdint.h>
#include "bootloader.h"
#include "lib_abstract.h"
/*********************************************/
uint8_t boot_jump_to_app(uint32_t app_address)
{
    __disable_irq();
    fnc_ptr jump_to_app;
    __set_MSP(*(volatile uint32_t*)app_address);
    SCB->VTOR = (uint32_t)(volatile uint32_t*)app_address;
    jump_to_app = (fnc_ptr)(*(volatile uint32_t*) (app_address + 4u));
    __enable_irq();
    jump_to_app();
    return 0;
}
uint8_t boot_check_is_new_firmware(uint32_t app_address){
    if((*(volatile uint32_t*)app_address) == 0xFFFFFFFF){
        return 1;
    }
    return 0;
}

