import serial
import time
import multiprocessing as mp
from ctypes import c_int, c_wchar_p, c_long, c_char


# parameters to change based on individual PC config
BT_PORT = "COM7"
BT_BAUDRATE = 115200

# other global parameters
BT_BUFFER_SIZE = 1000 #buffer size in number of bytes for read/write
BT_STATE = mp.Value(c_int)
BT_BUFFER = mp.Array(c_char, BT_BUFFER_SIZE) #buffer to store up to BT_BUFFER_SIZE bytes of incoming data
BT_WRITE_BUFFER = mp.Array(c_char, BT_BUFFER_SIZE) #buffer to store up to BT_BUFFER_SIZE bytes of outgoing data
BT_TIMEOUT = None #use None for listener to wait till EOF '\n' is read
BT_STATE.value = 0 #0 for deactivated, 1 for activated

class __BT_listener:
    def __init__(self):
        self.__s = None
        self.state = 0 #0 for deactivated, 1 for activated

    def begin_BT(self, port=BT_PORT,
                      baudrate=BT_BAUDRATE,
                      timeout=BT_TIMEOUT):        
        self.__port = port 
        self.__baudrate = baudrate
        self.__timeout = timeout
        self.__s = serial.Serial(port=self.__port, baudrate=self.__baudrate, timeout=self.__timeout)
        self.state = 1
        
    
    def close_BT(self):
        self.__s.close()
        self.state = 0
    
    def read(self):
        x = self.__s.readline() #read till a '/n' byte is read
        return x
    
    def write(self, data):
        self.__s.write(data.encode())

def __BT_parallel(bt_state, bt_buffer, bt_write_buffer):
    '''parallel thread used to handle bluetooth comms'''
    bt = __BT_listener()
    while True:
        time.sleep(0.1) #prevents overusage of processor for this thread. lower the value if missing data
        if bt_state.value == 1:
            if bt.state == 0: 
                try: 
                    bt.begin_BT()
                    print("Bluetooth Serial Port opened")
                except:
                    print("Failed to connect to Bluetooth Serial Port")
            if bt_buffer[-1] == b'\x00': #will not store anymore data once buffer is  full
                newdata = bt.read()
                bt_buffer[bt_buffer[:].index(b'\x00'): bt_buffer[:].index(b'\x00')+len(newdata)] = newdata #stores the data in the next slot of bt_data
            if bt_write_buffer[0] != b'\x00':
                bt.write(bt_write_buffer[:bt_write_buffer[:].index(b'\x00')]) #write data on buffer into serial 
                bt_write_buffer[:] = b'\x00' * len(bt_write_buffer) #flush write buffer
        if bt_state.value == 0:
            if bt.state == 1: bt.close_BT()

class BT_comm:
    def __init__(self):
        self.__flag_newdata = False
        self.__previous_time = None

    def new_data_available(self):
        return self.__flag_newdata
    
    def new_data(self, flush=True):
        '''returns a list of all of the incoming data that are still unread in the buffer.
        flush: flush buffer once data is read'''
        data = BT_BUFFER[:]
        if flush: 
            BT_BUFFER[:] = b'\x00' * len(BT_BUFFER)
            self.__flag_newdata = False
        return data.decode().split('\n')[:-2]
    
    def write(self, data):
        '''data should be list of strings'''
        string_to_write = ""
        for d in data:
            string_to_write += d
            string_to_write += "\n"
        BT_WRITE_BUFFER[:len(string_to_write)] = string_to_write.encode()


    def repeat(self, cycle_time=1):
        '''Use in a while loop to control cycle. Cycle_time is in seconds.
        Ensures a set frequency of cycles, independent of how long it takes to execute code in each cycle.'''
        if BT_BUFFER[0] != b'\x00': self.__flag_newdata = True
        if self.__previous_time is not None:
            while time.time() - self.__previous_time < cycle_time:
                time.sleep(cycle_time/100)
        self.__previous_time = time.time()
        return True


def setup():
    p = mp.Process(target=__BT_parallel, args=(BT_STATE, BT_BUFFER, BT_WRITE_BUFFER))
    p.start()
    BT_STATE.value = 1

# if __name__ == "__main__":
#     setup()
#     while True:
#         time.sleep(1)
#         print("main")
#         print(BT_BUFFER[:])

    