#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <Wire.h>

#define LED 13
#define QTR_THRESHOLD   900

// these might need to be tuned for different motor types
#define REVERSE_SPEED     200 // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  200 // ms
#define TURN_DURATION     300 // ms
#define ATTACK_DISTANCE   12  // inches

ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12

const int TrigPin = 7; // Trig Pin))) sensor pin number
const int EchoPin = 8; // Echo Pin))) sensor pin number

// uncomment according to the type of QTR sensor you're using
// two QTR-1RC sensors on pins A0 (left) and A3 (right)
QTRSensorsRC sensors((unsigned char[]){A0, A3}, 2); 
//A0 = Front Left
//A3 = Front Right

void waitForButtonAndCountDown() {
  Serial.println("Waiting on Button");
  digitalWrite(LED, HIGH);
  button.waitForButton();
  digitalWrite(LED, LOW);
  
  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);  
  delay(1000);
}

void setup() {
  Serial.begin(9600);
  // uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
 
  pinMode(LED, HIGH);
  waitForButtonAndCountDown();
  
}


void loop() {
  if (button.isPressed()) {
    // if button is pressed, stop and wait for another press to go again
    motors.setSpeeds(0, 0);
    button.waitForRelease();
    //Serial.println("Button Pressed");
    waitForButtonAndCountDown();
   }
  
  // used for PING))) distance calculation
  long duration, inches, cm;
  
  unsigned int sensor_values[2];
  sensors.read(sensor_values);
  
  // Sensor Checks
  /*
  Serial.print("AO: ");
  Serial.println(sensor_values[0]);
  Serial.print("A3: ");
  Serial.println(sensor_values[1]);
  */
  
  int sensortripped;
  
  if (sensor_values[0] < QTR_THRESHOLD) { 
    sensortripped = 1; 
  } else if (sensor_values[1] < QTR_THRESHOLD) { 
    sensortripped = 2; 
  } 
  
  motors.setSpeeds(100, -100);
  
  switch (sensortripped) {
    case 1:
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      delay(TURN_DURATION);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      sensortripped = 0;
      break;
    case 2:
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
      delay(TURN_DURATION);
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      sensortripped = 0;
      break;
  }

  
  // If PING))) sensor detects object < 12 inches, full speed
  // ahead!
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(TrigPin, OUTPUT);
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  pinMode(EchoPin, INPUT);
  digitalWrite(EchoPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(EchoPin, INPUT);
  duration = pulseIn(EchoPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  
  if ( inches <= ATTACK_DISTANCE ) {
    motors.setSpeeds(400,400);
    buzzer.playNote(NOTE_G(4), 500, 15);
    Serial.println("Attack!");
  }
  //cm = microsecondsToCentimeters(duration);
  
  /*
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  */
  delay(100);
}

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
