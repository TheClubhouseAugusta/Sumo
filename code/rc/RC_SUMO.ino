#include <AFMotor.h>

AF_DCMotor MotorLeft(1);
AF_DCMotor MotorRight(2);

#define THROTTLE_PIN   2 // throttle channel from RC receiver
#define THROTTLE_OFFSET 1500 // throttle offset
#define STEERING_PIN   13 // steering channel from RC receiver
#define STEERING_OFFSET 1500
#define LED_PIN       13 // user LED pin

#define MAX_SPEED             255 // max motor speed
#define PULSE_WIDTH_DEADBAND   25 // pulse width difference from 1500 us (microseconds) to ignore (to compensate for control centering offset)
#define PULSE_WIDTH_RANGE     350 // pulse width difference from 1500 us to be treated as full scale input (for example, a value of 350 means
//   any pulse width <= 1150 us or >= 1850 us is considered full scale)

void setup()
{
  Serial.begin(14400);
  Serial.println("RC Code Loading...");
  //pinMode(LED_PIN, OUTPUT);
  // uncomment one or both of the following lines if your motors' directions need to be flipped
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
  MotorLeft.setSpeed(255);
  MotorRight.setSpeed(255);

}

void loop()
{
  int throttle = pulseIn(THROTTLE_PIN, HIGH);
  int steering = pulseIn(STEERING_PIN, HIGH);
  int right_direction = FORWARD;
  int left_direction  = FORWARD;

  int left_speed, right_speed;

  if (throttle > 0 && steering > 0) {
    // both RC signals are good; turn on LED
    digitalWrite(LED_PIN, HIGH);

    // RC signals encode information in pulse width centered on 1500 us (microseconds); subtract 1500 to get a value centered on 0
    throttle -= THROTTLE_OFFSET;
    steering -= STEERING_OFFSET;

    // apply deadband
    if (abs(throttle) <= PULSE_WIDTH_DEADBAND) {
      throttle = 0;
    }
    if (abs(steering) <= PULSE_WIDTH_DEADBAND) {
      steering = 0;
    }

    // mix throttle and steering inputs to obtain left & right motor speeds
    left_speed = ((long)throttle * MAX_SPEED / PULSE_WIDTH_RANGE) - ((long)steering * MAX_SPEED / PULSE_WIDTH_RANGE);
    right_speed = ((long)throttle * MAX_SPEED / PULSE_WIDTH_RANGE) + ((long)steering * MAX_SPEED / PULSE_WIDTH_RANGE);

    // cap speeds to max
    left_speed = min(max(left_speed, -MAX_SPEED), MAX_SPEED);
    right_speed = min(max(right_speed, -MAX_SPEED), MAX_SPEED);

    if ( left_speed <= 0 ) {
      left_direction = BACKWARD;
      left_speed = left_speed * -1;
    } 
    else {
      left_direction = FORWARD;
    }
    if ( right_speed <= 0 ) {
      right_direction = BACKWARD;
      right_speed = right_speed * -1;
    } 
    else {
      right_direction = FORWARD;
    } 
  } 
  else {
    // at least one RC signal is not good; turn off LED and stop motors
    //digitalWrite(LED_PIN, LOW);

    left_speed = 0;
    right_speed = 0;
  }

  Serial.print("Throttle: ");
  Serial.print(throttle);

  Serial.print(", Steering: ");
  Serial.print(steering);

  Serial.print(", ");
  Serial.print(left_speed);
  Serial.print("/");
  Serial.println(right_speed); 

  MotorLeft.setSpeed(left_speed);
  MotorRight.setSpeed(right_speed);
  MotorLeft.run(left_direction);
  MotorRight.run(right_direction);
}


void throttleTest() {
  MotorLeft.run(FORWARD);
  delay(1000);
  MotorRight.run(FORWARD);
  delay(1000);
}

void motorTest() {

  /* TEST motors */
  Serial.println("Starting left motor");
  MotorLeft.run(FORWARD);
  delay(1000);
  MotorLeft.run(RELEASE);
  delay(100);
  MotorLeft.run(BACKWARD);
  delay(1000);
  MotorLeft.run(RELEASE);
  delay(100);

  Serial.println("Starting right motor");
  MotorRight.run(FORWARD);
  delay(1000);
  MotorRight.run(RELEASE);
  delay(100);
  MotorRight.run(BACKWARD);
  delay(1000);
  MotorRight.run(RELEASE);
  delay(100);
}


