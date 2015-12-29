main.elf: main.c
	arm-eabi-gcc -g -I ../lpc2106_install/include main.c -L ../lpc2106_install/lib -Ttarget.ld -nostdlib -o main.elf -Wl,-Map,main.map

clean:
	rm *.elf
