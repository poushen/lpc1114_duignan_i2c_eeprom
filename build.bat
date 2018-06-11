arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -g -c init.c -o .\out\init.o
arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -I. -g -c .\driver\led.c -o .\out\led.o
arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -I. -g -c .\driver\uart.c -o .\out\uart.o
arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -I. -I.\driver -g -c main.c -o .\out\main.o
arm-none-eabi-ld .\out\init.o .\out\led.o .\out\uart.o .\out\main.o -L "C:\Program Files (x86)\GNU Tools ARM Embedded\5.4 2016q2\lib\gcc\arm-none-eabi\5.4.1\armv6-m" -lgcc -T linker_script.ld --cref -Map .\out\main.map -nostartfiles -o .\out\main.elf
arm-none-eabi-objcopy -O ihex .\out\main.elf .\out\main.hex
