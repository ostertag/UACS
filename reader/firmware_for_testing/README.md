### Deployment:
Deployment proces is a bit clunky, but it is reliable way to replicate this project



1. In STM32cubeIDE import project from .ioc file
2. Use .ioc file in this repo
3. Ovewrite functions files main.c and main.h with files in this repo
4. Add files rc522.c to Core/Src and rc522.h to Core/Inc
5. Ovewrite file usbd_cdc_if.c in USB_DEVICE/App with files in this repo
