#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_PIN 25
#define BUZZER_PIN 26
#define POT_PIN 34   // Slider input

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (true);
  }

  Serial.println("ESP32 Dementia Safety System Started");
  Serial.println("Use slider to simulate motion / fall");
}

void loop() {
  // Read slider (0–4095)
  int potValue = analogRead(POT_PIN);

  // Map slider to acceleration magnitude (0.5g – 4.0g)
  float accMagnitude = map(potValue, 0, 4095, 5, 40) / 10.0;

  // Map slider to heart rate (60–130 BPM)
  int heartRate = map(potValue, 0, 4095, 60, 130);

  // Fall detection logic
  bool fallDetected = accMagnitude > 2.5;

  // OLED display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print("Heart Rate: ");
  display.println(heartRate);

  display.print("Acc Mag: ");
  display.println(accMagnitude, 2);

  display.print("Fall: ");
  display.println(fallDetected ? "YES" : "NO");

  display.display();

  // Serial Monitor (OBSERVATION)
  Serial.print("Slider=");
  Serial.print(potValue);
  Serial.print(" | HR=");
  Serial.print(heartRate);
  Serial.print(" | Acc=");
  Serial.print(accMagnitude);
  Serial.print(" | Fall=");
  Serial.println(fallDetected ? "YES" : "NO");

  // Emergency condition
  if (heartRate > 110 || fallDetected) {
    triggerAlert();
    delay(3000);
  }

  delay(1000);
}

void triggerAlert() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000);

  Serial.println("🚨 EMERGENCY ALERT!");
  Serial.println("HTTP POST -> /api/alert");
  Serial.println("MQTT Publish -> patient/alert");

  delay(2000);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);
}
