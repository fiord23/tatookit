#include "flash.h"
#include "stm32l431xx.h"
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

  // __disable_irq();
  // FLASH_PageErase(127, 1);
  // __enable_irq();

  // uint64_t dataf = speed;
  // HAL_FLASH_Unlock();
  // HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ADDR_FLASH_PAGE, dataf);
  // HAL_FLASH_Lock();

  // uint8_t *flash_biases = (uint8_t *)(ADDR_FLASH_PAGE);
  // flash_rdp_level1();