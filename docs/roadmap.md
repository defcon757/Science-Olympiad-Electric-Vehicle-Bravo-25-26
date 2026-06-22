# Development Roadmap

## Completed

- Arduino Mega 2560 hardware platform
- TB6612FNG motor driver wiring
- Start button trigger
- Encoder interrupt counting
- Distance calculation
- Straight-line movement routine
- Early-stop and trim movement strategy
- Active braking
- Timed right and left turn helpers

## In Progress

- Encoder calibration
- Straight-line tuning
- Stop accuracy tuning
- Turn duration tuning

## Next Improvements

### OLED Interface

Display:

```text
State
Left encoder
Right encoder
Target steps
Target distance
Distance traveled
```

### Full Quadrature Encoder Support

Use both A and B encoder channels per motor so the robot can count forward and reverse motion.

### Encoder-Based Turns

Replace:

```cpp
turnRight(850);
```

with:

```cpp
turnRightDegrees(90);
```

### Trapezoidal Velocity Profile

Future movement profile:

```text
Accelerate
Cruise
Decelerate
Trim
Stop
```

Expected benefits:

- Less wheel slip
- Better repeatability
- More predictable stopping

### PID Straight-Line Correction

Use encoder difference to adjust left and right motor PWM continuously.

### Obstacle Navigation

Event goal:

```text
Drive forward
Navigate around center obstacle
Return to centerline
Stop at target
```

