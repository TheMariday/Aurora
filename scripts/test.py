#!/usr/bin/env python

# Light each LED in sequence, and repeat.

import opc, time

numLEDs = 5
client = opc.Client('raspberrypi:3621')

while True:
	for i in range(numLEDs):
		pixels = [ (0,0,0) ] * numLEDs
		pixels[i] = (255, 255, 255)
		client.put_pixels(pixels, channel=3)
		time.sleep(0.1)
