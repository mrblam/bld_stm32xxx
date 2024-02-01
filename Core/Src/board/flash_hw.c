/*
 * flash_hw.c
 *
 *  Created on: Jan 31, 2024
 *      Author: hoanpx
 */
#include <lib_abstract.h>
#include "flash_hw.h"

static uint32_t GetSector(uint32_t Address) {
    uint32_t sector = 0;

    if ((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0)) {
        sector = FLASH_SECTOR_0;
    } else if ((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1)) {
        sector = FLASH_SECTOR_1;
    } else if ((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2)) {
        sector = FLASH_SECTOR_2;
    } else if ((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3)) {
        sector = FLASH_SECTOR_3;
    } else if ((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4)) {
        sector = FLASH_SECTOR_4;
    } else if ((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5)) {
        sector = FLASH_SECTOR_5;
    } else if ((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6)) {
        sector = FLASH_SECTOR_6;
    } else if ((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7)) {
        sector = FLASH_SECTOR_7;
    } else if ((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8)) {
        sector = FLASH_SECTOR_8;
    } else if ((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9)) {
        sector = FLASH_SECTOR_9;
    } else if ((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10)) {
        sector = FLASH_SECTOR_10;
    } else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11) */
    {
        sector = FLASH_SECTOR_11;
    }
    return sector;
}
void DRV_FLASH_Init() {
    HAL_FLASH_Unlock();

    /* Clear pending flags (if any) */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |
                            FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                            FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}
uint8_t DRV_FLASH_Erase(uint32_t StartSector, uint8_t numberSector) {
    uint32_t UserStartSector;
    uint32_t SectorError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    /* Unlock the Flash to enable the flash control register access *************/
    DRV_FLASH_Init();
    /* Get the sector where start the user flash area */
    UserStartSector = GetSector(StartSector);
    pEraseInit.TypeErase = TYPEERASE_SECTORS;
    pEraseInit.Sector = UserStartSector;
    pEraseInit.NbSectors = numberSector;
    pEraseInit.VoltageRange = VOLTAGE_RANGE_3;
    if (HAL_FLASHEx_Erase(&pEraseInit, &SectorError) != HAL_OK) {
        /* Error occurred while page erase */
        return (1);
    }
    return (0);
}
uint8_t DRV_FLASH_Program(uint32_t add,uint8_t* data,uint16_t len){
    uint8_t i;
    uint64_t data64 = 0;
    for(i = 0;i < len/4;i++){
        data64 = (data[0 + 4*i] ) |
                 (data[1 + 4*i] << 8) |
                 (data[2 + 4*i] << 16) |
                 (data[3 + 4*i] << 24) ;
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, add, data64) == HAL_OK) {
            if (*(uint32_t*)add != (uint32_t)(data64))
            {
              return 1;
            }
        }else{
            return 1;
        }
        data64 = 0;
        add += 4U;
    }
    return 0;
}
uint16_t DRV_FLASH_GetWriteProtectionStatus(void) {
    uint32_t ProtectedSECTOR = 0xFFF;
    FLASH_OBProgramInitTypeDef OptionsBytesStruct;

    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    /* Check if there are write protected sectors inside the user flash area ****/
    HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();

    /* Get pages already write protected ****************************************/
    ProtectedSECTOR = ~(OptionsBytesStruct.WRPSector) & FLASH_SECTOR_TO_BE_PROTECTED;

    /* Check if desired pages are already write protected ***********************/
    if (ProtectedSECTOR != 0) {
        /* Some sectors inside the user flash area are write protected */
        return FLASHIF_PROTECTION_WRPENABLED;
    } else {
        /* No write protected sectors inside the user flash area */
        return FLASHIF_PROTECTION_NONE;
    }
}
