# Hardware Notes

## Core Parts

- Arduino Mega 2560
- TB6612FNG dual motor driver
- Two DC gear motors with encoders
- Momentary start button
- External motor battery supply
- Optional SH1106 128x64 I2C OLED display

## Motor Driver Wiring

| TB6612FNG Pin | Arduino Mega Pin | Purpose |
| --- | --- | --- |
| IN1 | D5 | Motor A direction |
| IN2 | D6 | Motor A direction |
| ENA | D7 | Motor A PWM |
| IN3 | D8 | Motor B direction |
| IN4 | D9 | Motor B direction |
| ENB | D10 | Motor B PWM |
| VCC | 5V | Logic power |
| VM | Battery positive | Motor power |
| GND | Common ground | Shared reference |

The current motor driver board does not expose a separate STBY pin.

## Start Button

Version 3.0 uses:

| Button Side | Connection |
| --- | --- |
| Signal | D27 |
| Other side | GND |

The sketch uses `INPUT_PULLUP`, so the logic is:

```text
LOW  = pressed
HIGH = released
```

## Encoder Wiring

Version 3.0 counts one interrupt channel from each motor:

| Encoder Signal | Arduino Mega Pin |
| --- | --- |
| Motor A encoder | D2 |
| Motor B encoder | D18 |

Earlier notes used full quadrature wiring:

| Encoder Signal | Arduino Mega Pin |
| --- | --- |
| Left A | D2 |
| Left B | D3 |
| Right A | D18 |
| Right B | D19 |

Full quadrature support is a good future upgrade because it allows direction-aware counting.

## Optional OLED Wiring

| OLED Pin | Arduino Mega Pin |
| --- | --- |
| VCC | 5V |
| GND | GND |
| SDA | D20 |
| SCL | D21 |

Recommended libraries:

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
```

Display object:

```cpp
Adafruit_SH1106G display(128, 64, &Wire, -1);
```

Common I2C address:

```cpp
0x3C
```

## Power Notes

Motor current spikes can reset the Arduino or corrupt the OLED display. Use:

- Separate motor and logic power when possible
- Common ground between Arduino and motor driver
- A bulk capacitor across motor supply and ground

Suggested capacitor:

```text
470 uF to 1000 uF
```

