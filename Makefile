.PHONY: all build cmake clean

TARGET := main
TEST_MODE ?= FALSE
TEST_SWC ?= simple_module
BUILD_TYPE ?= Debug
CMAKE_PATH := cmake
CTEST_PATH := ctest
BOARD ?= youfang
SOFTWARE ?= app

ifeq ($(TEST_MODE), TRUE)
	BUILD_DIR := build/test/$(TEST_SWC)
	TOOLCHAIN_FILE := ""
else
	BUILD_DIR := build/release
	TOOLCHAIN_FILE := cmake/gcc-arm-none-eabi.cmake
endif

# Define board-specific config files outside the target recipe
ifeq ($(BOARD),youfang)
    INTERFACE_CFG := interface/stlink.cfg
else ifeq ($(BOARD),embedfire)
    INTERFACE_CFG := interface/cmsis-dap.cfg
else
    $(error Invalid TARGET_BOARD '$(TARGET_BOARD)'. Must be 'youfang' or 'embedfire')
endif

ifeq ($(SOFTWARE),app)
    BINARY_CFG := "program build/release/main.bin 0x08008000 verify reset exit" 
else ifeq ($(SOFTWARE),bootloader)
    BINARY_CFG := "program proj-00-bootloader-spl/bin/main.bin 0x08000000 verify reset exit" 
else
    $(error Invalid SOFTWARE '$(SOFTWARE)'. Must be 'app' or 'bootloader')
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
		-DTARGET_BOARD=${BOARD} \
		-DSOFTWARE=${SOFTWARE} \
		-DPROJECT=${PROJECT} \
		-DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
		-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DDUMP_ASM=OFF 

cmake: ${BUILD_DIR}/Makefile

build: cmake
	@$(MAKE) -C ${BUILD_DIR} --no-print-directory

flash: build/release/Makefile 
	openocd -f $(INTERFACE_CFG) -f target/stm32f1x.cfg -c $(BINARY_CFG) 

# test: build
# 	ctest --test-dir ./build/test
clean:
	rm -rf build


