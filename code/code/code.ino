#include <DS3231.h>
#include <Servo.h>

DS3231 rtc(SDA, SCL);
Servo myservo;

#define SERVO_PIN 9
#define BUTTON_PIN 7
#define LED_PIN 6


// Alarm durations (in minutes)
#define SCHEDULED_ALARM_DURATION 51
#define MANUAL_ALARM_DURATION    90

// Scheduled alarm times
int morningHour = 5;
int morningMinute = 30;

int eveningHour = 17;
int eveningMinute = 30;

// Alarm state tracking
bool alarm1Active = false;
bool alarm2Active = false;
bool manualAlarmActive = false;

unsigned long alarm1StartMillis = 0;
unsigned long alarm2StartMillis = 0;
unsigned long manualAlarmStartMillis = 0;

void setup() {
  Serial.begin(9600);
  rtc.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  myservo.attach(SERVO_PIN);
  myservo.write(180);  // Servo OFF initially

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED OFF initially

  // rtc.setDate(22, 6, 2025);  // Uncomment once to set time
  // rtc.setTime(17, 38, 0);

  Serial.println("⏰ Dual alarm + manual button with cancel support ready.");
}

void loop() {
  Time now = rtc.getTime();

  // --- Alarm 1: Morning ---
  if (now.hour == morningHour && now.min == morningMinute && now.sec == 0 && !alarm1Active) {
    Serial.println("⚡ Morning alarm triggered!");
    myservo.write(0);
    digitalWrite(LED_PIN, HIGH);  // Turn ON LED
    alarm1StartMillis = millis();
    alarm1Active = true;
  }

  if (alarm1Active && millis() - alarm1StartMillis >= SCHEDULED_ALARM_DURATION * 60000UL) {
    Serial.println("✅ Morning alarm ended.");
    stopAllAlarms();
  }

  // --- Alarm 2: Evening ---
  if (now.hour == eveningHour && now.min == eveningMinute && now.sec == 0 && !alarm2Active) {
    Serial.println("⚡ Evening alarm triggered!");
    myservo.write(0);
    digitalWrite(LED_PIN, HIGH);  // Turn ON LED
    alarm2StartMillis = millis();
    alarm2Active = true;
  }

  if (alarm2Active && millis() - alarm2StartMillis >= SCHEDULED_ALARM_DURATION * 60000UL) {
    Serial.println("✅ Evening alarm ended.");
    stopAllAlarms();
  }

  // --- Manual button-triggered alarm ---
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);  // Simple debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      // Cancel any running alarm
      if (alarm1Active || alarm2Active || manualAlarmActive) {
        Serial.println("🛑 Button pressed — alarm canceled!");
        stopAllAlarms();
        delay(500);  // Debounce buffer
      }
      // Trigger manual alarm only if none is running
      else {
        Serial.println("🔘 Manual alarm started (1 hour).");
        myservo.write(0);
        digitalWrite(LED_PIN, HIGH);  // Turn ON LED
        manualAlarmStartMillis = millis();
        manualAlarmActive = true;
        delay(500);
      }
    }
  }

  // Manual alarm timeout
  if (manualAlarmActive && millis() - manualAlarmStartMillis >= MANUAL_ALARM_DURATION * 60000UL) {
    Serial.println("✅ Manual alarm ended.");
    stopAllAlarms();
  }

  delay(100);
}

void stopAllAlarms() {
  myservo.write(180);  // Move to OFF position
  digitalWrite(LED_PIN, LOW);  // Turn OFF LED
  alarm1Active = false;
  alarm2Active = false;
  manualAlarmActive = false;
}
