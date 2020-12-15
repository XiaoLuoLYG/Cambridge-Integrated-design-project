#include <SPI.h>
#include <Wire.h>
// So this simple module will provide a function or two to spin the wheels. 
// please write in this header the syntax and usage of these functions. 
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port'. MotorM1 should be connected to the LEFT WHEEL, MotorM1 should be connected to the RIGHT WHEEL. 0 represnts forward, 1 represents backward.
Adafruit_DCMotor *myMotorM1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorM2 = AFMS.getMotor(2);


void activateMotorM2 (int speedInput, int forward) {
  // speedInput from 0 - 255
  // forward = 1 is moving forward, forward = 0 is the reverse direction
  myMotorM2->setSpeed(speedInput); // set speed
  myMotorM2->run(RELEASE); // turn on motor
  Serial.println("Running small motor...");

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
  myMotorM1->setSpeed(speedInput); // set speed
  myMotorM1->run(RELEASE); // turn on motor
  Serial.println("Running large motor...");

  // select direction
  if (forward == 1) {
    myMotorM1->run(FORWARD);
  } else {
    myMotorM1->run(BACKWARD);
  };
}
#include "arduino_secrets.h"
#include <WiFiNINA.h>



///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void WifiListener() {
 // Wifi Server Listener Code --> Listens to any HTTP request from the master PC
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // IMPORTANT: LISTENS TO COMMANDS HERE.
        // Write an if() for any command in this section.
        
        if (c == '\r') {
          if (currentLine.endsWith("HTTP/1.1")){
            String inputCommand = getValue(currentLine, ' ', 1); // Whole command
            String commandType = getValue(inputCommand, '?', 0); // Type of command
            String commandArgs = getValue(inputCommand, '?', 1); // Args of command

//            // Used for debugging
//            Serial.println(inputCommand);
//            Serial.println(commandType);
//            Serial.println(commandArgs);

             // MOTOR COMMANDS START
            if (commandType == "/HM1") {
              // HM1 is High, Motor M1. Collects the args
              String speedArg = getValue(commandArgs, '&', 0);
              String directionArg = getValue(commandArgs, '&', 1);
              String speedInput = getValue(speedArg, '=', 1);
              String directionInput = getValue(directionArg, '=', 1);
              // Calls function
              activateMotorM1(speedInput.toInt(), directionInput.toInt());       
            }

            if (commandType == "/LM1") {
              // LM1 is Low, Motor M1. Calls function.
              myMotorM1->run(RELEASE);
            }

            if (commandType == "/HM2") {
              // HM2 is High, Motor M2. Collects the args
              String speedArg = getValue(commandArgs, '&', 0);
              String directionArg = getValue(commandArgs, '&', 1);
              String speedInput = getValue(speedArg, '=', 1);
              String directionInput = getValue(directionArg, '=', 1);
              // Calls function
              activateMotorM2(speedInput.toInt(), directionInput.toInt());     
            }

            if (commandType == "/LM2") {
              // LM2 is Low, Motor M2. Calls function.
              myMotorM2->run(RELEASE);
            }

            // LINE FOLLOWING CODE

            // MOTOR COMMANDS END
        
          }

        }
      }
    }
    client.stop();
    Serial.println("client disconnected");
  }
}

// Function to display wifi status when initialising
void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

// Splits a string based on delimiter
String getValue(String stringToSplit, char separator, int index) {
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

// LINE FOLLOWER SENSOR INPUTS (ANALOG)
int left_sensor_pin = A0;
int middle_sensor_pin= A1;
int right_sensor_pin = A2;

int line_follower_cutoff = 100; 



int analogToBinary (int analogInput, int binaryCutOff) {
  return analogInput < binaryCutOff ? 0 : 1;
}

int readLineSensor(int sensor_pin) {
  return analogToBinary(analogRead(sensor_pin),line_follower_cutoff); 
}
bool line_follower_toggle = false;

void setup() {
  // initialize serial communication for debugging
  Serial.begin(9600);

  // Initialise the Motor Shield. Create with the default frequency 1.6KHz
  AFMS.begin();

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}

void loop() {
  WifiListener();

  if (line_follower_toggle) {
    // TO BE COMPLETED
    Serial.println("Work in Progress");
  }
  
}
