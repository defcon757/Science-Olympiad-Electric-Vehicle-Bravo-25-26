#include <Arduino.h>

/* ================= MOTOR PINS ================= */
const int IN1 = 5;
const int IN2 = 6;
const int ENA = 7;

const int IN3 = 8;
const int IN4 = 9;
const int ENB = 10;

/* ================= START BUTTON ================= */
const int START_BUTTON = 27;   // Button between D27 and GND

/* ================= ENCODER PINS ================= */
const int ENC_A = 2;
const int ENC_B = 18;

volatile long encoderCountA = 0;
volatile long encoderCountB = 0;

/* ================= MECHANICAL PARAMETERS ================= */
const float stepsPerRotation = 215.0;
const float mmPerRotation = 223.05;
const float stepsPerMM = stepsPerRotation / mmPerRotation;

/* ================= SPEED SETTINGS ================= */
const int moveMinPWM = 60;
const int moveMaxPWM = 120;

const int turnPWM = 120;

const int trimMinPWM = 50;
const int trimMaxPWM = 80;

const float undershootPercent = 0.75;

float steeringTrim = 0.0;

/* ================= ENCODER ISRS ================= */
void ISR_encoderA() { encoderCountA++; }
void ISR_encoderB() { encoderCountB++; }

/* ================= MOTOR CONTROL ================= */
void coastMotors();
void brakeMotors();

void turnLeft(unsigned long durationMs) {
  Serial.print("[");
  Serial.print(millis());
  Serial.println(" ms] Turning LEFT");

  analogWrite(ENB, 0);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, turnPWM);

  delay(durationMs);

  brakeMotors();
  delay(100);
  coastMotors();

  Serial.print("[");
  Serial.print(millis());
  Serial.println(" ms] Left turn complete");
}

void turnRight(unsigned long durationMs) {
  Serial.print("[");
  Serial.print(millis());
  Serial.println(" ms] Turning RIGHT");

  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, turnPWM);

  delay(durationMs);

  brakeMotors();
  delay(100);
  coastMotors();

  Serial.print("[");
  Serial.print(millis());
  Serial.println(" ms] Right turn complete");
}

/* ================= DRIVE FORWARD WITH BALANCING ================= */
void driveForwardBalanced(int basePWM) {
  static long lastCountA = 0;
  static long lastCountB = 0;
  static unsigned long lastTime = 0;

  long currentA, currentB;
  unsigned long currentTime = millis();

  noInterrupts();
  currentA = encoderCountA;
  currentB = encoderCountB;
  interrupts();

  long deltaA = currentA - lastCountA;
  long deltaB = currentB - lastCountB;

  unsigned long deltaTime = currentTime - lastTime;
  if (deltaTime == 0) deltaTime = 1;

  float speedA = (deltaA * 1000.0) / deltaTime;
  float speedB = (deltaB * 1000.0) / deltaTime;

  lastCountA = currentA;
  lastCountB = currentB;
  lastTime = currentTime;

  float speedError = speedA - speedB;
  speedError -= steeringTrim;

  float Kp = 0.05;
  int correction = (int)(Kp * speedError);

  int pwmA = basePWM - correction;
  int pwmB = basePWM + correction;

  pwmA = constrain(pwmA, moveMinPWM, moveMaxPWM);
  pwmB = constrain(pwmB, moveMinPWM, moveMaxPWM);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, pwmA);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, pwmB);

  Serial.print("SpeedA: ");
  Serial.print(speedA);
  Serial.print(" cps | SpeedB: ");
  Serial.print(speedB);
  Serial.print(" cps | PWM A: ");
  Serial.print(pwmA);
  Serial.print(" | PWM B: ");
  Serial.println(pwmB);
}

/* ================= ACTIVE BRAKE WITH REVERSE ================= */
void brakeMotors() {
  Serial.print("[");
  Serial.print(millis());
  Serial.println(" ms] Applying balanced active brake...");

  static long lastCountA = 0;
  static long lastCountB = 0;
  static unsigned long lastTime = 0;

  lastTime = millis();
  unsigned long brakeStart = millis();

  while (millis() - brakeStart < 120) {
    long currentA, currentB;
    unsigned long currentTime = millis();

    noInterrupts();
    currentA = encoderCountA;
    currentB = encoderCountB;
    interrupts();

    long deltaA = currentA - lastCountA;
    long deltaB = currentB - lastCountB;

    unsigned long deltaTime = currentTime - lastTime;
    if (deltaTime == 0) deltaTime = 1;

    float speedA = (deltaA * 1000.0) / deltaTime;
    float speedB = (deltaB * 1000.0) / deltaTime;

    lastCountA = currentA;
    lastCountB = currentB;
    lastTime = currentTime;

    float speedError = speedA - speedB;

    float KpBrake = 0.08;
    int correction = (int)(KpBrake * speedError);

    int brakeA = 140 - correction;
    int brakeB = 140 + correction;

    brakeA = constrain(brakeA, 80, 200);
    brakeB = constrain(brakeB, 80, 200);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, brakeA);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, brakeB);
  }

  coastMotors();

  Serial.print("[");
  Serial.print(millis());
  Serial.println(" ms] Balanced brake complete.");
}

void coastMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

/* ================= WAIT FOR COMPLETE STOP ================= */
void waitForCompleteStop(unsigned long stableTimeMs) {
  Serial.println("Verifying complete stop...");

  long lastA, lastB;
  noInterrupts();
  lastA = encoderCountA;
  lastB = encoderCountB;
  interrupts();

  unsigned long stableStart = millis();

  while (true) {
    long currentA, currentB;
    noInterrupts();
    currentA = encoderCountA;
    currentB = encoderCountB;
    interrupts();

    if (currentA != lastA || currentB != lastB) {
      stableStart = millis();
      lastA = currentA;
      lastB = currentB;
    }

    if (millis() - stableStart >= stableTimeMs) break;
    delay(10);
  }

  Serial.println("Confirmed: Robot fully stopped.");
}

/* ================= MOVE DISTANCE ================= */
void moveDistanceMM(float distanceMM) {
  long finalTarget = lround(distanceMM * stepsPerMM);
  long earlyStopTarget = finalTarget * undershootPercent;

  Serial.println("================================");
  Serial.print("Requested Distance (mm): ");
  Serial.println(distanceMM);
  Serial.print("Calculated Steps: ");
  Serial.println(finalTarget);
  Serial.print("Early Stop Steps: ");
  Serial.println(earlyStopTarget);
  Serial.println("================================");

  noInterrupts();
  encoderCountA = 0;
  encoderCountB = 0;
  interrupts();

  bool mainStopped = false;
  bool trimming = false;
  bool complete = false;
  int currentPWM = moveMinPWM;

  unsigned long lastPrint = 0;

  while (!complete) {
    long countA, countB;
    noInterrupts();
    countA = encoderCountA;
    countB = encoderCountB;
    interrupts();

    long progress = max(countA, countB);

    if (!mainStopped) {
      currentPWM += 1;
      currentPWM = constrain(currentPWM, moveMinPWM, moveMaxPWM);

      driveForwardBalanced(currentPWM);

      if (progress >= earlyStopTarget) {
        Serial.print("[");
        Serial.print(millis());
        Serial.println(" ms] Early stop reached.");

        brakeMotors();
        delay(150);
        coastMotors();

        mainStopped = true;
        waitForCompleteStop(3000);

        trimming = true;
        Serial.println("Starting trim phase...");
      }
    }

    if (trimming && !complete) {
      if (progress < finalTarget) {
        int trimPWM = constrain(trimMinPWM, trimMinPWM, trimMaxPWM);
        driveForwardBalanced(trimPWM);
      } else {
        brakeMotors();
        delay(100);
        coastMotors();

        trimming = false;
        complete = true;

        Serial.print("[");
        Serial.print(millis());
        Serial.println(" ms] Final target reached.");
      }
    }

    if (millis() - lastPrint >= 100) {
      Serial.print("[");
      Serial.print(millis());
      Serial.print(" ms] ");
      Serial.print("Avg Steps: ");
      Serial.print(progress);
      Serial.print(" / ");
      Serial.println(finalTarget);
      lastPrint = millis();
    }
  }
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(START_BUTTON, INPUT_PULLUP);

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A), ISR_encoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_B), ISR_encoderB, RISING);

  Serial.println("Distance-Based Movement System Ready");

  delay(1000);
}

/* ================= LOOP ================= */
void loop() {
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(START_BUTTON);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("Button pressed. Starting movement...");
    delay(50);

    moveDistanceMM(3900);

    delay(1000);

    turnRight(850);

    delay(1000);

    moveDistanceMM(500);
  }

  lastButtonState = currentButtonState;
}

