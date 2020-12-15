import serial, time

s = serial.Serial(port="COM7",
                  baudrate=115200,
                  timeout=None) #this is where you create the communication class with bluetooth device
print("start")
while True:
    time.sleep(0.1) #need this to reduce CPU usage of a while loop
    data_in = s.readline().decode() #readline reads from the bluetooth device until a '\n' is detected.
                                    #decode turns the data read, which is in bytes, to a string 
    # print(data_in)
    # write the main code here
    data_out = "hello\n"
    s.write(data_out.encode()) #write sends data to the bluetooth device. data_out is a string and encode turns data_out into bytes