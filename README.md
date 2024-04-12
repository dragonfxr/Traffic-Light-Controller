# Project Description
The project involves programming a traffic light controller to manage the flow of vehicles entering a freeway. The system is designed to be implemented on Arduino Uno boards and programmed using Embedded C.

**The traffic light system has the following functions**:

### 1. Basic Traffic Light (Normal Mode):
- Lights cycle through red, green, and yellow states, changing every second.
- Uses LEDs to simulate the traffic lights, each connected to different digital IO pins on the Arduino.

### 2. Configurable Traffic Light (Configuration Mode):
- Enhances the basic system to allow for configurable light cycles with four different timing states.
- Entering the configuration mode is triggered by a button press, allowing cycle times to be adjusted through a potentiometer.

### 3. Speed Monitor:
- Estimates vehicle speeds using two simulated light barriers. 
- Speed is then displayed using a PWM signal

### 4. Red-Light Camera:
- Monitors compliance with the red light using an additional light barrier. If breached during a red signal, a camera (simulated by an LED) is triggered to simulate capturing a traffic violation.

# Technologies Used
- Programming in C/Embedded C
- Input/Output Management: Using digital IO pins, ADC inputs, LEDs
- AVR Microcontrollers Libraries
- Register Manipulation
- Analog-to-Digital Conversion (ADC)
- Timers

# Circuit and Boards Setups
### 1. Traffic Light System:
![image](https://github.com/dragonfxr/trafficLightController/assets/112178497/77f1db8d-c97f-4f26-9366-c6c4ebbf6bf6)

### 2. Speed Monitor System:
![image](https://github.com/dragonfxr/trafficLightController/assets/112178497/c74d4b98-71c8-4560-9da1-acd693ff81fd)

### 3. Red Light Camera:
![image](https://github.com/dragonfxr/trafficLightController/assets/112178497/8fd71ef9-7b1b-4063-8e0f-7406cbc350b3)


# Contact Me
If you have more advice on the project or you want to give a new engineer like me some hints, please contact me.

Email:russell.feng.au@gmail.com

I'm always open to feedback and collaboration. Looking forward to hearing and learning from you!
