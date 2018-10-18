
PROJECT?=r0b1c
BUILD_DIR=./out
TOOLS_DIR=../tools
CONFIG_DIR=../config
SDK_DIR=../SDK15.2
ESP_DIR=./Espruino/src
SRC ?= ./src

#  Compiler/Assembler/Linker Paths
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
AR = $(PREFIX)ar
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# Default variables.
BUILD_TYPE ?= Debug

# Configure RTT based on BUILD_TYPE variable.
ifeq "$(BUILD_TYPE)" "Release"
OPTIMIZATION ?= 2
DEFINES += -DNRF_LOG_USES_RTT=0
endif

ifeq "$(BUILD_TYPE)" "Debug"
OPTIMIZATION = 0
DEFINES += -DNRF_LOG_USES_RTT=1
endif

ifneq "$(OS)" "Windows_NT"
COPY=cp
else
COPY=copy
endif

# Create macro which will convert / to \ on Windows.
ifeq "$(OS)" "Windows_NT"
define convert-slash
$(subst /,\,$1)
endef
else
define convert-slash
$1
endef
endif

# Some tools are different on Windows in comparison to Unix.
ifeq "$(OS)" "Windows_NT"
REMOVE = del
SHELL=cmd.exe
REMOVE_DIR = rd /s /q
MKDIR = mkdir
QUIET=>nul 2>nul & exit 0
BLANK_LINE=echo -
else
REMOVE = rm
REMOVE_DIR = rm -r -f
MKDIR = mkdir -p
QUIET=> /dev/null 2>&1 ; exit 0
BLANK_LINE=echo
endif

# Set VERBOSE make variable to 1 to output all tool commands.
VERBOSE?=0
ifeq "$(VERBOSE)" "0"
Q=@
else
Q=
endif

#########################################################################
# List of sources to be compiled/assembled
PROJ_SRCS = \
$(SRC)/main.c \
$(SRC)/bluetooth.c \
$(SRC)/buzzer.c \
$(SRC)/control.c \
$(SRC)/custom_service.c \
$(SRC)/fs.c \
$(SRC)/js_module.c \
$(SRC)/pca9685.c \
$(SRC)/r0b1c_cmd.c \
$(SRC)/r0b1c_service.c \
$(SRC)/devices/battery.c \
$(SRC)/devices/dummy.c \
$(SRC)/devices/gyro.c \
$(SRC)/devices/jswrap_led.c \
$(SRC)/devices/jswrap_motor.c \
$(SRC)/devices/jswrap_range.c \
$(SRC)/devices/motor.c \
$(SRC)/devices/r0b1c_device.c \
$(SRC)/devices/range.c \
$(SRC)/devices/rdev_btn.c \
$(SRC)/devices/rdev_led.c \
$(SRC)/devices/rj_port.c \
$(SRC)/js/jshardware.c \
$(SRC)/js/jswrap_board.c \
$(SRC)/js/jswrap_math.c \
$(SRC)/js/jswrapper.c

BLE_SRCS = \
$(SDK_DIR)/components/ble/ble_advertising/ble_advertising.c \
$(SDK_DIR)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
$(SDK_DIR)/components/ble/ble_services/ble_bas/ble_bas.c \
$(SDK_DIR)/components/ble/ble_services/ble_dis/ble_dis.c \
$(SDK_DIR)/components/ble/ble_services/ble_dfu/ble_dfu.c \
$(SDK_DIR)/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c \
$(SDK_DIR)/components/ble/common/ble_advdata.c \
$(SDK_DIR)/components/ble/common/ble_conn_params.c \
$(SDK_DIR)/components/ble/common/ble_conn_state.c \
$(SDK_DIR)/components/ble/common/ble_srv_common.c \
$(SDK_DIR)/components/softdevice/common/nrf_sdh.c \
$(SDK_DIR)/components/softdevice/common/nrf_sdh_ble.c

SDK_SRCS = \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_adc.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_clock.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_gpiote.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_pwm.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_saadc.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_twi.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_twim.c \
$(SDK_DIR)/modules/nrfx/drivers/src/nrfx_timer.c \
$(SDK_DIR)/modules/nrfx/mdk/system_nrf52.c \


LIB_SRCS = \
$(SDK_DIR)/components/libraries/atomic/nrf_atomic.c \
$(SDK_DIR)/components/libraries/atomic_flags/nrf_atflags.c \
$(SDK_DIR)/components/libraries/atomic_fifo/nrf_atfifo.c \
$(SDK_DIR)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
$(SDK_DIR)/components/libraries/balloc/nrf_balloc.c \
$(SDK_DIR)/components/libraries/crc32/crc32.c \
$(SDK_DIR)/components/libraries/scheduler/app_scheduler.c \
$(SDK_DIR)/components/libraries/fstorage/nrf_fstorage.c \
$(SDK_DIR)/components/libraries/fstorage/nrf_fstorage_sd.c \
$(SDK_DIR)/components/libraries/log/src/nrf_log_frontend.c \
$(SDK_DIR)/components/libraries/log/src/nrf_log_default_backends.c \
$(SDK_DIR)/components/libraries/log/src/nrf_log_backend_rtt.c \
$(SDK_DIR)/components/libraries/log/src/nrf_log_backend_serial.c \
$(SDK_DIR)/components/libraries/log/src/nrf_log_str_formatter.c \
$(SDK_DIR)/components/libraries/memobj/nrf_memobj.c \
$(SDK_DIR)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
$(SDK_DIR)/components/libraries/queue/nrf_queue.c \
$(SDK_DIR)/components/libraries/ringbuf/nrf_ringbuf.c \
$(SDK_DIR)/components/libraries/strerror/nrf_strerror.c \
$(SDK_DIR)/components/libraries/timer/app_timer.c \
$(SDK_DIR)/components/libraries/twi_mngr/nrf_twi_mngr.c \
$(SDK_DIR)/components/libraries/util/app_error_handler_gcc.c \
$(SDK_DIR)/components/libraries/util/app_util_platform.c \
$(SDK_DIR)/components/libraries/util/app_error.c \
$(SDK_DIR)/components/libraries/util/app_error_weak.c \
$(SDK_DIR)/components/libraries/experimental_section_vars/nrf_section_iter.c \
$(SDK_DIR)/integration/nrfx/legacy/nrf_drv_twi.c

ESP_SRCS = \
$(ESP_DIR)/jsflags.c \
$(ESP_DIR)/jslex.c \
$(ESP_DIR)/jsnative.c \
$(ESP_DIR)/jsparse.c \
$(ESP_DIR)/jsutils.c \
$(ESP_DIR)/jsvar.c \
$(ESP_DIR)/jsflash.c \
$(ESP_DIR)/jsvariterator.c \
$(ESP_DIR)/jswrap_array.c \
$(ESP_DIR)/jswrap_arraybuffer.c \
$(ESP_DIR)/jswrap_dataview.c \
$(ESP_DIR)/jswrap_date.c \
$(ESP_DIR)/jswrap_error.c \
$(ESP_DIR)/jswrap_functions.c \
$(ESP_DIR)/jswrap_json.c \
$(ESP_DIR)/jswrap_modules.c \
$(ESP_DIR)/jswrap_number.c \
$(ESP_DIR)/jswrap_object.c \
$(ESP_DIR)/jswrap_process.c \
$(ESP_DIR)/jswrap_promise.c \
$(ESP_DIR)/jswrap_regexp.c \
$(ESP_DIR)/jswrap_string.c 

LOG_SRCS += \
$(SDK_DIR)/external/segger_rtt/SEGGER_RTT.c \
$(SDK_DIR)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
$(SDK_DIR)/external/segger_rtt/SEGGER_RTT_printf.c \
$(SDK_DIR)/external/fprintf/nrf_fprintf.c \
$(SDK_DIR)/external/fprintf/nrf_fprintf_format.c

CSRCS = $(PROJ_SRCS) \
$(BLE_SRCS) \
$(SDK_SRCS) \
$(LIB_SRCS) \
$(ESP_SRCS) \
$(LOG_SRCS)

ASRCS = $(SDK_DIR)/modules/nrfx/mdk/gcc_startup_nrf52.S

#########################################################################
# AS includes
AS_INCLUDES =  \

# C includes
BLE_INCS = \
$(SDK_DIR)/components/ble/ble_advertising \
$(SDK_DIR)/components/ble/ble_db_discovery \
$(SDK_DIR)/components/ble/ble_services/ble_bas \
$(SDK_DIR)/components/ble/ble_services/ble_dfu \
$(SDK_DIR)/components/ble/ble_services/ble_dis \
$(SDK_DIR)/components/ble/common \
$(SDK_DIR)/components/ble/nrf_ble_gatt \
$(SDK_DIR)/components/ble/peer_manager \

LIB_INCS = \
$(SDK_DIR)/components/boards \
$(SDK_DIR)/components/libraries/atomic \
$(SDK_DIR)/components/libraries/atomic_fifo \
$(SDK_DIR)/components/libraries/atomic_flags \
$(SDK_DIR)/components/libraries/balloc \
$(SDK_DIR)/components/libraries/bootloader \
$(SDK_DIR)/components/libraries/bootloader/dfu \
$(SDK_DIR)/components/libraries/bootloader/ble_dfu \
$(SDK_DIR)/components/libraries/crc32 \
$(SDK_DIR)/components/libraries/delay \
$(SDK_DIR)/components/libraries/fstorage \
$(SDK_DIR)/components/libraries/experimental_section_vars \
$(SDK_DIR)/components/libraries/log \
$(SDK_DIR)/components/libraries/log/src \
$(SDK_DIR)/components/libraries/memobj \
$(SDK_DIR)/components/libraries/mutex \
$(SDK_DIR)/components/libraries/pwr_mgmt \
$(SDK_DIR)/components/libraries/queue \
$(SDK_DIR)/components/libraries/ringbuf \
$(SDK_DIR)/components/libraries/scheduler \
$(SDK_DIR)/components/libraries/strerror \
$(SDK_DIR)/components/libraries/svc \
$(SDK_DIR)/components/libraries/timer \
$(SDK_DIR)/components/libraries/twi_mngr \
$(SDK_DIR)/components/libraries/util \
$(SDK_DIR)/integration/nrfx/legacy


NRFX_INCS += \
$(SDK_DIR)/modules/nrfx \
$(SDK_DIR)/modules/nrfx/hal \
$(SDK_DIR)/modules/nrfx/drivers \
$(SDK_DIR)/modules/nrfx/drivers/include \
$(SDK_DIR)/modules/nrfx/mdk \

SDKINCS = \
$(BLE_INCS) \
$(NRFX_INCS) \
$(LIB_INCS) \
$(SDK_DIR)/external/segger_rtt \
$(SDK_DIR)/external/fprintf \
$(SDK_DIR)/components/toolchain/cmsis/include \
$(SDK_DIR)/components/softdevice/common \
$(SDK_DIR)/components/softdevice/s132/headers \
$(SDK_DIR)/components/softdevice/s132/headers/nrf52

JS_INCS = $(SRC)/js \
$(ESP_DIR) \
$(ESP_DIR)/../libs/math \
$(ESP_DIR)/../libs/compression \

INCDIRS = $(SRC) $(JS_INCS) $(SRC)/devices $(SDKINCS) $(CONFIG_DIR)

#########################################################################
# list of objects

# list of ASM program objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(ASRCS:.S=.o)))
vpath %.S $(sort $(dir $(ASRCS)))
# list of C program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CSRCS:.c=.o)))
vpath %.c $(sort $(dir $(CSRCS)))


DEVICE_FLAGS=-mcpu=cortex-m4 -mthumb
DEVICE_CFLAGS=$(DEVICE_FLAGS) -mthumb-interwork

# DEFINEs to be used when building C/C++ code
DEFINES += -DNRF52 -DNRF52832 -DBOARD_CUSTOM -DNRF52832_XXAA -DNRF52_PAN_74 -DSWI_DISABLE0
DEFINES += -DSOFTDEVICE_PRESENT -DS132 -DBLE_STACK_SUPPORT_REQD -DNRF_SD_BLE_API_VERSION=6
DEFINES += -DNRF_DFU_SVCI_ENABLED -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD

ifeq "$(OPTIMIZATION)" "0"
DEFINES += -DDEBUG
endif

# Compiler Options
GCFLAGS += -O$(OPTIMIZATION) -g3 $(DEVICE_CFLAGS)
GCFLAGS += -ffunction-sections -fdata-sections  -fno-exceptions -fno-strict-aliasing
GCFLAGS += -fno-delete-null-pointer-checks -fno-builtin -fshort-enums
GCFLAGS += $(patsubst %,-I%,$(INCDIRS))
GCFLAGS += $(DEFINES)
GCFLAGS += $(DEPFLAGS)
GCFLAGS += -Wall -Wno-unused-parameter
GCFLAGS += -Wno-pointer-sign

GPFLAGS += $(GCFLAGS) -std=gnu++11

AS_GCFLAGS += -g3 $(DEVICE_FLAGS) -x assembler-with-cpp
AS_GCFLAGS += $(patsubst %,-I%,$(INCDIRS))
AS_FLAGS += -g3 $(DEVICE_FLAGS)

# Linker script to be used.  Indicates what code should be placed where in memory.
LSCRIPT=gcc_nrf52.ld

# Linker Options.
LDFLAGS = $(DEVICE_FLAGS) --specs=nano.specs -mabi=aapcs
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map,--cref,--gc-sections
LDFLAGS += -T$(LSCRIPT) -L$(SDK_DIR)/modules/nrfx/mdk

# Libraries to be linked into final binary
LIBS = -lc -lnosys -lm

#########################################################################
#  Default rules to compile .c and .cpp file to .o
#  and assemble .s files to .o

$(BUILD_DIR)/%.o : %.S Makefile
	@echo Assembling $<
	$(Q) $(MKDIR) $(call convert-slash,$(dir $@)) $(QUIET)
	$(Q) $(CC) $(AS_GCFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o : %.c Makefile
	@echo Compiling $<
	$(Q) $(MKDIR) $(call convert-slash,$(dir $@)) $(QUIET)
	$(Q) $(CC) -c $(GCFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o : %.s Makefile
	@echo Assembling $<
	$(Q) $(MKDIR) $(call convert-slash,$(dir $@)) $(QUIET)
	$(Q) $(AS) $(AS_FLAGS) -o $@ $<

$(BUILD_DIR)/$(PROJECT).elf : $(OBJECTS) Makefile
	@echo $(LDFLAGS)
	$(Q) $(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	$(SZ) $@

%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@    

#########################################################################
# default action: build all

all: $(BUILD_DIR)/$(PROJECT).elf $(PROJECT).hex $(PROJECT).bin

.PHONY: flash dfu
print-%  : ; @echo $* = $($*)

clean:
	rm -fR .dep $(BUILD_DIR)

flash: $(PROJECT).hex
	echo 'ToDo'

dfu: $(PROJECT).bin
	echo 'ToDo'
