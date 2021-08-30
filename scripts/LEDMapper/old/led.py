import numpy as np
from helpers import intersect


class LED:
    def __init__(self, id, cams):
        self.id = id
        self.cams = cams
        self.location = None  # should be[x, y, z]
        self.uv_positions = {}  # cam_id:(u, v)
        self.localisable = False
        self.rays = None

    def localisation_certainty(self):
        return len(self.uv_positions)

    def locate2D(self, min_views=2):

        for cam in self.cams:
            pos = cam.locate_led(self.id)
            if pos is not None:
                self.uv_positions[cam.cam_id] = pos

        self.localisable = len(self.uv_positions) >= min_views

    def localise3D(self):

        if not self.localisable:
            return

        self.rays = np.zeros((len(self.uv_positions), 3, 2))
        c = 0
        for cam_id, vu_position in self.uv_positions.items():
            uv_position = [vu_position[1], vu_position[0]]
            self.rays[c] = np.array(self.cams[cam_id].ray_from_position(uv_position))
            c += 1

        self.location = intersect(self.rays[:, :, 0], self.rays[:, :, 1]).flatten()

    def render3D(self, ax):
        if self.location is None:
            return
        ax.scatter(self.location[0], self.location[1], self.location[2], marker='o', color='black')
        ax.text(float(self.location[0]), float(self.location[1]), float(self.location[2]), " L%i" % self.id, None)

    def renderRays3D(self, ax, alpha=0.1):
        if self.rays is None:
            return
        for i, ray in enumerate(self.rays):
            colour = self.cams[list(self.uv_positions.keys())[i]].colour #hacky af
            ax.plot3D(ray[0], ray[1], ray[2], colour, alpha=alpha)
