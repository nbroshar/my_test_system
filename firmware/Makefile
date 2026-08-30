CC = arm-none-eabi-gcc
CFLAGS = -mcpu=cortex-m7 -mthumb -ffreestanding -nostdlib -nostartfiles -O2 -Wall
LDFLAGS = -T linker.ld

blink.elf: blink.c linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) blink.c -o blink.elf
	arm-none-eabi-size blink.elf

clean:
	rm -f blink.elf
