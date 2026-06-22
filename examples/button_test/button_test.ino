const int START_BUTTON = 27;

void setup() {
  Serial.begin(115200);
  pinMode(START_BUTTON, INPUT_PULLUP);
  Serial.println("Button test ready. Press the button.");
}

void loop() {
  if (digitalRead(START_BUTTON) == LOW) {
    Serial.println("Button pressed");
    delay(50);
  }
}

