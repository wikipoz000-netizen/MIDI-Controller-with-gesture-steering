# MIDI-Controller-with-gesture-steering
The goal of this project was to design and implement a practical MIDI controller enhanced with an experimental gesture-based control system. The repository contains all schematics, PCB files, firmware source code, and configuration files required to reproduce and further develop the device.

# 🎛 MIDI DJ Controller with Gesture Control System

A fully functional DIY MIDI DJ controller extended with a contactless gesture control system.

This project was developed as an engineering thesis in Robotics and Automation and combines embedded systems design, PCB development, firmware implementation, and human–machine interaction concepts.

---

## 📌 Project Overview

The aim of this project was to design and build a professional-grade MIDI controller compatible with DJ software, extended with an innovative gesture-based control layer.

The device integrates traditional DJ hardware components (faders, jog wheels, pads, knobs) with ultrasonic distance sensors enabling contactless gesture interaction.

The controller communicates via USB-MIDI and is fully compatible with software such as Mixxx.

---

## 🚀 Key Features

- 🎚 Dual-deck DJ controller architecture
- 🎛 Jog wheels with touch detection
- 🎚 Channel faders and crossfader
- 🎛 EQ and filter control
- 🔘 Performance pads
- ✋ Contactless gesture control using ultrasonic sensors
- ⚡ Low-latency USB-MIDI communication
- 🧠 Custom firmware with signal filtering and state management
- 🖨 Custom PCB design
- 🧩 3D printed enclosure

---

## 🧠 System Architecture

The system is based on:

- **Microcontroller:** Raspberry Pi Pico (RP2040)
- **ADC Multiplexer:** Expands analog input channels
- **GPIO Expander:** Extends digital inputs
- **Capacitive touch sensing**
- **Ultrasonic distance sensors** for gesture detection
- **USB-MIDI communication layer**

High-level architecture:


The firmware handles:

- Analog signal filtering
- Quadrature encoder decoding
- Gesture distance measurement
- State management and mode switching
- MIDI message generation (Note On/Off, Control Change, Pitch Bend)

---

## ✋ Gesture Control System

The gesture control layer enables real-time modulation of selected DJ parameters without physical contact.

Implemented using ultrasonic distance sensors:

- Hand distance mapped to continuous MIDI CC values
- Suitable for:
  - Filter sweeps
  - Effect modulation
  - Volume transitions
  - Creative live performance interaction

Latency was measured and analyzed to ensure real-time usability.

---

## 📊 Latency Analysis

The project includes:

- ADC multiplexer delay testing
- Stability testing
- MIDI transmission timing
- Theoretical total system latency estimation

Results confirmed suitability for real-time DJ performance.

---

## 🛠 Hardware Design

The hardware development included:

- Custom schematic design
- PCB layout and routing
- Power regulation and signal integrity considerations
- 3D enclosure design
- Manual assembly and soldering

All hardware files are available in the `/hardware` directory.

---

## 💻 Firmware

The firmware is responsible for:

- Input acquisition
- Signal scaling and filtering
- MIDI message formatting
- USB communication
- Gesture interpretation logic

Written in C/C++ for Raspberry Pi Pico.

---

## 🎧 Software Integration

Tested with:

- Mixxx (open-source DJ software)

Includes:

- Static XML MIDI mapping
- Advanced JavaScript control logic

Mapping files are available in `/mixxx_mapping`.

---

## 📷 Project Gallery



---

## 📚 Engineering Thesis Context

This project was developed as part of an Engineering Thesis in Robotics and Automation.

**Title:**  
"Design and Implementation of a MIDI Controller with a Gesture Control System"

The work included:

- Analysis of commercial DJ controllers
- Evaluation of open-source reference projects
- Hardware and firmware development
- Functional and latency testing
- Human–Machine Interface analysis

---

## 🔮 Future Improvements

- Higher precision gesture sensing (ToF sensors)
- OLED status display
- LED feedback system
- Multi-layer MIDI modes
- Wireless MIDI (BLE-MIDI)

---

## 🧩 Educational Value

The project demonstrates integration of:

- Embedded systems
- PCB design
- Signal processing
- USB communication protocols
- Human–Machine Interaction
- Real-time system constraints

---

## 👤 Author

Wiktor Pożarzycki  
Robotics & Automation Engineer  

---

