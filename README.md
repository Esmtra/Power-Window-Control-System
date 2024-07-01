# Power Window Control System using Tiva C running FreeRTOS

## Introduction
In this project, our team developed a Power Window Control System utilizing the Tiva C Series TM4C123GH6PM microcontroller and FreeRTOS. The project demonstrates the capabilities of a real-time operating system (RTOS) in handling multiple tasks such as sensor input processing, motor control, and user interface management within an embedded system. This project highlights the importance of real-time task management, synchronization, and effective use of system resources in automotive safety applications. By integrating components such as DC motors, push buttons, and limit switches, the system mimics the operation of a full-scale power window, providing insights into the complexities of automotive control systems.

![Power Window Control System](https://github.com/Esmtra/Power-Window-Control-System/blob/main/diagrams/PWCS.png)

## System Requirements and Features
### 1. Dual Control Panels
- **Functionality**: Both the driver and front passenger have control panels capable of independently controlling the window.
- **Interactivity**: Controls include up, down, and stop functions for manual operation, with override functions for safety.

### 2. FreeRTOS Implementation
- **Task Management**: Multiple tasks run concurrently, including reading sensor data, controlling motor operations, and handling user interface inputs.
- **Synchronization**: Utilizes semaphores and mutexes to manage resource sharing and ensure the system operates without deadlock or resource starvation.

### 3. Safety Mechanisms
- **Limit Switches**: Positioned at both the top and bottom of the window trajectory to automatically stop the motor when the window reaches its full open or closed position.
- **Obstacle Detection**: Simulated using a push button that, when pressed, triggers the system to stop the window movement, mimicking the detection of an object obstructing the window path.

### 4. Functional Modes
- **Manual Mode**: Users can control the window manually by continuously pressing the control buttons.
- **Automatic Mode**: Features a one-touch operation where a single press fully opens or closes the window.
- **Lock Function**: Allows the driver to lock the passenger window controls, preventing operation from the passenger side.

## System Architecture
### Hardware Components
- **Tiva C Microcontroller**: Acts as the central control unit, managing inputs from sensors and user commands, and controlling the window motor.
- **DC Motor**: Drives the window mechanism, simulating the opening and closing of a car window.
- **Limit Switches**: Installed at the top and bottom of the window frame to detect the window's maximum open and closed positions.
- **Push Buttons**: Used for manual control of the window and simulating obstacle detection. Separate buttons are provided on both the driver and passenger control panels.

![Circuit Wiring Diagram](https://github.com/Esmtra/Power-Window-Control-System/blob/main/diagrams/circuit_wiring_diagram.png)

### Software Components
- **FreeRTOS**: Provides the framework for multitasking and task management.
- **Task Scheduling**: FreeRTOS schedules and manages all tasks, ensuring that higher-priority tasks preempt lower-priority tasks when necessary.
- **Semaphore and Mutex Handling**: Used to handle resource sharing between tasks, preventing race conditions.
- **Interrupt Service Routines (ISRs)**: Configured to handle inputs from the limit switches and obstacle detection button.

![Hardware Components](https://github.com/Esmtra/Power-Window-Control-System/blob/main/diagrams/HW.png)

### Integration and Communication
The system architecture is designed for seamless integration of hardware and software components. Tasks communicate via queues, sending messages between the control interface and motor control tasks to coordinate the window's movements. Mutexes and semaphores ensure that access to shared resources is properly synchronized among tasks.

## Implementation Details
### Code Structure and Task Implementation
- **Driver and Passenger Control Tasks**: Handle inputs from the driver and passenger control panels. They read the state of push buttons to either initiate or stop the movement of the window.
- **Window Motor Control Task**: Controls the DC motor based on commands received from the control tasks. It integrates feedback from the limit switches to stop the motor at the correct positions and handles the automatic movement logic.
- **Obstacle Detection Task**: Activated by a push button, this task takes the highest priority when triggered, immediately stopping the motor and initiating a reverse movement to free any detected obstacles.

### Resource Management
- **Semaphores and Mutexes**: Protect critical resources, such as the motor control interface, to avoid simultaneous access conflicts. Semaphores are used to signal between tasks, particularly from ISR routines to tasks that manage motor control and obstacle handling.
- **Queues**: Used for passing messages between tasks, ensuring organized and non-blocking communication.

### Interrupt Handling
- **Limit Switches and Obstacle Detection**: Interrupts are configured to respond to inputs from the limit switches and the obstacle detection button. The ISRs are designed to quickly handle these inputs, either by stopping the motor or by notifying the relevant tasks to take immediate action.

### Finite State Machine (FSM) Implementation
The system logic is governed by a Finite State Machine, which manages the states of the window (e.g., opening, closing, stopped). The FSM ensures smooth and logical transitions between states based on user inputs and sensor outputs.

![Simulink State Machine](https://github.com/Esmtra/Power-Window-Control-System/blob/main/diagrams/Simulink_SM.png)

### Tools
The FSM is visualized and tested using tools like MATLAB Simulink before implementation to ensure accuracy and reliability.

### Debugging and Validation
The system underwent rigorous testing, using both simulated inputs and real-world trials to validate each component’s functionality and the system’s overall reliability. Corner cases, such as simultaneous button presses and rapid command changes, were particularly focused on to ensure the system's robustness.

![Flowchart](https://github.com/Esmtra/Power-Window-Control-System/blob/main/diagrams/flowchart.png)

## References
1. Tiva™ C Series TM4C123GH6PM Microcontroller Data Sheet. Texas Instruments, 2021.
2. FreeRTOS User Guide.
