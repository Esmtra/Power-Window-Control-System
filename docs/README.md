# Power Window Control System

## Project Overview

The Power Window Control System is an advanced project designed to improve the safety, convenience, and functionality of automotive power windows. This system is built using the Tiva C microcontroller (TM4C123GH6PM) and FreeRTOS, offering features such as manual and one-touch control, obstacle detection, and a window lock function.

## Key Features

- **Obstacle Detection:** Ensures user safety by stopping the window if an obstacle is detected.
- **Manual and One-Touch Control:** Allows users to operate windows manually or with a single touch for full open/close actions.
- **Window Lock Function:** Enhances security by disabling window controls, preventing unauthorized use.

## System Components

- **Microcontroller:** Tiva-C (TM4C123GH6PM)
- **Actuators:** DC Motor controlled via H-Bridge
- **Sensors:** Limit switches for detecting window position
- **Control Interface:** On/Off switch push button for user input

## Technical Details

This system integrates hardware and software components to create a reliable and efficient control mechanism. The Tiva C microcontroller, running FreeRTOS, manages multiple tasks including motor control, sensor monitoring, and user input handling. The use of an H-Bridge allows for precise control over the motor direction and speed, ensuring smooth window operation.

## Project Documentation

The project is thoroughly documented in the provided [Power_Window_Control_System.pdf](docs/Power_Window_Control_System.pdf) report, which includes system layout, circuit diagrams, flowcharts, and detailed explanations of the code and hardware setup.

## Repository Structure

