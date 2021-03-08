import cv2
import numpy as np
from helpers import rotate_point


class Camera:
    def __init__(self, cam_id, colour='black', led_count=64, resolution=(640, 480), angle=0):
        self.cam_id = cam_id
        self.led_count = led_count
        self.resolution = resolution
        self.width, self.height = resolution
        self.sequence_fp = "D:\\Users\\Sam\\GIT\\Aurora\\LEDMapper\\seqences\\head_test\\cam_%i" % self.cam_id
        self.frames = np.zeros((self.led_count, self.height, self.width), dtype=np.uint8)
        self.frames_blurred = self.frames.copy()
        self.angle = angle
        self.position = rotate_point(np.array((-.5, 0, 0)), self.angle)
        self.colour = colour

    def load_frames(self):
        for led_id in range(self.led_count):
            filename = "%s\\led_%i.png" % (self.sequence_fp, led_id)
            self.frames[led_id] = cv2.imread(filename, 0)

    def play_frames(self):
        for led_id in range(self.led_count):
            cv2.imshow("cam_%i" % self.cam_id, self.frames[led_id])
            cv2.waitKey(10)
        cv2.waitKey(-1)

    def locate_led(self, led_id, blur=15, threshold=200):

        frame_blurred = cv2.GaussianBlur(self.frames[led_id], (blur, blur), 0)

        _, max_brightness, _, max_location = cv2.minMaxLoc(frame_blurred)

        if max_brightness > threshold:
            return max_location
        else:
            return None

    def ray_from_position(self, position):

        ray_end = [+0.5,
                   0.5 - position[1] / float(self.width),
                   +0.5 - position[0] / float(self.height)]

        ray_end_rotated = rotate_point(ray_end, self.angle)

        return [self.position[0], ray_end_rotated[0]], \
               [self.position[1], ray_end_rotated[1]], \
               [self.position[2], ray_end_rotated[2]]

    def render3d(self, ax):
        ax.scatter(self.position[0], self.position[1], self.position[2], marker='o', color=self.colour)

