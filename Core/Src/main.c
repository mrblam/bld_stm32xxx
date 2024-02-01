/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <main.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "bootloader.h"
#include "app_queue.h"
#include "app_hex_line_format.h"
#include "flash_hw.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct communication {
    uint8_t *buffer;
    uint16_t index;
} communication;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define ADDR_FLASH_SECTOR_2         ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define TX_TIMEOUT                  ((uint32_t)100)
#define TX_MAX_DELAY                0xFFFFFFFFU
#define USER_APP_ADDRESS            (0x08080000u)
#define LEN_BUFFER                  60
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static communication s_rx;
extern UART_HandleTypeDef huart2;
app_queue       g_queue;
hexLine         g_record;
bool            g_is_finish = false;
bool            g_is_new_hex_line = false;
uint8_t         g_my_rx_buffer[LEN_BUFFER];
uint8_t         g_buff_temp[LEN_BUFFER];
uint8_t         data;
uint16_t        g_line = 0;
uint32_t        FlashProtection = 0;
Boot_State      boot_state = Boot_State_Idle;
__IO uint32_t   flashdestination;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void DEV_SendData(uint8_t *p_string);
void DEV_PrintMsg(char *format,...);
uint8_t boot_receive_new_firmware(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  BOARD_Init();
  HAL_UART_Receive_IT(&huart2, &data, 1);
  s_rx.buffer = g_my_rx_buffer;
  flashdestination = USER_APP_ADDRESS;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      switch (boot_state) {
      case Boot_State_Idle:
          if (DRV_GPIO_ReadPin(BOOT_BTN_PORT, BOOT_BTN_PIN)) {
              DRV_Delay(1000);
              if (DRV_GPIO_ReadPin(BOOT_BTN_PORT, BOOT_BTN_PIN)) {
                  boot_state = Boot_State_Init;
              }
          } else {
              boot_state = Boot_State_Jump_To_User_App;
          }
          break;
      case Boot_State_Init:
          DEV_SendData((uint8_t*) "Boot Starting!!!\r\n");
          APP_QUEUE_Init(&g_queue);
          FlashProtection = DRV_FLASH_GetWriteProtectionStatus();
          if (FlashProtection != FLASHIF_PROTECTION_NONE) {
              DEV_SendData((uint8_t*) "Flash is protected!\r\n");
          }
          if(DRV_FLASH_Erase(USER_APP_ADDRESS,2)){
              DEV_SendData((uint8_t*) "Erase flash is failed!\r\n");
          }
          boot_state = Boot_State_Handler_Request;
          data = 0;
          break;
      case Boot_State_Receive_new_firmware:
          boot_receive_new_firmware();
          break;
      case Boot_State_Jump_To_User_App:
          if (!boot_check_is_new_firmware(USER_APP_ADDRESS)) {
              DEV_SendData((uint8_t*) "Jumping Application...");
              DRV_GPIO_DeInit(BOOT_BTN_PORT, BOOT_BTN_PIN);
              DRV_RCC_DeInit();
              DRV_UART_DeInit(&huart2);
              boot_jump_to_app(USER_APP_ADDRESS);
          }else{
              DEV_SendData((uint8_t*) "Missing user app!\r\n");
              boot_state = Boot_State_Init;
          }
          break;
      case Boot_State_Handler_Request:
          switch (data) {
          case 0:
              DEV_SendData((uint8_t*) "Press 1: Download new firmware\r\n");
              DEV_SendData((uint8_t*) "Press 2: Get basic information\r\n");
              DEV_SendData((uint8_t*) "Press 3: Jump to User App\r\n");
              data = 0xFF;
              break;
          case 1:
              DEV_SendData((uint8_t*) "Waiting new firmware ...\r\n");
              boot_state = Boot_State_Receive_new_firmware;
              break;
          case 2:
              boot_state = Boot_State_Get_Basic_Info;
              break;
          case 3:
              boot_state = Boot_State_Jump_To_User_App;
              break;
          default:
              break;
          }
          break;
      case Boot_State_Get_Basic_Info:
          uint32_t device_id,rev_id,uid_w1,uid_w2,uid_w0;
          device_id = DRV_GetDEVID();
          rev_id = DRV_GetREVID();
          uid_w0 = DRV_GetUIDw0();
          uid_w1 = DRV_GetUIDw1();
          uid_w2 = DRV_GetUIDw2();
          DEV_SendData((uint8_t*) "IAP version 1.0\r\n");
          DEV_PrintMsg("Device ID: %x\r\n",device_id);
          DEV_PrintMsg("Revision ID: %x\r\n",rev_id);
          DEV_PrintMsg("UID:%x%x%x\r\n",uid_w0,uid_w1,uid_w2);
          boot_state = Boot_State_Handler_Request;
          data = 0;
          break;
      default:
          break;
      }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 300;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void DEV_SendData(uint8_t *p_string){
    uint16_t length = 0;
    while (p_string[length] != '\0' && length < 50) {
        length++;
    }
    DRV_UART_Transmit(&huart2, p_string, length, TX_TIMEOUT);
}
void DEV_PrintMsg(char *format,...)
{
    char str[80];
    va_list args;
    va_start(args, format);
    vsprintf(str, format, args);
    DRV_UART_Transmit(&huart2,(uint8_t*)str, strlen(str),TX_MAX_DELAY);
    va_end(args);
}
uint8_t boot_receive_new_firmware(void) {
    if (g_is_new_hex_line) {
        if (!APP_QUEUE_CheckFull(&g_queue)) {
            APP_QUEUE_EnQueue(&g_queue, g_my_rx_buffer);
            memset((char*) g_my_rx_buffer,0,LEN_BUFFER);
        }
        g_is_new_hex_line = false;
    }
    if (g_queue.level == 1) {
        strcpy((char*) g_buff_temp, (char*) APP_QUEUE_DeQueue(&g_queue));
        if (APP_CheckHexLine(&g_record, g_buff_temp, g_line)) {
            DEV_SendData((uint8_t *)"Hex file error!!!");
            DEV_SendData((uint8_t *)"Please reset MCU!");
            Error_Handler();
        } else {
            if (g_record.type == DataRecord) {
                APP_GetDataHexLine(&g_record, g_buff_temp);
                if(!DRV_FLASH_Program(g_record.address,g_buff_temp,g_record.data_len)){
                    DEV_SendData((uint8_t *)">");
                }else{
                    DEV_SendData((uint8_t *)"Write data to flash error!\r\n");
                    boot_state = Boot_State_Handler_Request;
                    data = 0;
                }
            }
            if (g_record.type == EndOfFileRecord) {
                g_is_finish = true;
            }
        }
    }
    if (g_is_finish == true) {
        g_is_finish = false;
        boot_state = Boot_State_Handler_Request;
        data = 0;
        DEV_SendData((uint8_t*) "Update Firmware Success!!!\r\n");
    }
    return 0;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
    DRV_UART_Receive_IT(&huart2, &data, 1);
    if(boot_state == Boot_State_Receive_new_firmware){
        if (s_rx.index < LEN_BUFFER) {
            s_rx.buffer[s_rx.index] = data;
            s_rx.index++;
            if (s_rx.buffer[s_rx.index - 1] == '\n') {
                g_is_new_hex_line = true;
                s_rx.index = 0;
            }
        } else {
            s_rx.index = 0;
            memset(s_rx.buffer, 0, LEN_BUFFER);
        }
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
