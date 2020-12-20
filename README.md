# Space Invaders
This is our embedded software running on our TMS320F28379D microcontroller. The software is built on top of [FreeRTOS](https://www.freertos.org/) in C.

See our [full demo here](https://www.youtube.com/watch?v=ISFHsCCYF9k).

![Space Invaders Screen](https://user-images.githubusercontent.com/7267438/102722175-5400ce80-42cd-11eb-92a0-13b69c531328.PNG)


## Controller
The game was controlled with a wii nunchuck that was modified to be wireless using an xbee. The shooter moves left/right by moving the joystick and to shoot the trigger button is pressed.

The PCD is located in the [proto-controllerboard](https://github.com/Pixel-Pirates/proto-controllerboard) repository. Our custom fitted box for the PCB is located in the [Controller-Box-CAD](https://github.com/Pixel-Pirates/Controller-Box-CAD) repository.

![Wireless Controller](https://user-images.githubusercontent.com/7267438/102722288-35e79e00-42ce-11eb-8da2-6385197a118b.png)

## Display

The display is driven by a FPGA and is interfaced with this embedded code. See the VHDL driver at the [vga-driver](https://github.com/Pixel-Pirates/vga-driver) repository.

## Sound

Our speaker was driven by a DAC on our microcontroller and amplified by our OP AMP circuit. The sounds are stored in a sd card (see [sd_card](https://github.com/Pixel-Pirates/space-invaders/tree/master/sd_card) folder for contents), read from, and sent to DAC when needed. See the [speakerTask.c](https://github.com/Pixel-Pirates/space-invaders/blob/master/threads/speakerTask.c) for details.
