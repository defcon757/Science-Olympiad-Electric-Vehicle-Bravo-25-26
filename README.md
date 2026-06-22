# Arduino Mega EV Science Olympiad 2025-2026

Precision encoder-based electric vehicle platform for the Science Olympiad Electric Vehicle event.

This project uses an Arduino Mega 2560, TB6612FNG dual motor driver, DC gear motors with encoders, and a start button to drive an autonomous vehicle a commanded distance with repeatable stopping accuracy. Version 3.0 currently focuses on straight-line distance travel, encoder-based wheel balancing, active braking, trimming, and timed turns.

## Current Features

- Arduino Mega 2560 control
- TB6612FNG dual motor driver support
- Encoder interrupt counting on both drive motors
- Closed-loop straight-line balancing using encoder speed feedback
- Distance-based movement in millimeters
- Early-stop plus low-speed trim phase for improved accuracy
- Balanced active braking
- Button-triggered autonomous run sequence
- Serial Monitor diagnostics

## Hardware

| Component | Model / Notes |
| --- | --- |
| Microcontroller | Arduino Mega 2560 |
| Motor driver | TB6612FNG dual H-bridge |
| Motors | Two DC gear motors with encoders |
| Display | Optional SH1106 128x64 I2C OLED |
| Input | Momentary push button |
| Power | External motor battery supply with common ground |

## Version 3.0 Pin Mapping

| Signal | Arduino Mega Pin |
| --- | --- |
| Motor A IN1 | D5 |
| Motor A IN2 | D6 |
| Motor A ENA / PWM | D7 |
| Motor B IN3 | D8 |
| Motor B IN4 | D9 |
| Motor B ENB / PWM | D10 |
| Start Button | D27 to GND |
| Encoder A interrupt | D2 |
| Encoder B interrupt | D18 |

Older development notes used D13 for the start button and documented both encoder channels per wheel. The current working Version 3.0 sketch uses D27 for the start button and counts one interrupt signal from each motor.

## Mechanical Calibration

Current Version 3.0 constants:

```cpp
const float stepsPerRotation = 215.0;
const float mmPerRotation = 223.05;
const float stepsPerMM = stepsPerRotation / mmPerRotation;
```

These values should be recalibrated whenever wheel diameter, tire grip, battery voltage, motor gearing, or vehicle mass changes.

## Repository Layout

```text
.
|-- src/
|   `-- EV_Science_Olympiad_V3.ino
|-- examples/
|   |-- button_test/
|   |-- motor_test/
|   `-- oled_test/
|-- docs/
|   |-- calibration.md
|   |-- hardware.md
|   `-- roadmap.md
|-- .gitignore
|-- LICENSE
`-- README.md
```

## Uploading

1. Open `src/EV_Science_Olympiad_V3.ino` in the Arduino IDE.
2. Select `Arduino Mega or Mega 2560`.
3. Select the correct serial port.
4. Upload the sketch.
5. Open Serial Monitor at `115200` baud.
6. Press the start button to run the programmed movement sequence.

## Default Movement Sequence

The Version 3.0 sketch currently runs:

```cpp
moveDistanceMM(3900);
delay(1000);
turnRight(850);
delay(1000);
moveDistanceMM(500);
```

This was noted as working for a 4.3 meter target path. Adjust the distances and turn duration for each event setup.

## Development Roadmap

- Recheck encoder counts per wheel rotation
- Tune `undershootPercent`, trim speed, and braking time
- Add OLED status screens
- Replace timed turns with encoder-based turns
- Add a true trapezoidal velocity profile
- Add PID straight-line correction
- Build obstacle navigation around the center can
