const int trigPin = 9;
const int echoPin = 10;
const int armButton = 2;
const int btn1 = 3;
const int btn2 = 4;
const int btn3 = 5;
const int buzzer = 6;
const int ledPin = 13;

// --- DEFINE CODE ---
int masterCode[] = {3, 3, 1, 2, 2}; 
int codeLength = sizeof(masterCode) / sizeof(masterCode[0]);
int userAttempt[10]; 
int currentDigit = 0;
// -----------------------

float baseDistance;
bool isArmed = false;
bool isTyping = false; // Variable to check if user is entering code
unsigned long lastButtonTime = 0;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(armButton, INPUT_PULLUP);
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
}

void loop() {
  // ARMING THE SYSTEM
  if (digitalRead(armButton) == LOW && !isArmed) {
    playArmingSound();
    
    // DELAY 2 SECONDS BEFORE TAKING MEASUREMENT
    delay(2000);
    
    baseDistance = getStableDistance();
    isArmed = true;
    currentDigit = 0;
    isTyping = false;
    digitalWrite(ledPin, HIGH);
    Serial.print("SYSTEM ARMED. Base distance: ");
    Serial.println(baseDistance);
    delay(500); // Small pause to prevent double press
  }

  if (isArmed) {
    float currentDist = getStableDistance();
    // DISTANCE CHECK WITH 30cm TOLERANCE
    if (currentDist > 2 && abs(currentDist - baseDistance) > 30) {
      triggerAlarm();
    }
  }
}

void triggerAlarm() {
  while (true) {
    // ON PHASE (10 seconds) - Only if not typing code
    unsigned long timerOn = millis();
    while (millis() - timerOn < 10000) {
      if (!isTyping) {
        digitalWrite(ledPin, HIGH);
        tone(buzzer, 2500); 
        delay(80);
        digitalWrite(ledPin, LOW);
        noTone(buzzer);
        delay(50);
      }
      if (checkCodeLive()) return; // If returns true, disarms and exits
    }

    // OFF PHASE (2 seconds)
    noTone(buzzer);
    digitalWrite(ledPin, LOW);
    unsigned long timerOff = millis();
    while (millis() - timerOff < 2000) {
      if (checkCodeLive()) return;
    }
  }
}

bool checkCodeLive() {
  // If time passes (5 sec) without input, restart alarm
  if (isTyping && (millis() - lastButtonTime > 5000)) {
    isTyping = false;
    currentDigit = 0;
    playErrorBeep();
    Serial.println("Timeout! Alarm Resuming...");
  }

  int pressed = 0;
  if (digitalRead(btn1) == LOW) pressed = 1;
  else if (digitalRead(btn2) == LOW) pressed = 2;
  else if (digitalRead(btn3) == LOW) pressed = 3;

  if (pressed > 0) {
    if (!isTyping) {
      isTyping = true; // As soon as the first button is pressed, the alarm silences
      noTone(buzzer);
      digitalWrite(ledPin, HIGH); // LED stays on to indicate typing
    }
    
    playFeedbackBeep();
    userAttempt[currentDigit] = pressed;
    currentDigit++;
    lastButtonTime = millis();
    
    if (pressed == 1) waitForRelease(btn1);
    else if (pressed == 2) waitForRelease(btn2);
    else if (pressed == 3) waitForRelease(btn3);

    if (currentDigit == codeLength) {
      bool match = true;
      for (int i = 0; i < codeLength; i++) {
        if (userAttempt[i] != masterCode[i]) match = false;
      }

      if (match) {
        isArmed = false;
        isTyping = false;
        digitalWrite(ledPin, LOW);
        noTone(buzzer);
        playSuccessBeeps();
        return true; // SUCCESS
      } else {
        currentDigit = 0;
        isTyping = false; // If wrong, alarm restarts immediately
        playErrorBeep();
        return false;
      }
    }
  }
  return false;
}

// --- HELPER FUNCTIONS ---
void playArmingSound() {
  tone(buzzer, 600); delay(150);
  tone(buzzer, 800); delay(150);
  tone(buzzer, 1100); delay(200);
  noTone(buzzer);
}
void playFeedbackBeep() { tone(buzzer, 400); delay(150); noTone(buzzer); }
void playErrorBeep() { for(int i=0; i<2; i++) { tone(buzzer, 200); delay(300); noTone(buzzer); delay(100); } }
void playSuccessBeeps() { for(int i=0; i<3; i++) { tone(buzzer, 1500); delay(100); noTone(buzzer); delay(80); } }
void waitForRelease(int pin) { delay(50); while(digitalRead(pin) == LOW); delay(50); }

float getStableDistance() {
  float sum = 0;
  for(int i=0; i<3; i++) {
    digitalWrite(trigPin, LOW); delayMicroseconds(2);
    digitalWrite(trigPin, HIGH); delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH, 25000);
    sum += (duration * 0.034 / 2);
    delay(5);
  }
  return sum / 3;
}