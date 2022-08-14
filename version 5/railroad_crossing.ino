/*****************************
 * Railroad Crossing Project *
 *****************************
 * Sensors : Servo, Ultrasonic, Buzzer
 * Description : I Ultrasonic detect somthing with distance
 *         condition servo, buzzer and led trigger.
 *               We can take control manually with button.
 */

#include<Servo.h>

#define DEBUG true

#define pinServo 9
#define PIN_LED 2
#define PIN_LED_GREEN 3
#define PIN_LED_ORANGE 4
#define PIN_MANUAL_BUTTON 11
#define PIN_AUTOMATIC_BUTTON 9
#define PIN_UP_BUTTON 7
#define PIN_DOWN_BUTTON 8
#define PIN_BUZZER 10
#define pinUltrasonicEcho 6
#define pinUltrasonicTrig 5
#define SERVO_SPEED 90
#define LED_BLINK_SPEED 200
#define BUZZER_SPEED 100
#define READ_ULTRASONIC_SPEED 300
#define triggerDistance 50

/** 2 bits communication
 * 00 : train comming
 * 01 : gate closing
 * 10 : gate opening
 * 11 : drive carefully
 */
#define PIN_COM_1 13
#define PIN_COM_2 12

static Servo servo1;
volatile int cm = 401;
int distanceArray[5];
int distanceArrayCount = 0;
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
volatile unsigned long previousMillis6;

long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

void setup(){
  pinMode(PIN_COM_1, OUTPUT);
  pinMode(PIN_COM_2, OUTPUT);
  twoBitsCom(1,1);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_ORANGE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_MANUAL_BUTTON, INPUT);
  digitalWrite(PIN_MANUAL_BUTTON, HIGH);
  pinMode(PIN_AUTOMATIC_BUTTON, INPUT);
  pinMode(PIN_UP_BUTTON, INPUT);
  digitalWrite(PIN_UP_BUTTON, HIGH);
  pinMode(PIN_DOWN_BUTTON, INPUT);
  digitalWrite(PIN_DOWN_BUTTON, HIGH);
  pinMode(pinUltrasonicTrig, OUTPUT);
  pinMode(pinUltrasonicEcho, INPUT);
  servo1.attach(pinServo);
  delay(4000);
  servo1.write(0);
  delay(4000);
  Serial.begin(9600);
}


void loop(){
  // read push button state
  //if(digitalRead(PIN_AUTOMATIC_BUTTON))isManual=false;
  if(!digitalRead(PIN_MANUAL_BUTTON))isManual=!isManual;
  if(!digitalRead(PIN_UP_BUTTON))direction1='u';
  if(!digitalRead(PIN_DOWN_BUTTON))direction1='d';
  if(DEBUG)Serial.print("Mode : ");
  if(DEBUG)Serial.println(isManual?"Manual":"Automatic");
  if(DEBUG)Serial.print("Barrier : ");
  if(DEBUG)Serial.println(direction1=='u'?"UP":"Down");
  
  readUltrasonic();
  
  //  6 seconds alert before servo start
  if(!isManual && !isSensorTrigger && cm < triggerDistance
    || isManual && !isSensorTrigger && direction1 == 'd'){
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
  while (!isManual && cm < triggerDistance && pos < 90
        || isManual && direction1 == 'd' && pos < 90) {
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
      direction1='d';
      if(DEBUG)Serial.print("Reached bottom end ");
    }
  }
  
  // servo rotate 0°(top)
  while (!isManual && cm >= triggerDistance && pos > 0
        || isManual && direction1 == 'u' && pos > 0) {
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
      direction1='u';
      if(DEBUG)Serial.print("Reached top end ");
    }
  }
  
  // handle unwanted buzzer sound
  if(isGateClosed)buzzer();
  if(!isGateClosed){
    noTone(PIN_BUZZER);
    twoBitsCom(1,1);
  }
}
/*
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
*/
void readUltrasonicDistance2(){
  // Clears the trigPin condition
  digitalWrite(pinUltrasonicTrig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(pinUltrasonicTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinUltrasonicTrig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(pinUltrasonicEcho, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
}

void readUltrasonic(){
  if(millis() - previousMillis6 >= READ_ULTRASONIC_SPEED){
    // measure the ping time in cm
    //cm = distance;
    readUltrasonicDistance2();
    distanceArray[distanceArrayCount] = distance;
    distanceArrayCount++;
    if(distanceArrayCount == 5){
      int temp2 = getClosestValue(distanceArray);
      if(temp2 != -1)cm = temp2;
      if(DEBUG)Serial.print("Distance : ");
      if(DEBUG)Serial.println(cm);
      distanceArrayCount = 0;
    }
    previousMillis6 = millis();
  }
}

void servoWrite(int directionpos){
  if(millis() - previousMillis3 >= SERVO_SPEED){
    pos += directionpos;
    servo1.write(pos);
    if(DEBUG)Serial.print("Servo Angle : ");
    if(DEBUG)Serial.println(pos);
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

int getClosestValue(int distanceArray[5]){
  int total = 0, sum = 0;
  float center;
  for(int i = 0; i < 5; i++){
    if(distanceArray[i] > 0 && distanceArray[i] < 400){
      sum += distanceArray[i];
      total++;
    }
  }
  center = (double)sum/(double)total;
  int closestValue = -1;
  float distance = 401;
  for(int i = 0; i < 5; i++){
    if(distanceArray[i] > 0 && distanceArray[i] < 400){
      if(distance > fabs(distanceArray[i] - center)){
          distance = fabs(distanceArray[i] - center);
          closestValue = distanceArray[i];
      }
    }
  }
  return closestValue;
}