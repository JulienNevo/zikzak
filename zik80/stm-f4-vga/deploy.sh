#!/bin/sh

# after "init" can do this..
#                     -c "stm32f2x mass_erase 0" \
#                    -c "flash erase_address 0x08000000 0x20000" \
#                    -c "flash erase_address pad 0x08000000 0x1000" \
#                    -c "flash erase_address pad 0x8001144 0x1000" \
# flash auto_erase on|off
#                    -c "flash auto_erase on" \

openocd -f interface/stlink-v2.cfg \
                    -f board/stm32f4discovery.cfg \
                    -c "init" -c "reset init" \
                    -c "flash list" \
                    -c "flash erase_address pad 0x8000000 0x2000" \
                    -c "flash write_image vgatest.hex" \
                    -c "reset run" \
                    -c "shutdown"