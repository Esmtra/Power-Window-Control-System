
Window Motor Control using Tiva C Launchpad
This project is a Window Motor Control system that allows opening and closing windows automatically or manually. It uses a Tiva C Launchpad, a microcontroller from Texas Instruments, and FreeRTOS, a popular real-time operating system.

Table of Contents

Requirements
Installation
Usage
How it works
Project structure
Contributing
License
Requirements
Tiva C Launchpad
TivaWare software package
FreeRTOS v10.2.1
Code Composer Studio
Installation
Clone the repository or download the project as a ZIP file.
Open Code Composer Studio and create a new project using the files from the repository.
Make sure that the TivaWare software package and the FreeRTOS library are correctly installed and linked to the project.
Build and flash the project to the Tiva C Launchpad.
Usage
After uploading the code to the Tiva C Launchpad, the window motor control system can be used in two modes:

Automatic mode: The window motor opens and closes the window automatically based on the limit switches.
Manual mode: The user can open and close the window manually using the UP and DOWN push buttons.
To switch between the two modes, use the LOCK push button.

How it works
The system uses the following components:

1.INTERRUPTSInit() - This function initializes the interrupt controller of the microcontroller.

2.timer0Init() - This function initializes Timer0 of the microcontroller, which will be used for delays.

3.timer0_Delay(int time) - This function provides delay of 'time' milliseconds using Timer0.

4.motorInit() - This function initializes the GPIO pins of the microcontroller which are connected to the DC motor.

5.limitInit() - This function initializes the GPIO pins of the microcontroller which are connected to the limit switches.

6.buttonsInit() - This function initializes the GPIO pins of the microcontroller which are connected to the push buttons.

7.lock() - This task waits for a semaphore from ISR, and then checks whether the lock button is pressed or not. Depending on the value, the priority of the driver task is changed.

8.jamTask() - This task waits for a semaphore from ISR, and then turns the motor DOWN for 1 second.

9.recieveQueue() - This task waits for a queue from lower priority tasks. When a value is received, the state of the motor is determined.

10.driver() - This task takes a mutex upon entry, and then polls the UP and DOWN push buttons. If a button is pressed for less than 1 second, automatic mode is used. If it is pressed for more than 1 second, manual mode is used.

Two push buttons (UP and DOWN) for manual control.
Two limit switches to detect the window position.
One push button (LOCK) to switch between automatic and manual modes.
One motor to control the window position.
One red LED for indication.
The system consists of five tasks:

lock: This task waits for the LOCK push button to switch between the two modes. It changes the priority of the driver task based on the mode selected.

jamTask: This task waits for the semaphore from the ISR and turns the motor in the opposite direction for 1 second.

recieveQueue: This task waits for the queue from the driver task and sets the state of the motor accordingly.

driver: This task waits for the UP and DOWN push buttons to be pressed. If the button is pressed for less than 1 second, it switches to automatic mode. If the button is pressed for more than 1 second, it switches to manual mode.

Passenger: This task waits for the UP and DOWN push buttons to be pressed. If the button is pressed for less than 1 second, it switches to automatic mode. If the button is pressed for more than 1 second, it switches to manual mode.

Project structure
The project consists of the following files:

main.c: This file contains the main function and initializes the tasks, semaphores, mutex, queue, and interrupt handlers.
macros.h: This file contains the macros used in the project.
TM4C123GH6PM.h: This file contains the register definitions for the Tiva C Launchpad.
FreeRTOSConfig.h: This file contains the FreeRTOS configuration settings.

Contributing
Contributions are welcome! Please feel free to submit a Pull Request.

License
This project is licensed under the MIT License. See the LICENSE file for more information.