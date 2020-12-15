#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorM1 = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorM2 = AFMS.getMotor(1);

const int trigPin = 13;
const int echoPin = 12;
const int led_amber = 3; // amber led
const int led_green = 2;
const int led_red = 4;
const int start_button = 5;

// line follower binary cutoff
int line_follower_cutoff = 700;
 
// constants for distance sensing using ultrasound
const long distance_range_5 = 6;
const long distance_range_10 = 11;

// motor speed and delay params
int regular_speed = 120;
int turn_speed = 250;
int delay_time = 10;

// LED stuff
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 500;

// colour detection stuff
int sensorPinRed = A4;
int sensorPinBlue = A5;
int sensorValueRed = 0;
int sensorValueBlue = 0;
int calibrate_colour = 0;
int difference_colour = 0;

// fruits collected
int fruitsCollected = 0;

bool blinkeyLed = false;
// used to control the switch case function
int arduino_state = 0;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Start");
  AFMS.begin();
  pinMode(start_button,INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led_amber, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
}
void loop() {
  // blinkey led
  if (blinkeyLed == true) {
    unsigned long currentMillis = millis();
  
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
  
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(led_amber, ledState);
    }
  }
  //
  int button_state = digitalRead(start_button);
    // Initialising robot
  if (button_state==1) {
    Serial.println("Initialising HARIBOS...");
    sensorValueRed = analogRead(sensorPinRed);
    sensorValueBlue = analogRead(sensorPinBlue);
    calibrate_colour = sensorValueRed - sensorValueBlue;
    blinkeyLed = !blinkeyLed;

    if (arduino_state == 0) {
      activateMotorM1(regular_speed,1);
      activateMotorM2(regular_speed,1);
    
      arduino_state = 1;
    } else {
      arduino_state = 0;
      activateMotorM1(0,1);
      activateMotorM2(0,1);
    }
    delay(2000);
  }

  switch (arduino_state) {
    case 0: // pre-activation
      {
      Serial.println("HARIBOS not yet activated!");
      
      }
      break;

    case 1: // read ultrasound sensor. if nothing proceed to case 2. else proceed to case x.
      {
      Serial.println("Reading Ultrasound Sensor!");
      bool gotFruit = readUltrasoundSensor(distance_range_5, trigPin, echoPin);
      Serial.println(gotFruit);
      if (gotFruit) {  // This is where the LED On/Off happens
        activateMotorM1(0,1);
        activateMotorM2(0,1);
        turnOnLed(led_amber, 1500);
        arduino_state = 3; // change to fruit analysis case
      } else {
        arduino_state = 2; // continue line following algorithm
      }
      }
      break;

    case 2: // line following algorithm outbound
      {
      Serial.println("Line Following 0!");
        
      String current_state = readLineSensorArray(A0, A1, A2, A3, line_follower_cutoff);

      activateLineFollower0(current_state);
      delay(delay_time);        // delay in between reads for stability
      
      arduino_state = 1; // move back to case 1 to detect if there is a fruit ahead
      }
      break;   

    case 3: // fruit analysis
      {
        Serial.println("Analysing fruit");
        // creep up to 5 cm distance from fruit
//        bool gotFruitNear = readUltrasoundSensor(distance_range_5, trigPin, echoPin);
//        
//        while (!gotFruitNear) {
//          gotFruitNear = readUltrasoundSensor(distance_range_5, trigPin, echoPin);
//          
//          String current_state = readLineSensorArray(A0, A1, A2, A3, line_follower_cutoff);
//  
//          activateLineFollower0(current_state);
//          delay(delay_time);
//          
//        }
//
//        activateMotorM1(0,1);
//        activateMotorM2(0,1);
        
        // test fruit colour
        bool colourResult = false;

        while (!colourResult) {
          sensorValueRed = analogRead(sensorPinRed);
          sensorValueBlue = analogRead(sensorPinBlue);
          difference_colour = sensorValueRed - sensorValueBlue;

          // ripe fruit
          if (difference_colour > calibrate_colour){
//            digitalWrite(led_coloursensor, LOW);
            turnOnLed(led_green, 5500);
            colourResult = true;
            arduino_state = 5;

          // unripe fruit
          } else if (difference_colour < calibrate_colour){
//            digitalWrite(led_coloursensor, LOW);
            turnOnLed(led_red, 5500);
            colourResult = true;
            arduino_state = 5;
            
          } else {
            colourResult = false; // else detect again!
          }
        }
      }
      break;

    case 4: // handling ripe fruit
      {
        Serial.println("Ripe fruit detected!");
        // move forward for 300ms
        for (int i=0; i<=29; i++) {       
          String current_state = readLineSensorArray(A0, A1, A2, A3, line_follower_cutoff);
          activateLineFollower0(current_state);
          delay(delay_time);
        }

        // turn 180 degrees
        activateMotorM1(regular_speed,0);
        activateMotorM2(regular_speed,1);
        delay(4340);

        // go back to line following to base
        arduino_state = 6;
      }
      break;

    case 5: // handling unripe fruit
      {
        fruitsCollected++;
        Serial.println("Unripe fruit detected!");
        if (fruitsCollected == 6) {
          arduino_state = 6;
        } else {
          // move forward for 300ms
          activateMotorM1(regular_speed,1);
          activateMotorM2(regular_speed,1);
          delay(400);
  
          // turn right 90 degrees
          activateMotorM1(regular_speed,1);
          activateMotorM2(regular_speed,0);
          delay(2750);
  
          // move forward for 2000ms
          activateMotorM1(regular_speed,1);
          activateMotorM2(regular_speed,1);
          delay(2200);
  
          // reverse for 2000ms
          activateMotorM1(regular_speed,0);
          activateMotorM2(regular_speed,0);
          delay(2200);
  
  
          // turn left 90 degrees
          activateMotorM1(regular_speed,0);
          activateMotorM2(regular_speed,1);
          delay(2750);
  
          // reverse for 300ms
          activateMotorM1(regular_speed,0);
          activateMotorM2(regular_speed,0);
          delay(400);
  
          // back to line following
          sensorValueRed = analogRead(sensorPinRed);
          sensorValueBlue = analogRead(sensorPinBlue);
          calibrate_colour = sensorValueRed - sensorValueBlue;
          arduino_state = 1;
        }
        
      }
      break;

    case 6: // line following algorithm back to fruit drop zone
      {
      Serial.println("Docking...");
      String current_state = readLineSensorArray(A0, A1, A2, A3, line_follower_cutoff);

      // red box cutoff correct? TO TEST ///////////////
      if (current_state == "1111") {
        activateMotorM1(regular_speed,1);
        activateMotorM2(regular_speed,1);
        delay(3000);
        activateMotorM1(0,1);
        activateMotorM2(0,1);
        arduino_state = 0;
      } else {
      activateLineFollower0(current_state);
      delay(delay_time);        // delay in between reads for stability
      }
      }
      break;   
  }
}

// ALL USED FUNCTIONS ARE BELOW FOR REFERENCE

// line follower logic: OUTBOUND TRIP
void activateLineFollower0(String current_state) {
  // Possible states
  //  0000 - GO STRAIGHT
  //  0001 - TURN RIGHT
  //  0010 - TURN RIGHT
  //  0011 - T-JUNCTION WITH OPTION TO TURN RIGHT: TURN RIGHT 90 DEGREES
  //  0100 - TURN LEFT
  //  0101 - UNASSIGNED
  //  0110 - Y JUNCTION: TURN LEFT
  //  0111 - UNASSIGNED
  //  1000 - TURN LEFT
  //  1001 - Y JUNCTION: TURN LEFT
  //  1010 - UNASSIGNED
  //  1011 - UNASSIGNED
  //  1100 - T-JUNCTION WITH OPTION TO TURN LEFT: TURN LEFT 90 DEGREES
  //  1101 - UNASSIGNED
  //  1110 - UNASSIGNED
  //  1111 - T-JUNCTION WITH OPTION TO TURN LEFT OR RIGHT: TURN LEFT 90 DEGREES
    
  if(current_state == "0000"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
  }
  else if(current_state == "0001"){
    activateMotorM1(turn_speed,1);
    activateMotorM2(0,1);
  }
  else if(current_state == "0010"){
    activateMotorM1(turn_speed,1);
    activateMotorM2(0,1);
  }
  else if(current_state == "0011"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
    delay(200);
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,0);
    delay(2170);
  }
  else if(current_state == "0100"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "0110"){
    activateMotorM1(turn_speed,1);
    activateMotorM2(0,1);
  }
  else if(current_state == "1000"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "1001"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "1010"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "1100"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
    delay(200);
    activateMotorM1(regular_speed,0);
    activateMotorM2(regular_speed,1);
    delay(2170);
  }
  else if(current_state == "1111"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
    delay(200);
    activateMotorM1(regular_speed,0);
    activateMotorM2(regular_speed,1);
    delay(2170);
  }
  else {
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
  }
}

// line follower logic: HOMEBOUND TRIP
void activateLineFollower1(String current_state) {
  // Possible states
  //  0000 - GO STRAIGHT
  //  0001 - TURN RIGHT
  //  0010 - TURN RIGHT
  //  0011 - T-JUNCTION WITH OPTION TO TURN RIGHT: TURN RIGHT 90 DEGREES
  //  0100 - TURN LEFT
  //  0101 - UNASSIGNED
  //  0110 - Y JUNCTION: TURN RIGHT
  //  0111 - UNASSIGNED
  //  1000 - TURN LEFT
  //  1001 - UNASSIGNED
  //  1010 - UNASSIGNED
  //  1011 - UNASSIGNED
  //  1100 - T-JUNCTION WITH OPTION TO TURN LEFT: TURN LEFT 90 DEGREES
  //  1101 - UNASSIGNED
  //  1110 - UNASSIGNED
  //  1111 - T-JUNCTION WITH OPTION TO TURN LEFT OR RIGHT: TURN LEFT 90 DEGREES
    
  if(current_state == "0000"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
  }
  else if(current_state == "0001"){
    activateMotorM1(turn_speed,1);
    activateMotorM2(0,1);
  }
  else if(current_state == "0010"){
    activateMotorM1(turn_speed,1);
    activateMotorM2(0,1);
  }
  else if(current_state == "0011"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
    delay(200);
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,0);
    delay(2170);
  }
  else if(current_state == "0100"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "0110"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "1000"){
    activateMotorM1(0,1);
    activateMotorM2(turn_speed,1);
  }
  else if(current_state == "1100"){
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
    delay(200);
    activateMotorM1(regular_speed,0);
    activateMotorM2(regular_speed,1);
    delay(2170);
  }
  else {
    activateMotorM1(regular_speed,1);
    activateMotorM2(regular_speed,1);
  }
}

// ultrasound sensor
bool readUltrasoundSensor(long distance_range, int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration/2) / 29.1;

  if (distance < distance_range) {
    return true;
  } else {
    return false;
  }
}

//sensor stuff
int analogToBinary (int analogInput, int binaryCutOff) {
  return analogInput < binaryCutOff ? 1 : 0;
}

int readLineSensor(int sensor_pin, int line_follower_cutoff) {
  return analogToBinary(analogRead(sensor_pin),line_follower_cutoff); 
}

String readLineSensorArray(int A0, int A1, int A2, int A3, int line_follower_cutoff) {
  int sensor_value0 = readLineSensor(A0, 200);
  int sensor_value1 = readLineSensor(A1, line_follower_cutoff);
  int sensor_value2 = readLineSensor(A2, line_follower_cutoff);
  int sensor_value3 = readLineSensor(A3, line_follower_cutoff);
  String current_state = String(sensor_value0) + String(sensor_value1) + String(sensor_value2) + String(sensor_value3); 

  String to_print = String(analogRead(A0)) + " " + String(analogRead(A1)) + " " + String(analogRead(A2)) + " " + String(analogRead(A3));
  Serial.println(to_print);
  Serial.println(current_state);

  return current_state;
}
  
//motor stuff
void activateMotorM2 (int speedInput, int forward) {
  // speedInput from 0 - 255
  // forward = 1 is moving forward, forward = 0 is the reverse direction
  forward = !forward;
  myMotorM2->setSpeed(speedInput); // set speed
  myMotorM2->run(RELEASE); // turn on motor
  Serial.println("Running right motor...");
  Serial.println(speedInput);
  // select direction
  if (forward == 1) {
    myMotorM2->run(FORWARD);
  } else {
    myMotorM2->run(BACKWARD);
  };
}
void activateMotorM1 (int speedInput, int forward) {
  // speedInput from 0 - 255
  // forward = 1 is moving forward, forward = 0 is the reverse direction
  forward = !forward;
  myMotorM1->setSpeed(speedInput); // set speed
  myMotorM1->run(RELEASE); // turn on motor
  Serial.println("Running left motor...");
  Serial.println(speedInput);
  // select direction
  if (forward == 1) {
    myMotorM1->run(FORWARD);
  } else {
    myMotorM1->run(BACKWARD);
  };
}

// LED turn on wrapper
void turnOnLed(int led, int duration) {
  digitalWrite(led,HIGH);
  delay(duration);
  digitalWrite(led,LOW);
}
