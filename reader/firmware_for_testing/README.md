### Deployment:
Deployment proces is a bit clunky, but it is reliable way to replicate this project



1. In STM32cubeIDE, choose import and then import from .ioc file
2. Use .ioc file in this repo
3. Ovewrite functions files main.c and main.h in Core/src and Core/Inc with files in this repo
4. Add files rc522.c to Core/Src and rc522.h to Core/Inc
5. Ovewrite file usbd_cdc_if.c in USB_DEVICE/App with files in this repo


### Manual:

1. After flashing firmware on Reader, reconect reader to the computer (unplug and plug usb cable)
2. One your favorite serial terimal and connect to the device.
3. If menu doesnt appears pres "q" or "Q" or enter
4. Then read instructions on the screen
5. If menu is distorted, press "q" or "Q" or enter to reaload menu
6. "q" or "Q" also ends procceses: pwm modulation, uart reception and rc522 card reading
7. Commands for choices are case sensitive 
