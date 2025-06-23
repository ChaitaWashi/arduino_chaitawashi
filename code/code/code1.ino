#include <Wire.h>
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


void logWithTimestamp(const char* message) {
  Time t = rtc.getTime();
  if (t.hour > 23 || t.min > 59 || t.sec > 59) {
    Serial.print("[??:??:??] ");
  } else {
    Serial.print("[");
    if (t.hour < 10) Serial.print("0");
    Serial.print(t.hour); Serial.print(":");
    if (t.min < 10) Serial.print("0");
    Serial.print(t.min); Serial.print(":");
    if (t.sec < 10) Serial.print("0");
    Serial.print(t.sec);
    Serial.print("] ");
  }
  Serial.println(message);
}



void setup() {
  Wire.begin();
  delay(100);
  Serial.begin(9600);
  rtc.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  myservo.attach(SERVO_PIN);
  myservo.write(180);  // Servo OFF initially

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // LED OFF initially

  // rtc.setDate(24, 6, 2025);  // Uncomment once to set time
  // rtc.setTime(1, 55, 0);

  delay(1000);  // Give RTC a moment after power/reset
  rtc.getTime();  // Dummy read to wake things up
  delay(100);     // Slight delay for safety
  logWithTimestamp("⏰ Dual alarm + manual button with cancel support ready.");




}

void loop() {
  Time now = rtc.getTime();

  // --- Alarm 1: Morning ---
  if (now.hour == morningHour && now.min == morningMinute && now.sec == 0 && !alarm1Active) {
    delay(1000);  // Give RTC a moment after power/reset
    rtc.getTime();  // Dummy read to wake things up
    delay(100);     // Slight delay for safety
    logWithTimestamp("⚡ Morning alarm triggered!");
    myservo.write(0);
    digitalWrite(LED_PIN, HIGH);  // Turn ON LED
    alarm1StartMillis = millis();
    alarm1Active = true;
  }

  if (alarm1Active && millis() - alarm1StartMillis >= SCHEDULED_ALARM_DURATION * 60000UL) {
    delay(1000);  // Give RTC a moment after power/reset
    rtc.getTime();  // Dummy read to wake things up
    delay(100);     // Slight delay for safety
    logWithTimestamp("✅ Morning alarm ended.");
    stopAllAlarms();
  }

  // --- Alarm 2: Evening ---
  if (now.hour == eveningHour && now.min == eveningMinute && now.sec == 0 && !alarm2Active) {
    delay(1000);  // Give RTC a moment after power/reset
    rtc.getTime();  // Dummy read to wake things up
    delay(100);     // Slight delay for safety
    logWithTimestamp("⚡ Evening alarm triggered!");
    myservo.write(0);
    digitalWrite(LED_PIN, HIGH);  // Turn ON LED
    alarm2StartMillis = millis();
    alarm2Active = true;
  }

  if (alarm2Active && millis() - alarm2StartMillis >= SCHEDULED_ALARM_DURATION * 60000UL) {
    delay(1000);  // Give RTC a moment after power/reset
    rtc.getTime();  // Dummy read to wake things up
    delay(100);     // Slight delay for safety
    logWithTimestamp("✅ Evening alarm ended.");
    stopAllAlarms();
  }

  // --- Manual button-triggered alarm ---
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50);  // Simple debounce
    if (digitalRead(BUTTON_PIN) == LOW) {
      // Cancel any running alarm
      if (alarm1Active || alarm2Active || manualAlarmActive) {
        delay(1000);  // Give RTC a moment after power/reset
        rtc.getTime();  // Dummy read to wake things up
        delay(100);     // Slight delay for safety
        logWithTimestamp("🛑 Button pressed — alarm canceled!");
        stopAllAlarms();
        delay(500);  // Debounce buffer
      }
      // Trigger manual alarm only if none is running
      else {
        delay(1000);  // Give RTC a moment after power/reset
        rtc.getTime();  // Dummy read to wake things up
        delay(100);     // Slight delay for safety
        logWithTimestamp("🔘 Manual alarm started (1 hour).");
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
    delay(1000);  // Give RTC a moment after power/reset
    rtc.getTime();  // Dummy read to wake things up
    delay(100);     // Slight delay for safety
    logWithTimestamp("✅ Manual alarm ended.");
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
