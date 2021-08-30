import math
import numpy as np
import cv2


def rotate_point(point, angle):
    x2 = (point[0] * math.cos(angle)) - (point[1] * math.sin(angle))
    y2 = (point[0] * math.sin(angle)) + (point[1] * math.cos(angle))

    return [x2, y2, point[2]]


def draw_axis(ax):
    ax.plot3D([0, .1], [0, 0], [0, 0], 'red')
    ax.plot3D([0, 0], [0, .1], [0, 0], 'green')
    ax.plot3D([0, 0], [0, 0], [0, .1], 'blue')


def intersect(P0, P1):
    """P0 and P1 are NxD arrays defining N lines.
    D is the dimension of the space. This function
    returns the least squares intersection of the N
    lines from the system given by eq. 13 in
    http://cal.cs.illinois.edu/~johannes/research/LS_line_intersect.pdf.
    """
    # generate all line direction vectors
    n = (P1 - P0) / np.linalg.norm(P1 - P0, axis=1)[:, np.newaxis]  # normalized

    # generate the array of all projectors
    projs = np.eye(n.shape[1]) - n[:, :, np.newaxis] * n[:, np.newaxis]  # I - n*n.T
    # see fig. 1

    # generate R matrix and q vector
    R = projs.sum(axis=0)
    q = (projs @ P0[:, :, np.newaxis]).sum(axis=0)

    # solve the least squares problem for the
    # intersection point p: Rp = q
    p = np.linalg.lstsq(R, q, rcond=None)[0]

    return p


def render_location(tag, led_position, display_image):

    if (led_position is None) or (max(led_position) == 0):
        return display_image

    cv2.putText(display_image, tag, tuple(led_position), cv2.FONT_HERSHEY_SIMPLEX, .5, 0, 3)
    cv2.putText(display_image, tag, tuple(led_position), cv2.FONT_HERSHEY_SIMPLEX, .5, 255)

    return display_image
