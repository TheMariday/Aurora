# around 0
import json

model = []
scale = -1 / 4.0
centerX = 7 / 2.0
centerY = 7 / 2.0


for v in range(8):
    for u in range(8):
        px = u
        py = v
        model[index++] = [  (px - centerX) * scale, 0, (py - centerY) * scale ]