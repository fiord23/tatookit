#include "flash.h"
#include "stm32l431xx.h"
#include "motor.h"

#define FLASH_USER_ADDR 0x0803F800
extern uint8_t speed;

void flash_rdp_level1(void)
{
    __disable_irq();
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    FLASH_OBProgramInitTypeDef init;
    RCC->AHB1ENR |= RCC_AHB1ENR_FLASHEN;

    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
    HAL_FLASH_OB_Launch();
    __enable_irq();
}

HAL_StatusTypeDef CodeProtection_SetLevel(uint8_t Level)
{
    HAL_StatusTypeDef ret = HAL_OK;
    FLASH_OBProgramInitTypeDef init;
    uint8_t rdp_level;

    // Check input parameters
    switch (Level)
    {
    case 0: // Level 0: Read-out protection disabled
        rdp_level = OB_RDP_LEVEL_0;
        break;
    case 1: // Level 1: Read-out protection enabled
        rdp_level = OB_RDP_LEVEL_1;
        break;
    // case 2:	// Level 2: Permanent protection enabled
    //	rdp_level = OB_RDP_LEVEL_2;
    //	break;
    default: // Incorrect parameter
        return HAL_ERROR;
    }

    // Read current protection level
    HAL_FLASHEx_OBGetConfig(&init);
    if (init.RDPLevel == rdp_level)
    {
        return HAL_OK;
    }

    // Only RDP option byte
    init.OptionType = OPTIONBYTE_RDP;
    init.RDPLevel = rdp_level;

    // Program option bytes
    ret = HAL_FLASH_Unlock();
    if (ret != HAL_OK)
    {
        return ret;
    }
    ret = HAL_FLASH_OB_Unlock();
    if (ret != HAL_OK)
    {
        return ret;
    }
    ret = HAL_FLASHEx_OBProgram(&init);
    if (ret != HAL_OK)
    {
        return ret;
    }
    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();

    // Reload option bytes (causes system reset)
    HAL_FLASH_OB_Launch();

    return ret;
}

void Flash_Save(uint8_t value)
{
    uint64_t current = *(uint64_t *)FLASH_USER_ADDR;

    if ((uint8_t)(current & 0xFF) == value)
        return; // уже записано

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase;
    uint32_t pageError = 0;

    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Page = (FLASH_USER_ADDR - 0x08000000) / 2048;
    erase.NbPages = 1;

    if (HAL_FLASHEx_Erase(&erase, &pageError) == HAL_OK)
    {
        uint64_t data64 = value;

        HAL_FLASH_Program(
            FLASH_TYPEPROGRAM_DOUBLEWORD,
            FLASH_USER_ADDR,
            data64);
    }

    HAL_FLASH_Lock();
}

void Flash_LoadOrInit(void)
{
    uint64_t flash_data = *(uint64_t *)FLASH_USER_ADDR;

    if (flash_data == 0xFFFFFFFFFFFFFFFF)
    {
        speed = MOTOR_SPEED_DEFAULT;
        Flash_Save(speed);
    }
    else
    {
        speed = (uint8_t)(flash_data & 0xFF);
    }
}
