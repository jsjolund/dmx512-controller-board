# Sunshine DMX512 Controller Board
Controller board for DMX512 compatible lighting fixtures, built around the ARM Cortex-M4 processor (STM32F411RET).

On the hardware side, there are of four PCB:s, a main board containing the microcontroller as well as DMX512, USB USART and power circuitry. Other boards contain buttons, LCD, potentiometer sliders and similar. See the Kicad project files for details.

The software is built using the STM32 hardware abstraction layer (HAL). Aside from hardware drivers for the various input devices, it contains DMX512 generation and a simple menu system for the LCD.

![Enclosure](/Images/enclosure.jpg)

![Main PCB](/Images/main_board_pcb.jpg)

## STM32F411 docs
[Nucleo-64 board documentation](http://www.st.com/web/en/resource/technical/document/user_manual/DM00105823.pdf)

[STM32F411 Datasheet](http://www.st.com/resource/en/datasheet/stm32f411re.pdf)

[STM32F411 Reference Manual](http://www.st.com/resource/en/reference_manual/dm00119316.pdf)

[STM32F411 Programming Manual](http://www.st.com/resource/en/programming_manual/dm00046982.pdf)


