import currentSensorRead
import opc
import time


if __name__ == '__main__':
    
    cc = currentSensorRead.CurrentController("/dev/ttyUSB0")
    client = opc.Client('localhost:7890')

    cc.flush()

    f = False
    while True:
        cc.setBoardFET(7, f)
        f = not f
        time.sleep(2)
        print("looping")


    # for i in range(10):

    #     s = 25.5*i
    #     pixels = [ (s,s,s) ] * 64

    #     client.put_pixels(pixels)
    #     client.put_pixels(pixels)
    #     time.sleep(1)

    #     cc.flush()
    #     c = 0
    #     for i in range(10):
    #         cc.refresh()
    #         c += cc.getBoardCurrent(7)

    #     print(int(c/10))

