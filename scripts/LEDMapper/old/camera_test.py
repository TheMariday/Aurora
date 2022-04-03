import numpy as np
import cv2
import time

# Video source - can be camera index number given by 'ls /dev/video*
# or can be a video file, e.g. '~/Video.avi'
cap = cv2.VideoCapture(0)

#cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
#cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

lastFrame = None

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)


    # Display the resulting frame
    if lastFrame is None:
        lastFrame = frame.copy() #cv2.GaussianBlur(frame, (9, 9), 0)
    else:
        cv2.imshow('frame', cv2.absdiff(frame, lastFrame))
        if cv2.waitKey(1) == ord('c'):
            lastFrame = frame.copy()  # cv2.GaussianBlur(frame, (9, 9), 0)

        #time.sleep(1)

    #lastFrame = frame.copy()

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()