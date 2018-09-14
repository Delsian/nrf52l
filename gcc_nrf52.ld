/* Linker script to configure memory regions. */

SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x23000, LENGTH = 0x55000
  RAM (rwx) :  ORIGIN = 0x20003000, LENGTH = 0xd000
  BOOTLOADER (rx) : ORIGIN = 0x6D000, LENGTH = 0x11000
  
  script_page (r) : ORIGIN = 0x0006C000, LENGTH = 0x1000
  bootloader_settings_page (r) : ORIGIN = 0x0007F000, LENGTH = 0x1000
  mbr_params_page (r) : ORIGIN = 0x0007E000, LENGTH = 0x1000
  uicr_mbr_params_page (r) : ORIGIN = 0x10001018, LENGTH = 0x4
}

SECTIONS
{
  .bootloader_settings_page(NOLOAD) :
  {
    PROVIDE(__start_bootloader_settings_page = .);
    KEEP(*(SORT(.bootloader_settings_page*)))
    PROVIDE(__stop_bootloader_settings_page = .);
  } > bootloader_settings_page
  .script_page(NOLOAD) :
  {
    PROVIDE(__start_script_page = .);
    KEEP(*(SORT(.script_page*)))
    PROVIDE(__stop_script_page = .);
  } > script_page

}

SECTIONS
{
  . = ALIGN(4);
  .mem_section_dummy_ram :
  {
  }
  .log_dynamic_data :
  {
    PROVIDE(__start_log_dynamic_data = .);
    KEEP(*(SORT(.log_dynamic_data*)))
    PROVIDE(__stop_log_dynamic_data = .);
  } > RAM
  .cli_sorted_cmd_ptrs :
  {
    PROVIDE(__start_cli_sorted_cmd_ptrs = .);
    KEEP(*(.cli_sorted_cmd_ptrs))
    PROVIDE(__stop_cli_sorted_cmd_ptrs = .);
  } > RAM
  .fs_data :
  {
    PROVIDE(__start_fs_data = .);
    KEEP(*(.fs_data))
    PROVIDE(__stop_fs_data = .);
  } > RAM

} INSERT AFTER .data;

SECTIONS
{
  .mem_section_dummy_rom :
  {
  }
  .stext :
  {
    . = ALIGN(4);
    _stext = .;
    PROVIDE(stext = .);
  }
  .sdh_soc_observers :
  {
    PROVIDE(__start_sdh_soc_observers = .);
    KEEP(*(SORT(.sdh_soc_observers*)))
    PROVIDE(__stop_sdh_soc_observers = .);
  } > FLASH
  .pwr_mgmt_data :
  {
    PROVIDE(__start_pwr_mgmt_data = .);
    KEEP(*(SORT(.pwr_mgmt_data*)))
    PROVIDE(__stop_pwr_mgmt_data = .);
  } > FLASH
  .sdh_ble_observers :
  {
    PROVIDE(__start_sdh_ble_observers = .);
    KEEP(*(SORT(.sdh_ble_observers*)))
    PROVIDE(__stop_sdh_ble_observers = .);
  } > FLASH
  .log_const_data :
  {
    PROVIDE(__start_log_const_data = .);
    KEEP(*(SORT(.log_const_data*)))
    PROVIDE(__stop_log_const_data = .);
  } > FLASH
  .sdh_req_observers :
  {
    PROVIDE(__start_sdh_req_observers = .);
    KEEP(*(SORT(.sdh_req_observers*)))
    PROVIDE(__stop_sdh_req_observers = .);
  } > FLASH
  .sdh_state_observers :
  {
    PROVIDE(__start_sdh_state_observers = .);
    KEEP(*(SORT(.sdh_state_observers*)))
    PROVIDE(__stop_sdh_state_observers = .);
  } > FLASH
  .sdh_stack_observers :
  {
    PROVIDE(__start_sdh_stack_observers = .);
    KEEP(*(SORT(.sdh_stack_observers*)))
    PROVIDE(__stop_sdh_stack_observers = .);
  } > FLASH
    .cli_command :
  {
    PROVIDE(__start_cli_command = .);
    KEEP(*(.cli_command))
    PROVIDE(__stop_cli_command = .);
  } > FLASH
  .etext :
  {
    . = ALIGN(4);
    _etext = .;
    PROVIDE(etext = .);
  }
} INSERT AFTER .text

INCLUDE "nrf5x_common.ld"
