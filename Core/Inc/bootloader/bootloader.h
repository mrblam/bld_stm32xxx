/*
 * bootloader.h
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */

#ifndef INC_BOOTLOADER_BOOTLOADER_H_
#define INC_BOOTLOADER_BOOTLOADER_H_
/*********************************************/
typedef enum{
    Boot_State_Idle = 0,
    Boot_State_Init = 1,
    Boot_State_Receive_new_firmware = 2,
    Boot_State_Jump_To_User_App = 3,
    Boot_State_Handler_Request = 4,
    Boot_State_Get_Basic_Info = 5,
    Boot_State_default = 0xff
}Boot_State;
typedef void (*fnc_ptr)(void);
/*********************************************/
/*
 * @brief jump to application user
 * @param address
 * @retval 0 if success
 */
uint8_t boot_jump_to_app(uint32_t app_address);
uint8_t boot_check_is_new_firmware(uint32_t app_address);
/*********************************************/

#endif /* INC_BOOTLOADER_BOOTLOADER_H_ */
