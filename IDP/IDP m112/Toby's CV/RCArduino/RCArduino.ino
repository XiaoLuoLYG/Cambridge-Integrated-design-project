// Minimal all in one script for recieving speeds from bluetooth and sending them to the wheels
// Note arduino indentation is two spaces rather than python 4

#include <Adafruit_MotorShield.h>
#include <string.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorLeft = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorRight = AFMS.getMotor(1);

int leftspeed;
int rightspeed;

void setup() {
  AFMS.begin();
  pinMode(NINA_RESETN, OUTPUT);
  digitalWrite(NINA_RESETN, LOW);

  Serial.begin(9600);
  Serial.println("begin run");
  SerialNina.begin(115200); // it seems different baudrates are required for usb and bluetooth. 
  pinMode(5,INPUT); //for control button for stop
}

void loop() {
  int button_state = digitalRead(5);
  if (button_state==1) SetMotorSpeeds(0, 0); //stop
  
  if (SerialNina.available()) { 
//    String command = SerialNina.readStringUntil("\n"); // receive a string of two signed ints separated with a whitespace

//    String leftspeed = GetElementFromSplitStringByIndex(command, ' ', 0); // split command at whitespace into the two signed ints.
//    String rightspeed = GetElementFromSplitStringByIndex(command, ' ', 1);
  
////    Serial.println("Command " + command);
//
//    Serial.println("Left Speed: "+String(leftspeed)+" and Right Speed: "+String(rightspeed));
//    SetMotorSpeeds(leftspeed.toInt(), rightspeed.toInt()); //send extracted ints to wheels
    
    unsigned char command[4];
    SerialNina.readBytes(command,4);
    Serial.print("Command received: ");
    Serial.print(command[0]);
    Serial.print(command[1]);
    Serial.print(command[2]);
    Serial.println(command[3]);
    if (command[0] == 0){
      leftspeed = -command[1];
    }else{
      leftspeed = command[1];
    }
    if (command[2] == 0){
      rightspeed = -command[3];
    }else{
      rightspeed = command[3];
    }
    Serial.println("Left Speed: "+String(leftspeed)+" and Right Speed: "+String(rightspeed));
    SetMotorSpeeds(leftspeed, rightspeed); //send extracted ints to wheels
  }
}


void SetMotorSpeeds(int speedinputLeft, int speedinputRight) {
  // speedinputs can be positive or negative ints +-255

  // Left motor:
  Serial.println("send info to motor");
  myMotorLeft->setSpeed(abs(speedinputLeft)); // set speed
  myMotorLeft->run(RELEASE); // turn on motor
  Serial.println("Running left motor...");
  Serial.println(speedinputRight);
   if (speedinputLeft < 0) {
    // activateMotorRight(abs(speedinputRight),0);
    myMotorLeft->run(FORWARD); // which appears to now be backwards
  } else {  
      myMotorLeft->run(BACKWARD); // which appears to now be forwards
  }

  // Right motor:
  myMotorRight->setSpeed(abs(speedinputRight)); // set speed
  myMotorRight->run(RELEASE); // turn on motor
  Serial.println("Running right motor...");
  Serial.println(speedinputRight);
   if (speedinputRight < 0) {
    // activateMotorRight(abs(speedinputRight),0);
    myMotorRight->run(FORWARD); // which appears to now be backwards
  } else {  
      myMotorRight->run(BACKWARD); // which appears to now be forwards
  }
}

// Splits a string based on delimiter. Not sure why strtok() doesn't do the job. 
String GetElementFromSplitStringByIndex(String stringToSplit, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = stringToSplit.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (stringToSplit.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? stringToSplit.substring(strIndex[0], strIndex[1]) : "";
}
