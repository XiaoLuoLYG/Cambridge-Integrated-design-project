"""
moodle instructions: https://www.vle.cam.ac.uk/mod/page/view.php?id=13110541 step 10 additional stuff:
	-> ensure arguments --baud 115200 --before no_reset
Bluetooth Serial Terminal: https://www.microsoft.com/en-us/p/bluetooth-serial-terminal/9wzdncrdfst8#activetab=pivot:overviewtab
Arduino IDE serial monitor: ensure upload rate is set to 115200 baud (bottom right)


1. open main_Bluetooth/main_Bluetooth.ino 
2. ensure that ESP32 board package is installed. If it isn't, way to get ESP32 package:
   https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
3. upload main_Bluetooth.ino 
4. type "bluetooth and other device settings" in the start menu to open the bluetooth settings
5. click "Add Bluetooth or other device" and search for "M112" and add it (this is the bluetooth name of the arduino)
6. return to "bluetooth and other device settings" again and click "More bluetooth options"
7. open the tab "COM Ports" and search for the outgoing COM port with name "M112 'ESP32SPP'". Note the port number.
8. edit to the port number found earlier.
9. write python in the format shown in test.py
	- entire code should be nested in if __name__ == "__main__":
10. to see data received by arduino over bluetooth, open Serial Monitor and set baud rate to 115200.

"""


# 
import serial # Non standard: pip3 install pyserial

MySerialConnection = serial.Serial(port="COM7", baudrate=115200, timeout=None) # this is where you create the communication class with bluetooth device

def SendSpeedsToRobot(leftspeed, rightspeed): # hopefully two signed ints, +-255
    if abs(leftspeed) > 255 or abs(rightspeed) > 255:
        print("Illegally large wheel speeds not sent:", leftspeed, " ", rightspeed)
    else:
        sendmessage = str(leftspeed) + " " + str(rightspeed) # separate by a whitespace and concat. Can't add then concat or the values will be summed in decimal!
        # print("Sending speeds", sendmessage)
        # MySerialConnection.write(sendmessage.encode())

        # message_btyes
        message_btyes = bytes([leftspeed>=0,abs(leftspeed),rightspeed>=0,abs(rightspeed)])
        print(message_btyes)
        MySerialConnection.write(message_btyes)