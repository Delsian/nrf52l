CMAKE_MINIMUM_REQUIRED(VERSION 3.3)
INCLUDE("../config/nrf52.cmake")
PROJECT(controller C ASM)
enable_language(ASM)
enable_language(C)

ADD_DEFINITIONS( 
	-DBOARD_CUSTOM
	-DCONFIG_GPIO_AS_PINRESET
	-DBLE_STACK_SUPPORT_REQD
	-DFLOAT_ABI_HARD
	-DNRF52
	-DNRF52832_XXAA
	-DNRF52_PAN_74
	-DNRF_SD_BLE_API_VERSION=6
	-DS132
	-DSOFTDEVICE_PRESENT
	-DSWI_DISABLE0
	-DNRF_DFU_SETTINGS_VERSION=1
	-DNRF_DFU_SVCI_ENABLED
	-DSVC_INTERFACE_CALL_AS_NORMAL_FUNCTION
	-DuECC_ENABLE_VLI_API=0
	-DuECC_OPTIMIZATION_LEVEL=3
	-DuECC_SQUARE_FUNC=0
	-DuECC_SUPPORT_COMPRESSED_POINT=0
	-DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
	-DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
	-DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
	-D__HEAP_SIZE=8192
	-D__STACK_SIZE=8192
	-DNRFX_RNG_ENABLED=1
#	-DUSE_TWI_MNGR
	)

INCLUDE("../config/SDK.cmake")

SET(ESPRUINO_FILES
	Espruino/src/jsflags.c
	Espruino/src/jslex.c
	Espruino/src/jsnative.c
	Espruino/src/jsparse.c
	Espruino/src/jsutils.c
	Espruino/src/jsvar.c
	Espruino/src/jsflash.c
	Espruino/src/jsvariterator.c
	Espruino/src/jswrap_array.c
	Espruino/src/jswrap_arraybuffer.c
	Espruino/src/jswrap_dataview.c
	Espruino/src/jswrap_date.c
	Espruino/src/jswrap_error.c
	Espruino/src/jswrap_functions.c
	Espruino/src/jswrap_json.c
	Espruino/src/jswrap_modules.c
	Espruino/src/jswrap_number.c
	Espruino/src/jswrap_object.c
	Espruino/src/jswrap_process.c
	Espruino/src/jswrap_promise.c
	Espruino/src/jswrap_regexp.c
	Espruino/src/jswrap_string.c
	)
SET(SOURCE_FILES
	src/main.c
	src/bluetooth.c
	src/buzzer.c
	src/control.c 
	src/custom_service.c
	src/fs.c
	src/js_module.c
	src/pca9685.c
	src/r0b1c_cmd.c
	src/r0b1c_service.c
	src/devices/battery.c
	src/devices/dummy.c
#	src/devices/gyro.c
	src/devices/jswrap_led.c
	src/devices/jswrap_motor.c
	src/devices/jswrap_range.c
	src/devices/motor.c
	src/devices/r0b1c_device.c
	src/devices/range.c
	src/devices/rdev_btn.c
	src/devices/rdev_led.c
	src/devices/rj_port.c
	src/js/jshardware.c
	src/js/jswrap_board.c
	src/js/jswrap_math.c
	src/js/jswrapper.c
	)

SET(COMPILE_DEFINITIONS_DEBUG -O0 -g3 -DDEBUG)
SET(COMPILE_DEFINITIONS_RELEASE -Os)

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/.
	${SDK_INCLUDE_DIRS}
	${SDK_LIB_INCLUDE_DIRS}
	${SDK_BLE_INCLUDE_DIRS}
	${SDK_BOOT_INCLUDE_DIRS}
	${SDK_CRYPTO_INCLUDE_DIRS}
	./src
	./src/devices
	./src/js
	./Espruino/src
	./Espruino/libs/math
	./Espruino/libs/compression
	../config
	)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf
	${NRF52_STARTUP}
	${SOURCE_FILES}
	${SDK_LIB_SOURCES}
	${SDK_BLE_SOURCES}
	${SDK_MOD_DRV_SOURCES}
	${NRF52_SYSTEM}
	${SDK_LOG_SOURCES}
	${ESPRUINO_FILES}
	${SDK_EXT_FPRINTF_SOURCES}
	)

TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME}.elf 
	m
	c
	nosys
	)

# Convert elf to hex & bin
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}.elf 
	POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Oihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
	)
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}.elf
	POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
	)

# adds target for erasing and flashing the board
ADD_CUSTOM_TARGET(flashsd
	COMMAND ${NRFJPROG} -f ${TARGET_PLATFORM} --eraseall
    COMMAND ${NRFJPROG} -f ${TARGET_PLATFORM} --program ${SOFTDEVICE_PATH}
    COMMENT "Flashing Softdevice"
    )

SET(S_PARAM --application-version 1 --bootloader-version 1 --bl-settings-version 1)
ADD_CUSTOM_TARGET(settings
    COMMAND ${NRFUTIL} settings generate --family ${TARGET_PLATFORM} --application ${CMAKE_PROJECT_NAME}.hex ${S_PARAM} ${SETTINGS_HEX}  	 
    COMMENT "Generate bootloader settings"
    )

# Create DFU package with version from tag    
SET(KEY_FILE ../config/private.pem)
ADD_CUSTOM_TARGET(dfu
	COMMAND git describe --tags
	EXECUTE_PROCESS(COMMAND git describe --tags
		OUTPUT_VARIABLE SW_VER)
#    COMMAND ${NRFUTIL} pkg generate --hw-version 52 --application ${CMAKE_PROJECT_NAME}.hex --key-file ${KEY_FILE} --sd-req 0x9D --application-version-string ${SW_VER} dfu.zip  	 
    COMMENT "Generate DFU package"
    )

# Derict FW flashing
ADD_CUSTOM_TARGET(flash
    COMMAND ${NRFJPROG} -f ${TARGET_PLATFORM} --program ${CMAKE_PROJECT_NAME}.hex --sectorerase
    COMMAND ${NRFJPROG} -f ${TARGET_PLATFORM} -r
    COMMENT "Flashing program"
    )

ADD_CUSTOM_TARGET(configure
    COMMAND java -jar ${SDK_CONFIG_TOOL} ${SDK_CONFIG_FILE}
    COMMENT "Configure SDK"
    )
