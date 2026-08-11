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
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/led_hal)

set(STARTUP_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/led_hal/startup_stm32f103xe.s)
set(MCU_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/flash/STM32F103XE_FLASH.ld)
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
    ${CMSIS_PATH}/system_stm32f10x.c) 

    #  -------- Create executable --------
    add_executable(${EXECUTABLE}
        ${PROJECT_SOURCES}
        ${HAL_SOURCES}
        ${CMSIS_SOURCES}
        ${STARTUP_SCRIPT})
        
    #  -------- Include dependencies --------
    # 1. Collect subdirectories into a list variable
    set(MY_INCLUDE_DIRS "")
    
    file(GLOB SUBDIRS LIST_DIRECTORIES true "${PROJECT_DIR}/*")
    foreach(SUBDIR ${SUBDIRS})
        if(IS_DIRECTORY ${SUBDIR})
            list(APPEND MY_INCLUDE_DIRS ${SUBDIR})
        endif()
    endforeach()
    
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
