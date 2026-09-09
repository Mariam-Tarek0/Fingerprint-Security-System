# Fingerprint-Based Security and Control System

## Overview
A security and hardware-control system built with **ATmega32** and **AS608 fingerprint sensor**, integrating external **EEPROM**, **LCD interface**, and **push-button navigation**. The system authenticates users via fingerprint, manages user data, and controls hardware devices securely.

## Features
- **Fingerprint Authentication**: Enroll, identify, and delete fingerprints.
- **User Management**: Store user names in external EEPROM linked to fingerprint IDs.
- **Security Logic**: Alarm and lockout after 3 failed attempts.
- **Hardware Control**: Authorized users can control LEDs and other devices.
- **LCD Interface**: Display menus, greetings, and error messages.

## System Architecture
- **Hardware**: ATmega32, AS608, EEPROM, LCD, Buttons, Buzzer, LEDs.
- **MCAL Layer**: DIO, UART, TWI/I²C, Interrupts.
- **HAL Layer**: Fingerprint, LCD, EEPROM, Buttons, Buzzer, Control, Users.
- **Application Layer**: Authentication flow, menu management, security logic.

## Project Details
The system identifies users through their fingerprint using the **AS608 sensor**.  
- If the fingerprint matches, the user is welcomed on the LCD and granted access to control hardware devices.  
- If the fingerprint fails three consecutive times, the system triggers a buzzer alarm and enforces a temporary lockout.  
- User names are stored in an external **24C64 EEPROM**, linked to fingerprint IDs, allowing personalized greetings and user management.  
- The layered architecture (MCAL → HAL → Application) ensures modularity, easier debugging, and scalability.  

## Hardware Components
- **ATmega32 Microcontroller**  
- **AS608 Fingerprint Sensor**  
- **24C64 EEPROM**  
- **16×2 LCD**  
- **Push Buttons**  
- **Buzzer & LEDs**  
