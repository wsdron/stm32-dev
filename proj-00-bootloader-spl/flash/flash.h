/**
 * @file    flash.h
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module handles the memory related functions.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stm32f10x.h"
#include "stm32f10x_flash.h"

/**
 * @brief   Page size of the on-chip flash.
 *          The Standard Peripheral Library does not expose this, so it is
 *          derived from the density selected in the project defines.
 *          Low/Medium density devices have 1 KB pages, High/XL density
 *          (and Connectivity line) devices have 2 KB pages.
 */
#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) ||                       \
    defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
#define FLASH_PAGE_SIZE ((uint32_t)0x400u) /**< 1 KB. */
#elif defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||                     \
    defined(STM32F10X_XL) || defined(STM32F10X_CL)
#define FLASH_PAGE_SIZE ((uint32_t)0x800u) /**< 2 KB. */
#else
#error                                                                         \
    "FLASH_PAGE_SIZE: unsupported STM32F10x density, define STM32F10X_LD/MD/HD/XL/CL."
#endif

/**
 * @brief   Last address of the on-chip flash, i.e. FLASH_BASE + flash size - 1.
 *          The Standard Peripheral Library does not expose this either, so the
 *          flash size is derived from the density selected in the project
 *          defines.
 */
#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL)
#define FLASH_SIZE ((uint32_t)0x00008000u) /**< 32 KB. */
#elif defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
#define FLASH_SIZE ((uint32_t)0x00020000u) /**< 128 KB. */
#elif defined(STM32F10X_HD)
#define FLASH_SIZE ((uint32_t)0x00080000u) /**< 512 KB. */
#elif defined(STM32F10X_XL)
#define FLASH_SIZE ((uint32_t)0x00100000u) /**< 1 MB. */
#elif defined(STM32F10X_CL)
#define FLASH_SIZE ((uint32_t)0x00040000u) /**< 256 KB. */
#endif

#define FLASH_END_ADDRESS ((uint32_t)(FLASH_BASE + FLASH_SIZE - 1u))

/* Start and end addresses of the user application. */
#define FLASH_APP_START_ADDRESS ((uint32_t)0x08008000u)
#define FLASH_APP_END_ADDRESS                                                  \
    ((uint32_t)(FLASH_END_ADDRESS -                                            \
                0x10u)) /**< Leave a little extra space at the end. */

/* Status report for the functions. */
typedef enum
{
    FLASH_OK = 0x00u,             /**< The action was successful. */
    FLASH_ERROR_SIZE = 0x01u,     /**< The binary is too big. */
    FLASH_ERROR_WRITE = 0x02u,    /**< Writing failed. */
    FLASH_ERROR_READBACK = 0x04u, /**< Writing was successful, but the content
                                     of the memory is wrong. */
    FLASH_ERROR = 0xFFu           /**< Generic error. */
} flash_status;

flash_status flash_erase(uint32_t address);
flash_status flash_write(uint32_t address, uint32_t *data, uint32_t length);
void flash_jump_to_app(void);

#endif /* FLASH_H_ */
