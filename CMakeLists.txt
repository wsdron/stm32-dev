cmake_minimum_required(VERSION 3.14)

# add modules to module path so we can include() them easier
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

project(main LANGUAGES C CXX ASM)

set(EXECUTABLE main)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CPU_PARAMETERS
    -mcpu=cortex-m3
    -mthumb
    -mfloat-abi=soft)

set(PROJECT_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})    

# Normalize input to lowercase to make the check case-insensitive
message("TARGET_BOARD passed down value '${TARGET_BOARD}'")
string(TOLOWER "${TARGET_BOARD}" TARGET_BOARD_LOWER)

if(TARGET_BOARD_LOWER STREQUAL "youfang")
    message(STATUS "Configuring for Youfang board")
    set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/flash_stm32f103c8t6/startup_stm32f103c8tx.s)
    set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/flash_stm32f103c8t6/STM32F103C8T6_FLASH.ld)
    set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT})

elseif(TARGET_BOARD_LOWER STREQUAL "embedfire")
    message(STATUS "Configuring for Embedfire board")
    set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/flash_stm32f103vet6/startup_stm32f103xe.s)
    set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/flash_stm32f103vet6/STM32F103XE_FLASH.ld)
    set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT})

else()
    message(FATAL_ERROR "Invalid TARGET_BOARD '${TARGET_BOARD}'. Must be 'youfang' or 'embedfire'.")
endif()

set(CMSIS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/hal/cmsis)
set(HAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/hal/stm32_peripheral)


# Production build
if(NOT TEST_MODE)

    #  -------- Collate Code Sources --------
    file(GLOB_RECURSE PROJECT_SOURCES "${PROJECT_DIR}/*.c")
    #  -------- Include HAL Sources -------- 
    file(GLOB_RECURSE HAL_SOURCES "${HAL_PATH}/*.c")

#  -------- Include CMSIS Sources -------- 
    set(CMSIS_SOURCES
    ${CMSIS_PATH}/syscalls.c
    ${CMSIS_PATH}/system_stm32f10x.c) 

    #  -------- Create executable --------
    add_executable(${EXECUTABLE}
        ${PROJECT_SOURCES}
        ${HAL_SOURCES}
        ${CMSIS_SOURCES}
        ${STARTUP_SCRIPT})
        
    #  -------- Include dependencies --------
    # 1. Collect subdirectories into a list variable

    ## 1. Initialize an empty list
    set(MY_INCLUDE_DIRS "")
    
    ## 2. Find ALL files in all subdirectories recursively
    file(GLOB_RECURSE ALL_FILES "${PROJECT_DIR}/*")
    
    ## 3. Extract the folder path for every file found
    foreach(FILE_PATH ${ALL_FILES})
        get_filename_component(DIR_PATH ${FILE_PATH} DIRECTORY)
        list(APPEND MY_INCLUDE_DIRS ${DIR_PATH})
    endforeach()
    
    ## 4. Remove duplicate paths so each folder is listed only once
    if(MY_INCLUDE_DIRS)
        list(REMOVE_DUPLICATES MY_INCLUDE_DIRS)
    endif()
    
    # 2. Append extra manual paths to your list
    list(APPEND MY_INCLUDE_DIRS
        ${PROJECT_DIR}
        ${HAL_PATH}/inc
        ${CMSIS_PATH})

    # 3. Add all paths to your target in one call
    target_include_directories(${EXECUTABLE} PRIVATE ${MY_INCLUDE_DIRS})

    #  -------- Compilation configuration --------
    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        target_compile_options(${EXECUTABLE} PRIVATE
            ${CPU_PARAMETERS}
                -DUSE_STDPERIPH_DRIVER
                -DSTM32F10X_HD
                -std=gnu11
                -Wall
                -Wextra
                -Wpedantic
                -Wshadow
                -Wdouble-promotion
                -Wformat=2 -Wformat-truncation
                -Wmissing-include-dirs
                -Wsign-compare
                -Wundef
                # -Wcast-align
                # -Wconversion
                -fno-common
                -fsingle-precision-constant
                -fomit-frame-pointer
                -ffunction-sections 
                -fdata-sections
                -Wno-unused-parameter
                --specs=nano.specs
                -Og -g3 -ggdb)

    else()
        target_compile_options(${EXECUTABLE} PRIVATE
        ${CPU_PARAMETERS}
            -DUSE_HAL_DRIVER 
            -DSTM32F10X_HD
            -std=gnu11
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wdouble-promotion
            -Wformat=2 -Wformat-truncation
            -Wmissing-include-dirs
            -Wsign-compare
            -Wundef
            # -Wcast-align
            # -Wconversion
            -fno-common
            -fsingle-precision-constant
            -fomit-frame-pointer
            -ffunction-sections 
            -fdata-sections
            -Wno-unused-parameter
            --specs=nano.specs
            -Og -g0)

    endif()

    target_link_options(${EXECUTABLE} PRIVATE
        -T${MCU_LINKER_SCRIPT}
        ${CPU_PARAMETERS}
        -Wl,-Map=${EXECUTABLE}.map
        --specs=nano.specs
        --specs=nosys.specs
        -Wl,-u,_printf_float
        -Wl,--gc-sections
        -lc)

    #  -------- Post-build: create binary --------
    add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin)
# Test build
else()
    #  -------- GoogleTest --------
    include(FetchContent)
    FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(src/modules/${TEST_SWC})
    add_subdirectory(src/modules/${TEST_SWC}/test)

endif()
