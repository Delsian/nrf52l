arm-none-eabi-objcopy -O ihex "Debug/nrf52l.elf" nrf52l.hex
nrfjprog --program nrf52l.hex --sectorerase
nrfjprog --reset
