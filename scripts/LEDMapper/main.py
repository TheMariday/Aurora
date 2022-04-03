import cv2
import opc
from threading import Thread
from queue import Queue
import time

_sentinel = object()


def show_webcam(output_queue):
    cam = cv2.VideoCapture(0)

    cam.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
    cam.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    cam.read() # do not remove!

    cam.set(cv2.CAP_PROP_AUTOFOCUS, 0)
    cam.set(cv2.CAP_PROP_FOCUS, 0)
    cam.set(cv2.CAP_PROP_AUTO_EXPOSURE, 0)
    cam.set(cv2.CAP_PROP_EXPOSURE, -10)
    cam.set(cv2.CAP_PROP_GAIN, 0)

    while True:
        ret_val, img = cam.read()
        #cv2.imshow('webcam raw', img)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = cv2.blur(gray, (21, 21))
        cv2.imshow('webcam filtered', gray)

        (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray)
        queue_data = (maxVal, maxLoc)
        output_queue.put(queue_data)
        if cv2.waitKey(1) == 27:
            break  # esc to quit

    output_queue.put(_sentinel)
    cv2.destroyAllWindows()

def set_led(c, led_index, brightness):
    pixels = [(0, 0, 0)] * numLEDs
    pixels[int(led_index)] = (brightness, brightness, brightness)
    c.put_pixels(pixels)
    c.put_pixels(pixels)


if __name__ == '__main__':

    numLEDs = 512
    reference_led_index = 0
    max_brightness = 0
    background_brightness = 0
    brightness_threshold = 0
    frame_rate = 0
    client = opc.Client('localhost:7890')

    pixels = [(0, 0, 0)] * numLEDs
    client.put_pixels(pixels)
    client.put_pixels(pixels)

    q = Queue()
    print("starting  camera thread")
    cam_thread = Thread(target=show_webcam, args=(q,))
    cam_thread.start()
    q.get()
    print("started camera thread")

    time.sleep(1)
    q.queue.clear()
    print("Frame rate: ", end='')
    start = time.time()
    for i in range(100):
        print(".", end='')
        q.get()
    end = time.time()
    frame_rate = 100.0/(end - start)
    print(frame_rate)

    print("Background brightness: ", end='')
    for i in range(100):
        print(".", end='')
        maxVal, maxLoc = q.get()
        background_brightness += maxVal

    background_brightness /= 100

    print(background_brightness)

    while True:
        uin = input("Enter the led you want to use as your reference or c to continue: ")

        if uin == "c":
            break

        try:
            reference_led_index = int(uin)
        except:
            continue

        set_led(client, reference_led_index, 255)

        time.sleep(1)
        q.queue.clear()

    print("Reference led: %i" % reference_led_index)
    print("Max brightness: ", end='')
    step = 10
    brightness = 0
    while True:
        brightness += step

        print(".", end='')
        set_led(client, reference_led_index, brightness)
        time.sleep(1)
        q.queue.clear()
        maxVal, maxLoc = q.get()

        if brightness > 255:
            max_brightness = 255
            brightness_threshold = (maxVal + background_brightness) / 2
            break

        if maxVal > 250:
            if step == 10:
                brightness -= step
                step = 1
            else:
                max_brightness = brightness
                brightness_threshold = (maxVal + background_brightness) / 2
                break

    print("%i (%f / %f)" % (max_brightness, maxVal, background_brightness))

    set_led(client, reference_led_index, 0)
    time.sleep(1)

    latency_raw = 0
    print("Latency: ", end='')
    for i in range(0, 30):
        print(".", end='')
        set_led(client, reference_led_index, max_brightness)
        q.queue.clear()

        maxVal = 0
        start = time.time()
        while maxVal < brightness_threshold:
            maxVal, maxLoc = q.get()
        latency_raw += time.time() - start

        set_led(client, reference_led_index, 0)
        time.sleep(0.5)

    latency_raw /= 30.0

    print(latency_raw)

    start = time.time()
    for led_index in range(0, 8):
        set_led(client, led_index, max_brightness)
        time.sleep(latency_raw*1.5)
        q.queue.clear()
        maxVal, maxLoc = q.get()
        print(led_index, maxVal, maxLoc[0], maxLoc[1])

    end = time.time()

    print("total time: ", end - start)
