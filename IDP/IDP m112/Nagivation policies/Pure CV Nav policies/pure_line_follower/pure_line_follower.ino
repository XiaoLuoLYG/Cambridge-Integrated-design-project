// Test navigation policy that simply follows a line indefinitely. 
// Does not care about PC instructins, so no need to initilise comms
// Requires sensors script, which handles reading the line detectors

#include IDP_sensors // attempting to import sensors code


// PARAMETERS to optimise for line follower
int regularSpeed = 110;        // MAX 255
int turnSpeed = 230;    // MAX 255 
int turnDelay = 10;

// LINE FOLLOWER SENSOR STATES (BINARY)
int left_sensor_state;
int middle_sensor_state;
int right_sensor_state;


void lineFollower (int left_sensor_state, int middle_sensor_state, int right_sensor_state) {
  if((left_sensor_state == 0 && right_sensor_state == 0) && middle_sensor_state == 1){
    Serial.println("Go forward");

    activateMotorM1(regularSpeed, 0);
    activateMotorM2(regularSpeed, 0);
  
    delay(turnDelay);
   }
    
  if((left_sensor_state == 1 && right_sensor_state == 0) && middle_sensor_state == 1){
    Serial.println("Turn left");

    activateMotorM1(regularSpeed, 0);
    activateMotorM2(turnSpeed, 0);
  
    delay(turnDelay);
   }

  if((left_sensor_state == 0 && right_sensor_state == 1) && middle_sensor_state == 1){
    Serial.println("Turn right");

    activateMotorM1(turnSpeed, 0);
    activateMotorM2(regularSpeed, 0);
  
    delay(turnDelay);
   }
}

void loop() {

    left_sensor_state = readlinesensor("left")
    middle_sensor_state = readlinesensor("middle")
    right_sensor_state = readlinesensor("right")

    lineFollower(left_sensor_state, middle_sensor_state, right_sensor_state);
}