import mathutils
import numpy as np
import cv2
import itertools


class Camera:

    def __init__(self, angle, distance, mtx, dst):
        self.angle = mathutils.Euler((0, 0, angle))
        self.position = mathutils.Vector((-distance, 0, 0))
        self.position.rotate(self.angle)
        self.LEDs = {}
        self.mtx = mtx
        self.dst = dst
        self.distance = distance
        self.jitter = (0, 0, 0)

    def apply_jitter(self):
        self.angle.x += self.jitter[0]
        self.angle.y += self.jitter[1]
        self.angle.z += self.jitter[2]
        self.position.x += self.jitter[3]
        self.position.y += self.jitter[4]
        self.position.z += self.jitter[5]

    def set_jitter(self, jitter=0.001):
        self.jitter = (np.random.normal(0, jitter), np.random.normal(0, jitter), np.random.normal(0, jitter),
                       np.random.normal(0, jitter*100), np.random.normal(0, jitter*100), np.random.normal(0, jitter*100))

    def set_rotation(self, angle):
        self.angle = mathutils.Euler((0, 0, np.radians(angle)))
        self.position = mathutils.Vector((-self.distance, 0, 0))
        self.position.rotate(self.angle)

    def get_rotation(self):
        return np.degrees(self.angle.x), np.degrees(self.angle.y), np.degrees(self.angle.z)


    def load_file(self, filename, detect_limit=200):
        file = open(filename, "r")
        self.LEDs = {}
        for line in file.readlines():
            idx, brightness, u, v = [int(a) for a in line.split(",")]
            if brightness > detect_limit:
                self.LEDs[idx] = (brightness, u, v)
        file.close()

    def can_see(self, led_id):
        return led_id in self.LEDs

    def project(self, led_id, rotation_jitter=None):
        if not self.can_see(led_id):
            return False

        brightness, u, v = self.LEDs[led_id]
        z, y = cv2.undistortPoints((u, v), self.mtx, self.dst)[0][0]
        ray = mathutils.Vector((1, -y, -z))
        jitteredAngle = self.angle.copy()
        jitteredAngle.x += self.jitter[0]
        jitteredAngle.y += self.jitter[1]
        jitteredAngle.z += self.jitter[2]
        ray.rotate(jitteredAngle)
        posJitter = mathutils.Vector(self.jitter[3:])
        end_point = self.position + ray + posJitter
        start_point = self.position.copy()
        start_point += posJitter
        return start_point, end_point


class LED:

    def __init__(self, position, type = "regular"):
        self.position = position
        self.type = type


class LED_Points:

    def __init__(self):
        self.data = {}

    def add(self, led_id, position):
        self.data[led_id] = LED(position)

    def write(self, filename):
        f = open(filename, 'w')
        for led_id in self.data:
            led = self.data[led_id]
            f.write("%i, %f, %f, %f\n" % (led_id, led.position.x, led.position.y, led.position.z,))
        f.close()

    def interpolate(self):
        interpolated_count = 0
        keys = list(self.data.keys()) # allows us to add stuff to the dict whilst looping
        for led_id in keys:
            if (led_id + 1 not in self.data) and (led_id + 2 in self.data):
                jump_distance = (self.data[led_id].position - self.data[led_id + 2].position).magnitude
                if jump_distance < 80:
                    midpoint = (self.data[led_id].position + self.data[led_id + 2].position) / 2.0
                    self.data[led_id + 1] = LED(midpoint, "interpolated")
                    interpolated_count += 1


angle_offset = 0 #-30.3
angles = list(range(0, 360, 45))
distance = 1510
led_count = 5*8*64

mtx = np.array([[1.15639183e+03, 0.00000000e+00, 9.39991762e+02],
       [0.00000000e+00, 1.15571930e+03, 5.61724599e+02],
       [0.00000000e+00, 0.00000000e+00, 1.00000000e+00]])
dst = np.array([[0.09447224, - 0.23710961,  0.00037585, - 0.00044418,  0.1098414]])


cameras = []
for angle in angles:
    filename = "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\calibrations\\calibration_20220407-%i.csv" % angle
    c = Camera(np.radians(angle+angle_offset), distance, mtx, dst)
    c.load_file(filename)
    cameras.append(c)

worst_rmse = 1607811
lowest_rmse = 1607811

for i in range(500):

    led_points = LED_Points()

    for c in cameras:
        c.set_jitter(0.001 * (1 - (i / 500.0)))

    total_rmse = 0

    for led_id in range(led_count):

        valid_cameras = []

        for camera in cameras:
            if camera.can_see(led_id):
                valid_cameras.append(camera)

        if len(valid_cameras) <= 1:
            continue

        intersection_points = []
        for camera_a, camera_b in list(itertools.combinations(valid_cameras, 2)):

            start_a, end_a = camera_a.project(led_id)
            start_b, end_b = camera_b.project(led_id)
            intersection_a, intersection_b = mathutils.geometry.intersect_line_line(start_a, end_a, start_b, end_b)
            intersection_points.append(intersection_a)
            intersection_points.append(intersection_b)


        midpoint = mathutils.Vector((0, 0, 0))
        for p in intersection_points:
            midpoint += p
        midpoint /= len(intersection_points)

        rmse = 0
        for point in intersection_points:
            distance_to_midpoint = (point - midpoint).magnitude
            rmse += (distance_to_midpoint) ** 2

        #print("led %i rmse: %f" % (led_id, rmse))
        total_rmse += rmse
        led_points.add(led_id, midpoint)


    if total_rmse < lowest_rmse:
        lowest_rmse = total_rmse
        for c in cameras:
            c.apply_jitter()

        led_points.interpolate()
        led_points.write("blenderPoints_optimised.txt")

    str = "%f, %f, " % (total_rmse, lowest_rmse)
    for c in cameras:
        str += "%f,%f,%f," % (np.degrees(c.angle.x), np.degrees(c.angle.y), np.degrees(c.angle.z))
        str += "%f,%f,%f," % (c.position.x, c.position.y, c.position.z)

    print(str)

