# Pathfinder to do list:

Contains details on the current project progress, such as future plans, in
progress tasks and notes for future reference.

## General

### Todo

- [ ] Fill out README.md

## STM32F401:

### Todo

- [ ] Move away from using STM32CubeIDE to something like Segger Embedded Studio
- [ ] Add more features to logging and/or look into how proper logging is done 
- [ ] Reduce the number of magic numbers in the code
- [ ] Fill out all function descriptions

### In Progress

- [ ] Testing individual hardware components and sensors
	- [x] Test basic FreeRTOS functionality
	- [x] Test I2C/UART control
	- [x] Interface with GY-88
	- [x] Get current position using the MPU6050
	- [x] Interface with the AS5600 encoder
		- [x] Power a motor with PWM
	- [ ] Get current speed using the AS5600

### Done

- [x] Create basic logging function
