/**
 * @file    flash.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module handles the memory related functions.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "flash.h"

/* Function pointer for jumping to user application. */
typedef void (*fnc_ptr)(void);

/**
 * @brief   This function erases the memory.
 *          The Standard Peripheral Library has no "erase N pages" call, so the
 *          pages are erased one by one, from "address" up to the end of flash.
 * @param   address: First address to be erased (the last is the end of the
 * flash).
 * @return  status: Report about the success of the erasing.
 */
flash_status flash_erase(uint32_t address)
{
    flash_status status = FLASH_OK;

    FLASH_Unlock();

    /* Erase every page from "address" up to the end of the flash. */
    for (uint32_t page = address;
         page <= (FLASH_END_ADDRESS - (FLASH_PAGE_SIZE - 1u));
         page += FLASH_PAGE_SIZE)
    {
        if (FLASH_COMPLETE != FLASH_ErasePage(page))
        {
            status = FLASH_ERROR;
            break;
        }
    }

    FLASH_Lock();

    return status;
}

/**
 * @brief   This function flashes the memory.
 * @param   address: First address to be written to.
 * @param   *data:   Array of the data that we want to write.
 * @param   *length: Size of the array.
 * @return  status: Report about the success of the writing.
 */
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length)
{
    flash_status status = FLASH_OK;

    FLASH_Unlock();

    /* Loop through the array. */
    for (uint32_t i = 0u; (i < length) && (FLASH_OK == status); i++)
    {
        /* If we reached the end of the memory, then report an error and don't
         * do anything else.*/
        if (FLASH_APP_END_ADDRESS <= address)
        {
            status = FLASH_ERROR_SIZE;
        }
        else
        {
            /* The actual flashing. If there is an error, then report it. */
            if (FLASH_COMPLETE != FLASH_ProgramWord(address, data[i]))
            {
                status = FLASH_ERROR_WRITE;
            }
            /* Read back the content of the memory. If it is wrong, then report
             * an error. */
            else if (data[i] != (*(volatile uint32_t *)address))
            {
                status = FLASH_ERROR_READBACK;
            }
            else
            {
                /* Nothing to do. */
            }

            /* Shift the address by a word. */
            address += 4u;
        }
    }

    FLASH_Lock();

    return status;
}

/**
 * @brief   Sets the main stack pointer.
 *          The Standard Peripheral Library's CMSIS (core_cm3.h) only declares
 *          __set_MSP() as extern and provides no core_cm3.c implementation, so
 *          it is done here with inline assembly to keep the module self
 *          contained.
 * @param   top_of_main_stack: New value of the MSP register.
 * @return  void
 */
static inline void flash_set_msp(uint32_t top_of_main_stack)
{
    __asm volatile("MSR msp, %0" : : "r"(top_of_main_stack) : "memory");
}

/**
 * @brief   Actually jumps to the user application.
 * @param   void
 * @return  void
 */
void flash_jump_to_app(void)
{
    /* Function pointer to the address of the user application. */
    fnc_ptr jump_to_app;
    jump_to_app =
        (fnc_ptr)(*(volatile uint32_t *)(FLASH_APP_START_ADDRESS + 4u));

    /* Reset all the peripherals of the bootloader, so the user application
     * starts from a clean state (the SPL equivalent of HAL_DeInit()). */
    RCC_APB1PeriphResetCmd(0xFFFFFFFFu, ENABLE);
    RCC_APB1PeriphResetCmd(0xFFFFFFFFu, DISABLE);
    RCC_APB2PeriphResetCmd(0xFFFFFFFFu, ENABLE);
    RCC_APB2PeriphResetCmd(0xFFFFFFFFu, DISABLE);

    /* Disable all interrupts. */
    __disable_irq();

    /* Change the main stack pointer. */
    flash_set_msp(*(volatile uint32_t *)FLASH_APP_START_ADDRESS);

    /* Enable interrupts and jump to the user application. */
    __enable_irq();
    jump_to_app();
}
