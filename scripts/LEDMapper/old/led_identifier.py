import numpy as np
import matplotlib.pyplot as plt

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

    for cam_id in range(cam_count):
        print("Loading camera %i" % cam_id)
        cams[cam_id] = Camera(cam_id, colour=colours[cam_id], angle=angles[cam_id])
        cams[cam_id].load_frames()

    if render:
        ax = plt.axes(projection='3d')  # Data for a three-dimensional line

        ax.set_xlim3d(-default_scale, default_scale)
        ax.set_ylim3d(-default_scale, default_scale)
        ax.set_zlim3d(-default_scale, default_scale)
        ax.view_init(elev=30, azim=180+45)
        draw_axis(ax)

        for i, cam in enumerate(cams):
            cam.render3d(ax)

    led_tester = LedTester()

    led_certainties = {}

    print("Localising LEDs: ", end = '')



    for led_id in range(led_count):

        led = LED(led_id, cams)
        led.locate2D()

        certainty = led.localisation_certainty()

        print("_.-'^"[min(certainty, 4)], end='')

        if certainty in led_certainties:
            led_certainties[certainty] += 1
        else:
            led_certainties[certainty] = 1

        if led.localisable:
            led.localise3D()
            # print("LED %i localised at location: %r" % (led_id, led.location))
            if render:
                led.render3D(ax)
                led.renderRays3D(ax)
            led_tester.add_led(led_id, led.location)

    print("")

    sorted_certainties = sorted(led_certainties)
    for certainty in sorted_certainties:
        print("%i Views: %i LEDs" % (certainty, led_certainties[certainty]))

    # for direction in range(3):
    #     print("Running test wipe in direction %s" % "xyz"[direction])
    #     led_tester.wipe(direction)

    if render:
        plt.show()