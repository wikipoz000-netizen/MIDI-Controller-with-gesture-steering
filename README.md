# MIDI DJ Controller with Gesture Control

This project presents a custom-built MIDI DJ controller enhanced with an experimental gesture control system.  
It was developed as part of an engineering thesis in Robotics and Automation.

## Project Description

The device combines traditional DJ controller elements (faders, knobs, jog wheels, buttons) with a contactless gesture control layer based on ultrasonic distance sensors.

The controller communicates with a computer via USB-MIDI and is compatible with DJ software such as Mixxx.

## Hardware

Main components:

- Raspberry Pi Pico (RP2040)
- ADC multiplexer (for extended analog inputs)
- GPIO expander (for additional digital inputs)
- Ultrasonic distance sensors (gesture detection)
- Capacitive touch sensing
- Custom-designed PCB
- 3D-printed enclosure

All hardware schematics and PCB files are available in the `/hardware` directory.

## Firmware

The firmware is responsible for:

- Reading analog and digital inputs
- Signal filtering and scaling
- Gesture interpretation
- Generating MIDI messages (Note On/Off, Control Change, Pitch Bend)
- USB-MIDI communication

Source code is available in the `/firmware` directory.

## Software Integration

The controller was tested with Mixxx DJ software.  
The repository includes MIDI mapping files (XML and JS).

## Repository Contents

- Hardware schematics
- PCB design files
- Firmware source code
- MIDI mapping configuration
- Documentation and test results

## Author

Wiktor Pożarzycki  
Engineering Thesis Project – 2026
