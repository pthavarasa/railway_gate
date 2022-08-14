/*****************************
 * Railroad Crossing Project *
 *****************************
 * Sensors : Servo, Ultrasonic, Buzzer
 * Description : I Ultrasonic detect somthing with distance
 *         condition servo, buzzer and led trigger.
 *               We can take control manually with button.
 */

#include<Servo.h>

#define DEBUG false

#define pinServo 9
#define PIN_LED 2
#define PIN_LED_GREEN 3
#define PIN_LED_ORANGE 4
#define PIN_MANUAL_BUTTON 9
#define PIN_AUTOMATIC_BUTTON 11
#define PIN_UP_BUTTON 12
#define PIN_DOWN_BUTTON 13
#define PIN_BUZZER 10
#define pinUltrasonic 6
#define pinUltrasonicTrig 5
#define SERVO_SPEED 30
#define LED_BLINK_SPEED 200
#define BUZZER_SPEED 100
#define triggerDistance 50

/** 2 bits communication
 * 00 : train comming
 * 01 : gate closing
 * 10 : gate opening
 * 11 : drive carefully
 */
#define PIN_COM_1 13
#define PIN_COM_2 12

extern void buzzer();
extern void ledBlink();
extern void alertLedBlink();
extern void printServoAngle();
extern long readUltrasonicDistance(int pin);
extern long readUltrasonicDistance2();

static Servo servo1;
volatile int cm = 0;
volatile int pos = 0;
volatile char direction1 = 'u';
volatile bool isManual = false;
volatile bool isLedHigh = false;
volatile bool isAlertLedHigh = false;
volatile bool isBuzzerHigh = false;
volatile bool isGateClosed = false;
volatile bool isSensorTrigger = false;
volatile unsigned long previousMillis;
volatile unsigned long previousMillis2;
volatile unsigned long previousMillis3;
volatile unsigned long previousMillis4;
volatile unsigned long previousMillis5;

void setup(){
  pinMode(PIN_COM_1, OUTPUT);
  pinMode(PIN_COM_2, OUTPUT);
  twoBitsCom(1,1);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_ORANGE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MANUAL_BUTTON, INPUT);
  pinMode(PIN_AUTOMATIC_BUTTON, INPUT);
  pinMode(PIN_UP_BUTTON, INPUT);
  pinMode(PIN_DOWN_BUTTON, INPUT);
  pinMode(pinUltrasonic, INPUT);
  pinMode(pinUltrasonicTrig, OUTPUT);
  servo1.attach(pinServo);
  servo1.write(0);
  Serial.begin(9600);
}


void loop(){
  // read push button state
  //if(digitalRead(PIN_AUTOMATIC_BUTTON))isManual=false;
  //if(digitalRead(PIN_MANUAL_BUTTON))isManual=true;
  //if(digitalRead(PIN_UP_BUTTON))direction1='u';
  //if(digitalRead(PIN_DOWN_BUTTON))direction1='d';
  if(DEBUG)Serial.print(isManual);
  if(DEBUG)Serial.print(direction1);
  
  // measure the ping time in cm
  //cm = 0.01723 * readUltrasonicDistance(7);
  int maxi = 0;
  for(int i = 0; i < 5; i++){
    cm = readUltrasonicDistance2();
    if(cm > maxi)maxi = cm;
    delay(200);
  }
  cm = maxi;
  Serial.println(cm);
  
  //  6 seconds alert before servo start
  if(!isSensorTrigger && cm < triggerDistance){
    //Serial.println(cm);
    twoBitsCom(0,0);
    digitalWrite(PIN_LED, 1);
    previousMillis4 = millis();
    do{
      buzzer();
    }while(millis() - previousMillis4 <= 6000);
    isSensorTrigger = true;
  }
  
  // servo rotate 90°(bottom)
  while (cm < triggerDistance && pos < 90) {
    servoWrite(1);
    ledBlink();
    alertLedBlink();
    buzzer();
    twoBitsCom(0,1);
    if (pos == 90) {
      digitalWrite(PIN_LED, HIGH);
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_ORANGE, LOW);
      isGateClosed = true;
      if(DEBUG)Serial.print("Reached bottom end ");
    }
  }
  
  // servo rotate 0°(top)
  while (cm >= triggerDistance && pos > 0) {
    servoWrite(-1);
    ledBlink();
    alertLedBlink();
    twoBitsCom(1,0);
    buzzer();
    if (pos == 0) {
      digitalWrite(PIN_LED, LOW);
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_ORANGE, LOW);
      isSensorTrigger = false;
      isGateClosed = false;
      if(DEBUG)Serial.print("Reached top end ");
    }
  }
  
  // handle unwanted buzzer sound
  if(isGateClosed)buzzer();
  if(!isGateClosed){
    noTone(PIN_BUZZER);
    twoBitsCom(1,1);
  }
  delay(500);
}

long readUltrasonicDistance(int pin){
  pinMode(pin, OUTPUT);  // Clear the trigger
  digitalWrite(pin, LOW);
  delayMicroseconds(5);
  // Sets the pin on HIGH state for 10 micro seconds
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  // Reads the pin, and returns the sound wave travel time in microseconds
  return pulseIn(pin, HIGH);
}

long readUltrasonicDistance2(){
  // Clears the trigPin
  digitalWrite(pinUltrasonicTrig, LOW);
  delayMicroseconds(10);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(pinUltrasonicTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinUltrasonicTrig, LOW);
  delayMicroseconds(15);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(pinUltrasonic, HIGH);
  delayMicroseconds(15);
  // Calculating the distance
  long distance = duration * 0.034 / 2;
  return distance;
}

void servoWrite(int direction1){
  if(millis() - previousMillis3 >= SERVO_SPEED){
    pos += direction1;
    servo1.write(pos);
    if(DEBUG)printServoAngle();
    previousMillis3 = millis();
  }
}

void ledBlink(){
  if(millis() - previousMillis >= LED_BLINK_SPEED){
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

void alertLedBlink(){
  if(millis() - previousMillis5 >= LED_BLINK_SPEED){
    if(isAlertLedHigh){
      digitalWrite(PIN_LED_ORANGE, LOW);
      isAlertLedHigh = !isAlertLedHigh;
    }else{
      digitalWrite(PIN_LED_ORANGE, HIGH);
      isAlertLedHigh = !isAlertLedHigh;
    }
    previousMillis5 = millis();
  }
}

void buzzer(){
  if(millis() - previousMillis2 >= BUZZER_SPEED){
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

void twoBitsCom(int bit1, int bit2){
  digitalWrite(PIN_COM_1, bit1);
  digitalWrite(PIN_COM_2, bit2);
}

void printServoAngle(){
  Serial.print("Degrees rotation= ");
    Serial.println(pos);
}