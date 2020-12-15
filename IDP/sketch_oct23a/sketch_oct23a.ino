/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will print the IP address of your WiFi module (once connected)
  to the Serial monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 9.

  If the IP address of your board is yourAddress:
  http://yourAddress/H turns the LED on
  http://yourAddress/L turns it off

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the Wifi.begin() call accordingly.

  Circuit:
   Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and UNO WiFi Rev.2)
   LED attached to pin 9

  created 25 Nov 2012
  by Tom Igoe
*/
#include <SPI.h>
#include <WiFiNINA.h>



// Importing Libraries
#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotorM1 = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorM2 = AFMS.getMotor(2);

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "LENOVOR480_801_ 2781";        // your network SSID (name)
char pass[] = "4e7F1/63";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  // initialize serial communication for debugging
  Serial.begin(9600);

  // Initialise the Motor Shield. Create with the default frequency 1.6KHz
  AFMS.begin();

  
  // Initialise Wifi:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

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
              activateMotorM1(speedInput.toInt(), directionInput.toInt());              // GET /H turns the LED on 
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
              activateMotorM2(speedInput.toInt(), directionInput.toInt());              // GET /H turns the LED on 
            }

            if (commandType == "/LM2") {
              // LM2 is Low, Motor M2. Calls function.
              myMotorM2->run(RELEASE);
            }
            // MOTOR COMMANDS END
        
          }

        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

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
