# Calibration Guide

Accurate distance travel depends on measuring the actual relationship between encoder counts and vehicle movement.

## Current Version 3.0 Values

```cpp
const float stepsPerRotation = 215.0;
const float mmPerRotation = 223.05;
const float stepsPerMM = stepsPerRotation / mmPerRotation;
```

This means:

```text
215 encoder counts = 1 wheel rotation
1 wheel rotation = 223.05 mm of travel
```

## Recalibrating Encoder Counts

1. Mark the wheel with tape.
2. Rotate the wheel exactly one full revolution.
3. Read the encoder count from Serial Monitor.
4. Repeat at least five times.
5. Average the counts.
6. Update `stepsPerRotation`.

## Recalibrating Distance Per Rotation

1. Mark the floor and wheel start point.
2. Roll the vehicle forward for 5 to 10 wheel rotations.
3. Measure total distance traveled.
4. Divide by the number of rotations.
5. Update `mmPerRotation`.

Example:

```text
Measured distance: 1115.25 mm
Wheel rotations: 5

mmPerRotation = 1115.25 / 5
mmPerRotation = 223.05
```

## Distance Formula

```cpp
targetSteps = distanceMM * stepsPerMM;
```

where:

```cpp
stepsPerMM = stepsPerRotation / mmPerRotation;
```

## Tuning Stop Accuracy

The current sketch uses an early-stop point and then trims slowly:

```cpp
const float undershootPercent = 0.75;
```

If the vehicle stops too short, increase this value. If it overshoots before trim, decrease it.

Tune in this order:

1. `stepsPerRotation`
2. `mmPerRotation`
3. `moveMinPWM` and `moveMaxPWM`
4. active brake duration
5. `undershootPercent`
6. trim PWM
7. steering trim

## Test Log Template

```text
Date:
Battery voltage:
Target distance:
Actual distance:
Error:
Surface:
stepsPerRotation:
mmPerRotation:
undershootPercent:
moveMinPWM:
moveMaxPWM:
trimMinPWM:
trimMaxPWM:
Notes:
```

