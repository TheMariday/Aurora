import numpy as np
import cv2
#import matplotlib.pyplot as plt
import opc
from time import sleep
client = opc.Client('localhost:7890')

cap = cv2.VideoCapture(0)

WHITE = [64, 64, 64]
BLACK = [0, 0, 0]

pix_buffer = np.zeros((64, 3))

# width = 1280.0
# height = 720.0
# cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
# cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

# radius = 10


# ax = plt.axes(projection='3d')# Data for a three-dimensional line


for i in range(64):
    pix_buffer = np.zeros((64, 3))
    pix_buffer[i] = WHITE
    client.put_pixels(pix_buffer)
    client.put_pixels(pix_buffer)

    sleep(.3)

    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    cv2.imwrite("led_%i.png" % i, gray)

    ret, frame = cap.read()

    # perform a naive attempt to find the (x, y) coordinates of
    # the area of the image with the largest intensity value

    # gray = cv2.GaussianBlur(gray, (21, 21), 0)
    #
    # (minVal, maxVal, minLoc, maxLoc) = cv2.minMaxLoc(gray)
    # cv2.circle(gray, maxLoc, radius, (255, 0, 0), 2)
    # # display the results of our newly improved method
    # cv2.imshow("Robust", gray)
    #
    # ax.clear()
    # brightest_x = maxLoc[0]
    # brightest_y = maxLoc[1]
    #
    # ax.plot3D([0, 0.5], [0, 1], [0, 0.5], 'blue')
    # ax.plot3D([0, 0.5], [0, 1], [0, -0.5], 'blue')
    # ax.plot3D([0, -0.5], [0, 1], [0, 0.5], 'blue')
    # ax.plot3D([0, -0.5], [0, 1], [0, -0.5], 'blue')
    #
    # ax.plot3D([0, brightest_y/height - 0.5],[0, 1],[0, 0.5-brightest_x/width], 'red')
    # plt.pause(0.05)


cap.release()
# cv2.destroyAllWindows()