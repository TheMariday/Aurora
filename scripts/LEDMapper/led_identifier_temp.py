import numpy as np
import matplotlib.pyplot as plt
import cv2
from camera import Camera
from helpers import draw_axis
from led import LED
from led_tester import LedTester

if __name__ == "__main__":

    print("Initialising LED Identifier")
    led_count = 64
    cam_count = 3
    default_scale = 0.1
    render = True
    ax = None

    led_info = np.zeros((led_count, 3))

    cams = [None] * cam_count
    angles = [45, 0, -45]
    colours = ['red', 'green', 'blue']

    print("Loading Camera data")

    d = []
    t = []

    for cam_id in range(cam_count):
        cam = Camera(cam_id, colour=colours[cam_id], angle=angles[cam_id])
        cam.load_frames()

        for led_id in range(led_count):
            frame_blurred = cv2.GaussianBlur(cam.frames[led_id], (15, 15), 0)

            mean = cv2.mean(frame_blurred)

            _, max_brightness, _, max_location = cv2.minMaxLoc(frame_blurred)

            d.append(max_brightness/mean[0])
            t.append(max_brightness)

    d.sort(reverse=True)
    t.sort(reverse=True)
    plt.plot(d)
    # plt.plot(t)
    plt.show()