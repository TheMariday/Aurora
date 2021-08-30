import opc
import numpy as np

client = opc.Client('localhost:7890')

WHITE = [256, 256, 256]
BLACK = [0, 0, 0]

pix_buffer = np.zeros((64, 3))

MIDDLE = 30
pix_buffer[31] = WHITE
pix_buffer[25] = WHITE

client.put_pixels(pix_buffer)
client.put_pixels(pix_buffer)
