import time
import opc
import numpy as np


class LedTester:

    def __init__(self, led_count=64):
        self.led_count = led_count
        self.led_cols = np.zeros((self.led_count, 3), dtype=int)
        self.led_positions = np.zeros((self.led_count, 3), dtype=float)
        self.client = opc.Client('localhost:7890')

    def add_led(self, led_id, position):
        self.led_positions[led_id] = position

    def wipe(self, direction=0):
        self.led_cols = np.zeros((512, 3), dtype=int)
        mins = np.amin(self.led_positions, axis=0)
        maxs = np.amax(self.led_positions, axis=0)

        for frame in range(100):
            frontier = mins[direction] + (maxs[direction] - mins[direction]) * ((frame - 50) / 40.0)
            for led_id in range(self.led_count):
                if self.led_positions[led_id].any():
                    dist = abs(self.led_positions[led_id, direction] - frontier)
                    if dist < 0.1:
                        self.led_cols[led_id, direction] = 128 * (0.1 - dist) * 5
            self.render()
            time.sleep(0.03)

    def render(self):
        self.client.put_pixels(self.led_cols)
        self.client.put_pixels(self.led_cols)
