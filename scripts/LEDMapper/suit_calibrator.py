import mathutils
import numpy as np
import cv2
import math
import itertools

angles = list(range(0,360,45))
distance = 1510
led_count = 5*8*64

view_data = [[] for _ in range(led_count)]

for angle in angles:
    filename = "D:\\Users\\Sam\\GIT\\Aurora\\scripts\\LEDMapper\\calibrations\\calibration_20220407-%i.csv" % angle
    file = open(filename, "r")
    for line in file.readlines():
        idx, brightness, u, v = line.split(",")
        view_data[int(idx)].append([float(angle), int(brightness), int(u), int(v)])
    file.close()


mtx = np.array([[1.15639183e+03, 0.00000000e+00, 9.39991762e+02],
       [0.00000000e+00, 1.15571930e+03, 5.61724599e+02],
       [0.00000000e+00, 0.00000000e+00, 1.00000000e+00]])
dst = np.array([[0.09447224, - 0.23710961,  0.00037585, - 0.00044418,  0.1098414]])



cameraAngles = {}
cameraPositions = {}
offset = -30.3
for angle in angles:
    cameraAngles[angle] = mathutils.Euler((0, 0, np.radians(angle+offset)))
    cameraPositions[angle] = mathutils.Vector((-distance, 0, 0))
    cameraPositions[angle].rotate(cameraAngles[angle])

average_error = 0
c = 0

f = open("blenderPoints.txt", "w")

pointsOut = []


for led_id in range(led_count):

    led_rays = []
    for led_view in view_data[led_id]:
        angle, brightness, u, v = led_view
        if brightness > 200:
            z, y = cv2.undistortPoints((u, v), mtx, dst)[0][0]
            ray = mathutils.Vector((1, -y, -z))
            ray.rotate(cameraAngles[angle])
            end_point = cameraPositions[angle] + ray

            led_rays.append((cameraPositions[angle], end_point))

    if len(led_rays) == 1:
        pointsOut.append((led_id, mathutils.Vector((0, 0, 0))))
        #f.write("%i, %f, %f, %f, %f\n" % (led_id, 0, 0, 0, 0))

    if len(led_rays) < 2:
        continue


    intersectionPoints = []
    intersectionScores = []
    intersectionDiffs = []
    for rays in list(itertools.combinations(led_rays, 2)):
        intersection1, intersection2 = mathutils.geometry.intersect_line_line(rays[0][0], rays[0][1], rays[1][0], rays[1][1])
        diff = (intersection1 - intersection2).magnitude
        if diff > 30:
            continue
        intersectionDiffs.append(diff)
        intersectionPoints.append((intersection1 + intersection2) / 2.0)
        error = (1 - (diff / 30)) ** 2
        intersectionScores.append(error)

    if len(intersectionPoints) == 0:
        continue

    if len(intersectionPoints) > 1:
        worstScoreIndex = np.argmin(intersectionScores)
        del intersectionPoints[worstScoreIndex]
        del intersectionScores[worstScoreIndex]
        del intersectionDiffs[worstScoreIndex]

    m = mathutils.Vector((0,0,0))
    for p in intersectionPoints:
        m += p

    m /= len(intersectionPoints)

    #print(sum(intersectionDiffs) / len(intersectionDiffs))
    pointsOut.append((led_id, m))

for i in range(len(pointsOut)):
    idx, vec = pointsOut[i]
    if vec.magnitude == 0:
        diff = (pointsOut[i-1][1] - pointsOut[i+1][1]).magnitude
        if diff < 100:
            newMid = (pointsOut[i-1][1] + pointsOut[i+1][1]) / 2.0
            pointsOut[i] = (idx, newMid)


for p in pointsOut:
    f.write("%i, %f, %f, %f\n" % (p[0], p[1].x, p[1].y, p[1].z, ))
f.close()
