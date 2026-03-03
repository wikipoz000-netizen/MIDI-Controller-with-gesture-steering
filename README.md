# MIDI DJ Controller z kontrolą gestami

Projekt przedstawia autorski kontroler MIDI DJ wzbogacony o eksperymentalny system sterowania gestami. Urządzenie zostało opracowane w ramach pracy inżynierskiej na kierunku **Automatyka i Robotyka**.

## Opis projektu

Urządzenie łączy tradycyjne elementy kontrolera DJ (fadery, pokrętła, koła jog, przyciski) z warstwą bezdotykowego sterowania gestami opartą na ultradźwiękowych czujnikach odległości.

Kontroler komunikuje się z komputerem poprzez interfejs **USB-MIDI** i jest w pełni kompatybilny z oprogramowaniem DJ-skim, takim jak **Mixxx**.

## Sprzęt (Hardware)

Główne komponenty systemu:

* **Mikrokontroler:** Raspberry Pi Pico (RP2040)
* **Wejścia analogowe:** Multiplekser ADC (rozszerzenie liczby kanałów analogowych)
* **Wejścia cyfrowe:** Ekspander GPIO (dodatkowe wejścia dla przycisków)
* **Detekcja gestów:** Ultradźwiękowe czujniki odległości
* **Interfejs dotykowy:** Pojemnościowe czujniki dotyku
* **Konstrukcja:** Dedykowana płytka PCB oraz obudowa wykonana w technologii druku 3D

Pełna dokumentacja sprzętowa, schematy oraz projekty PCB znajdują się w katalogu `/hardware`.

## Oprogramowanie układowe (Firmware)

Firmware odpowiada za:

* Odczyt i filtrowanie sygnałów z wejść analogowych i cyfrowych.
* Skalowanie sygnałów i eliminację szumów.
* Interpretację danych z czujników na konkretne gesty.
* Generowanie komunikatów MIDI (**Note On/Off, Control Change, Pitch Bend**).
* Komunikację USB-MIDI o niskim opóźnieniu.

Kod źródłowy dostępny jest w katalogu `/firmware`.

## Integracja z oprogramowaniem

Kontroler został przetestowany z oprogramowaniem **Mixxx**. W repozytorium znajdują się niezbędne pliki mapowania MIDI (formaty `.xml` oraz `.js`).

## Zawartość repozytorium

* **Hardware:** Schematy ideowe i projekty płytek drukowanych.
* **Firmware:** Kod źródłowy mikrokontrolera.
* **Mapping:** Pliki konfiguracyjne MIDI dla oprogramowania DJ.
* **Dokumentacja:** Wyniki testów i specyfikacja techniczna.

---

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


---
## Licencja

Ten projekt jest udostępniony na licencji **Creative Commons Zero**. 
Więcej informacji w pliku [LICENSE](LICENSE).


## Author

Wiktor Pozarzycki  
Engineering Thesis Project – 2026
