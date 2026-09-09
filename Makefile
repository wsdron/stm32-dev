.PHONY: all build cmake clean

TARGET := main
TEST_MODE ?= FALSE
TEST_SWC ?= simple_module
BUILD_TYPE ?= Debug
CMAKE_PATH := cmake
CTEST_PATH := ctest
TARGET_BOARD ?= youfang

ifeq ($(TEST_MODE), TRUE)
	BUILD_DIR := build/test/$(TEST_SWC)
	TOOLCHAIN_FILE := ""
else
	BUILD_DIR := build/release
	TOOLCHAIN_FILE := cmake/gcc-arm-none-eabi.cmake
endif

# Define board-specific config files outside the target recipe
ifeq ($(TARGET_BOARD),youfang)
    INTERFACE_CFG := interface/stlink.cfg
else ifeq ($(TARGET_BOARD),embedfire)
    INTERFACE_CFG := interface/cmsis-dap.cfg
else
    $(error Invalid TARGET_BOARD '$(TARGET_BOARD)'. Must be 'youfang' or 'embedfire')
endif

all: clean build 
	    @if [ "$(TEST_MODE)" = TRUE ]; then \
			$(CTEST_PATH) --test-dir $(BUILD_DIR);\
		fi

${BUILD_DIR}/Makefile:
	@${CMAKE_PATH} \
		-B${BUILD_DIR} \
		-DTEST_MODE=${TEST_MODE} \
		-DTEST_SWC=${TEST_SWC} \
		-DTARGET_BOARD=${TARGET_BOARD} \
		-DPROJECT=${PROJECT} \
		-DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
		-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DDUMP_ASM=OFF 

cmake: ${BUILD_DIR}/Makefile

build: cmake
	@$(MAKE) -C ${BUILD_DIR} --no-print-directory

flash: build/release/Makefile 
	openocd -f $(INTERFACE_CFG) -f target/stm32f1x.cfg -c "program build/release/$(TARGET).elf verify reset exit"

flash2: build/release/Makefile 
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program build/release/$(TARGET).elf verify reset exit"

flash3: build/release/Makefile 
	openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg -c "program build/release/$(TARGET).elf verify reset exit"

# test: build
# 	ctest --test-dir ./build/test
clean:
	rm -rf build


