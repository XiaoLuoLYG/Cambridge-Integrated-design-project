# import BT_handler
from BT_handler import BT_comm, setup

if __name__ == "__main__":
    setup()
    a = BT_comm()
    while a.repeat():
        # run code here
        if a.new_data_available():
            # do something with the new data
            # data is in the form [data1, data2]
            pass
        a.write(data_to_write) #send data, which should be a list of commands [command1, command2]
        