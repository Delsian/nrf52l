PROJECT_NAME     := nrf52l
TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build
SDK_ROOT := $(NRF52_SDK)
PROJ_DIR := .

$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(PROJ_DIR)/src/main.c \
  $(PROJ_DIR)/src/bluetooth.c \
  $(PROJ_DIR)/src/swo_log.c \
  $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/experimental_memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs/ble_lbs.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas/ble_bas.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/external/freertos/source/croutine.c \
  $(SDK_ROOT)/external/freertos/source/list.c \
  $(SDK_ROOT)/external/freertos/source/queue.c \
  $(SDK_ROOT)/external/freertos/source/tasks.c \
  $(SDK_ROOT)/external/freertos/source/timers.c \
  $(SDK_ROOT)/external/freertos/source/portable/MemMang/heap_4.c \
  $(SDK_ROOT)/external/freertos/portable/GCC/nrf52/port.c \
  $(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52/port_cmsis.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components/drivers_nrf/comp \
  $(SDK_ROOT)/components/drivers_nrf/twi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/libraries/experimental_log \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/drivers_nrf/i2s \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries\bsp \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/libraries/experimental_log/src \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/boards \
  $(SDK_ROOT)/components/libraries/experimental_memobj \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_db_discovery \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/libraries/pwr_mgmt \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/drivers_nrf/wdt \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus_c \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components/libraries/ecc \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/components/drivers_nrf/twis_slave \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/cli \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/drivers_nrf/pwm \
  ./config \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/drivers_nrf/rng \
  $(SDK_ROOT)/components/drivers_nrf/power \
  $(SDK_ROOT)/components/libraries/usbd/config \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/drivers_nrf/qdec \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/libraries/twi_mngr \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/twi \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/drivers_nrf/pdm \
  $(SDK_ROOT)/components/drivers_nrf/usbd \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/drivers_nrf/spi_slave \
  $(SDK_ROOT)/components/drivers_nrf/lpcomp \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/drivers_nrf/swi \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/drivers_nrf/saadc \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/ble/ble_services/ble_nus \
  $(SDK_ROOT)/external/freertos/portable/CMSIS/nrf52 \
  $(SDK_ROOT)/external/freertos/portable/GCC/nrf52 \
  $(SDK_ROOT)/external/freertos/source/include \

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBOARD_CUSTOM
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DENABLE_DEBUG_LOG_SUPPORT
CFLAGS += -DENABLE_SWO
CFLAGS += -DNRF_LOG_USES_RTT=1
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF_SD_BLE_API_VERSION=5
CFLAGS += -DS132
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS +=  -Wall -Werror
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums 

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52832_XXAA
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=5
ASMFLAGS += -DS132
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSWI_DISABLE0

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs


# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52832_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52832_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase

# Flash the program
flash: $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex
	@echo Flashing: $<
	nrfjprog -f nrf52 --program $< --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s132_nrf52_5.0.0_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_5.0.0_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ./config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
