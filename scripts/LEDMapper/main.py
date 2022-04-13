import cv2
import opc
from threading import Thread
from queue import Queue
import time

_sentinel = object()
LED_COUNT = 512 + 64
led_offset = 2560


def show_webcam(cam, output_queue):

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
        #gray = cv2.blur(gray, (21, 21))

        (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray)
        queue_data = (maxVal, maxLoc)
        output_queue.put(queue_data)

        gray_small = cv2.resize(gray, (0, 0), fx=0.3, fy=0.3)
        cv2.imshow('webcam filtered', gray_small)

        if cv2.waitKey(1) == 27:
            break  # esc to quit

    output_queue.put(_sentinel)
    cv2.destroyAllWindows()


def set_led(c, led_index, brightness):
    pixels = [(0, 0, 0)] * (LED_COUNT + led_offset)
    pixels[int(led_index)] = (brightness, brightness, brightness)
    c.put_pixels(pixels)
    c.put_pixels(pixels)


if __name__ == '__main__':

    reference_led_index = 0
    led_value = 0
    led_brightness = 0
    background_brightness = 0

    client = opc.Client('raspberrypi:7890')

    set_led(client, 1, 255)

    q = Queue()
    print("starting  camera thread")
    cam = cv2.VideoCapture(0)
    cam_thread = Thread(target=show_webcam, args=(cam, q,))
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

    while True:

        uin = input("Enter the brightness and the exposure (255 -10) to use as your reference or c to continue: ")

        if uin == "c":
            break

        try:

            led_value, exposure = uin.split(" ")
            led_value = int(led_value)
            exposure = float(exposure)

            cam.set(cv2.CAP_PROP_EXPOSURE, exposure)

            set_led(client, reference_led_index, led_value)
            time.sleep(1)
            q.queue.clear()
            led_brightness, _ = q.get()

            print("led on %i" % led_brightness)

            set_led(client, reference_led_index, 0)
            time.sleep(1)
            q.queue.clear()
            background_brightness, _ = q.get()

            print("led off %i" % background_brightness)

            print("diff %i" % (led_brightness - background_brightness))

            brightness_threshold = (background_brightness + led_brightness) / 2

            print("threshold: %i" % brightness_threshold)

        except:
            print("something went wrong")
            continue

    set_led(client, reference_led_index, 0)
    time.sleep(1)

    latency_raw = 0

    print("Latency: ", end='')
    for _ in range(0, 50):
        print(".", end='')
        set_led(client, reference_led_index, led_value)
        q.queue.clear()

        maxVal = 0
        start = time.time()
        while maxVal < (led_brightness + background_brightness) / 2.0:
            maxVal, maxLoc = q.get()
        latency_raw += time.time() - start

        set_led(client, reference_led_index, 0)
        time.sleep(0.5)

    latency_raw /= 50.0

    print(latency_raw)

    filename = "calibration_" + time.strftime("%Y%m%d-%H%M%S") + ".csv"

    csv_file = open(filename, 'w')


    try:
        start = time.time()
        for led_index in range(led_offset, led_offset+LED_COUNT):
            set_led(client, led_index, led_value)
            time.sleep(latency_raw*2)
            q.queue.clear()
            maxVal, maxLoc = q.get()
            data = (led_index, maxVal, maxLoc[0], maxLoc[1])
            csv_file.write("%i, %i, %i, %i\n" % data)

            if led_index % 10 == 0:
                time_per_led = (time.time() - start) / (led_index + 1)
                print()
                print("Time remaining: %i " % int(time_per_led*(LED_COUNT-led_index)))

            print(data, end=' ')
        print("done")

    finally:
        csv_file.close()
        print("closed")
