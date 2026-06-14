#include <Servo.h>
const int MQ_SENSOR_PIN = A0; const int FAN_PIN = 8;
const int BUZZER_PIN = 9; const int MAINS_LED_PIN = 10; const int SERVO_PIN = 11;

const int RGB_RED_PIN = 5; const int RGB_GREEN_PIN = 6; const int RGB_BLUE_PIN = 7;


const int LEVEL2_THRESHOLD = 300; const int LEVEL3_THRESHOLD = 600;


unsigned long previousMillisBuzzer = 0; unsigned long previousMillisSerial = 0; const long serialInterval = 500;
long buzzerInterval = 0;



int buzzerState = LOW; int currentLevel = 0;
bool isWarmingUp = true; unsigned long startTime;
                     Servo valveServo;

bool isValveOpen = true;

void setup() {Serial.begin(9600);
pinMode(FAN_PIN, OUTPUT); pinMode(BUZZER_PIN, OUTPUT); pinMode(MAINS_LED_PIN, OUTPUT); pinMode(RGB_RED_PIN, OUTPUT); pinMode(RGB_GREEN_PIN, OUTPUT); pinMode(RGB_BLUE_PIN, OUTPUT);


digitalWrite(FAN_PIN, LOW); digitalWrite(BUZZER_PIN, LOW); digitalWrite(MAINS_LED_PIN, HIGH);



valveServo.attach(SERVO_PIN); valveServo.write(0); isValveOpen = true;


setRGB(0, 0, 255);

startTime = millis();
}


void loop() {

unsigned long currentMillis = millis();
if (isWarmingUp) {
if (currentMillis - startTime < 10000) {
if (currentMillis - previousMillisSerial >= 1000) { previousMillisSerial = currentMillis;
Serial.println("GAS:0,LEVEL:0,MAINS:ON,FAN:OFF - WARMING UP");
}
return;
} else {
isWarmingUp = false;
}
}


int gasValue = analogRead(MQ_SENSOR_PIN);


if (gasValue < LEVEL2_THRESHOLD) { currentLevel = 1;
} else if (gasValue >= LEVEL2_THRESHOLD && gasValue < LEVEL3_THRESHOLD) {
currentLevel = 2;
} else { currentLevel = 3;
}


switch (currentLevel) { case 1:
digitalWrite(FAN_PIN, LOW); digitalWrite(MAINS_LED_PIN, HIGH);
if (!isValveOpen) { valveServo.write(0); isValveOpen = true;
}


buzzerInterval = 0; buzzerState = LOW;
digitalWrite(BUZZER_PIN, buzzerState); setRGB(0, 255, 0);

break; case 2:
digitalWrite(FAN_PIN, HIGH); digitalWrite(MAINS_LED_PIN, HIGH);

if (isValveOpen) { valveServo.write(90); isValveOpen = false;
}


buzzerInterval = 500;
setRGB(255, 165, 0);
break; case 3:
digitalWrite(FAN_PIN, HIGH); digitalWrite(MAINS_LED_PIN, LOW);
if (isValveOpen) { valveServo.write(90);
isValveOpen = false;
}

buzzerInterval = -1;
setRGB(255, 0, 0);

break;
}

if (buzzerInterval > 0) {

if (currentMillis - previousMillisBuzzer >= buzzerInterval) { previousMillisBuzzer = currentMillis;
buzzerState = !buzzerState; digitalWrite(BUZZER_PIN, buzzerState);
}


} else if (buzzerInterval == -1) { digitalWrite(BUZZER_PIN, HIGH);
} else {

digitalWrite(BUZZER_PIN, LOW);
}
if (currentMillis - previousMillisSerial >= serialInterval) { previousMillisSerial = currentMillis;
String mainsStatus = digitalRead(MAINS_LED_PIN) ? "ON" : "OFF"; String fanStatus = digitalRead(FAN_PIN) ? "ON" : "OFF";
String valveStatus = isValveOpen ? "OPEN" : "CLOSED";

Serial.print("GAS:");
Serial.print(gasValue); Serial.print(",LEVEL:"); Serial.print(currentLevel); Serial.print(",MAINS:"); Serial.print(mainsStatus); Serial.print(",FAN:"); Serial.print(fanStatus); Serial.print(",VALVE:"); Serial.println(valveStatus);
}
}

void setRGB(int r, int g, int b) { analogWrite(RGB_RED_PIN, r); analogWrite(RGB_GREEN_PIN, g); analogWrite(RGB_BLUE_PIN, b);
