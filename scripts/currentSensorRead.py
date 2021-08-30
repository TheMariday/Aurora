import serial
import time

class CurrentController:

    def __init__(self, port='COM9', baud=57600, boardCount=8, timeout=1):
        self.ard = serial.Serial(port, baud, timeout=timeout)
        time.sleep(2)

        self.send(100)
        if self.read() != 1:
            print("Failed to handshake")

    def send(self, val):
        self.ard.write(chr(val).encode())
    
    def read(self):

        msg  = self.ard.read(2);
        if(len(msg) != 2):
            print("didn't recieve enough data")
            return 0
        lsb, msb = msg
        return msb << 8 | lsb

    def setFet(self, board, state):
        self.send(board+state*8)
        return self.read()

    def getCurrent(self, board):
        self.send(board+8*2)
        return self.read()

    def getAll(self):
        self.send(24);
        
        currents = []
        for _ in range(8):
            currents.append(self.read())
            if currents[-1] == 0:
                return 0;

        return currents

def printAll(values):

    print("-"*50)
    for i in range(8):
        print("%i: %i %s" % (i, values[i], int((values[i]-400)/5)*"#"))

def unitsToAmps(units):
    return (units-494.8)*(25/1024)

if __name__ == "__main__":

    cc = CurrentController("COM10")

    #unloaded noise deviance = 32mA
    #unloaded noise range    = 200mA

    board = 7
    before = cc.getCurrent(board)
    after = cc.setFet(board,True)

    print(after-before)

