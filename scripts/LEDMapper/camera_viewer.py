import cv2

if __name__ == '__main__':
    cam = cv2.VideoCapture(0)

    cam.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
    cam.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    cam.read()  # do not remove!

    cam.set(cv2.CAP_PROP_AUTOFOCUS, 0)
    cam.set(cv2.CAP_PROP_FOCUS, 0)

    c = 0
    while True:
        ret_val, img = cam.read()
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray_small = cv2.resize(gray, (0, 0), fx=0.3, fy=0.3)
        cv2.imshow('my webcam', gray_small)
        key = cv2.waitKey(1) & 0xFF

        if key == ord('s'):
            filename = "D:\\Users\\Sam\\GIT\\temp\\capture_%i.png" % c
            cv2.imwrite(filename, gray)
            c += 1
