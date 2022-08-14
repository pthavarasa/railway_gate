/*****************************
 * Railroad Crossing Project *
 *****************************
 * Sensors : Servo, Ultrasonic, Buzzer
 * Description : if Ultrasonic detect somthing with distance
 *				 condition servo, buzzer and led trigger.
 */

#include<Servo.h>

#define pinServo 8
#define PIN_LED 9
#define PIN_BUZZER 10
#define pinUltrasonic 7
#define servoSpeed 10
#define LED_BLINK_SPEED 2
#define BUZZER_SPEED 2
#define triggerDistance 150

long readUltrasonicDistance(int pin);
void ledBlink();
void buzzer();

Servo servo1;
int cm = 0;
int pos = 0;
bool isLedHigh = false;
bool isBuzzerHigh = false;
unsigned long previousMillis;
unsigned long previousMillis2;

void setup(){
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(pinUltrasonic, INPUT);
  servo1.attach(pinServo);
  servo1.write(0);
  Serial.begin(9600);

}

void loop(){
  // measure the ping time in cm
  cm = 0.01723 * readUltrasonicDistance(7);
  // servo rotate 90°(bottom)
  while (cm < triggerDistance && pos < 90) {
    pos++;
    servo1.write(pos);
    Serial.print("Degrees rotation= ");
    Serial.println(pos);
    ledBlink();
    buzzer();
    if (pos == 90) {
      digitalWrite(PIN_LED, HIGH);
      Serial.print("Reached bottom end ");
    }
    delay(servoSpeed);
  }
  
  // servo rotate 0°(top)
  while (cm >= triggerDistance && pos > 0) {
    pos--;
    servo1.write(pos);
    Serial.print("Degrees rotation= ");
    Serial.println(pos);
    ledBlink();
    if (pos == 0) {
      digitalWrite(PIN_LED, LOW);
      Serial.print("Reached top end ");
    }
    delay(servoSpeed);
  }
  noTone(PIN_BUZZER);
  delay(100); // Wait for 100 millisecond(s)
}

long readUltrasonicDistance(int pin){
  pinMode(pin, OUTPUT);  // Clear the trigger
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  // Sets the pin on HIGH state for 10 micro seconds
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  // Reads the pin, and returns the sound wave travel time in microseconds
  return pulseIn(pin, HIGH);
  previousMillis = millis();
  previousMillis2 = millis();
}

void ledBlink(){
  if(millis() - previousMillis >= LED_BLINK_SPEED * 100){
    if(isLedHigh){
      digitalWrite(PIN_LED, LOW);
      isLedHigh = !isLedHigh;
    }else{
      digitalWrite(PIN_LED, HIGH);
      isLedHigh = !isLedHigh;
    }
    previousMillis = millis();
  }
}

void buzzer(){
  if(millis() - previousMillis2 >= BUZZER_SPEED * 100){
  if(isBuzzerHigh){
    tone(PIN_BUZZER, 500);
    isBuzzerHigh = !isBuzzerHigh;
  }else{
    noTone(PIN_BUZZER);
    isBuzzerHigh = !isBuzzerHigh;
  }
    previousMillis2 = millis();
  }
}