import serial
import time


class CurrentController:

    def __init__(self, port='COM5', baud=57600, boardCount=8):
        self.ard = serial.Serial(port,baud)
        self.delimeter = b'\xff\xff'
        self.boardCount = boardCount
        self.packet_size = self.boardCount*3 + len(self.delimeter)

        time.sleep(0.5)

        self.boardCurrent = [0]*boardCount
        self.boardFET = [0]*boardCount

        self.calibrationOffset = -420
        self.calibrationMultiplier = 61

        self.disableAll()

    def __repr__(self):
        s = "CurrentController:\n"
        for board in range(self.boardCount):
            s += "board %i: " % board + ("enabled" if self.boardFET[board] else "disabled") + ", Draw = %i\n" % self.boardCurrent[board]
        
        return s

    def disableAll(self):
        for board in range(self.boardCount):
            self.setBoardFET(board, False)

    def setBoardFET(self, board, state):
        s = chr(board*2+int(state)).encode()
        print("setting board to", board, state, s)
        self.ard.write(s)
        #self.ard.flush()

    def getBoardFET(self, board):
        return self.boardFET[board]

    def getBoardCurrent(self, board):
        return self.boardCurrent[board]

    def flush(self):
        self.ard.flushInput()
        msg = self.ard.read_until(self.delimeter);

    def refresh(self, flush=False):

        if flush:
            self.flush()

        msg = self.ard.read_until(self.delimeter);

        if len(msg) != self.packet_size:
            print("Recieved message not long enough!")
            return False


        for board in range(8):
            lsb, msb, fet  = msg[board*3:board*3+3]

            current = (msb << 8 | lsb)

            #current += self.calibrationOffset
            #current *= self.calibrationMultiplier

            self.boardCurrent[board] = current
            self.boardFET[board] = fet

        return True

