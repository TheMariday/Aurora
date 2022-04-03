import numpy as np
import cv2 as cv

criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

objp = np.zeros((6*9,3), np.float32)
objp[:,:2] = np.mgrid[0:9,0:6].T.reshape(-1, 2)*25.1

objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.

images = ["D:\\Users\\Sam\\GIT\\temp\\calibration\\capture_%i.png" % i for i in range(22)]
for fname in images:
    img = cv.imread(fname)
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    ret, corners = cv.findChessboardCorners(gray, (9, 6), None)

    if ret == True:

        objpoints.append(objp)
        corners2 = cv.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
        imgpoints.append(corners)

        cv.drawChessboardCorners(img, (9,6), corners2, ret)
        cv.imshow('img', img)
        cv.waitKey(1)

cv.destroyAllWindows()

ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

for fname in images:
    img = cv.imread(fname)
    img = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    h, w = img.shape[:2]
    newcameramtx, roi = cv.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

    # undistort
    dst = cv.undistort(img, mtx, dist, None, newcameramtx)
    # crop the image
    x, y, w, h = roi
    #dst = dst[y:y + h, x:x + w]
    cv.imwrite(fname.replace(".png", "_calibrated.png"), dst)


print("ret", ret)
print("mtx", mtx)
print("dist", dist)
print("rvecs", rvecs)
print("tvecs", tvecs)

"""
focal length = 1156.39183px / 2.54408mm 
center = 939.991762 / 561.724599
Aptina AR0330 1/3" ?
2.2 µm pixel
4.22400mm horisontal

ret 0.2812484750769275
mtx [[1.15639183e+03 0.00000000e+00 9.39991762e+02]
 [0.00000000e+00 1.15571930e+03 5.61724599e+02]
 [0.00000000e+00 0.00000000e+00 1.00000000e+00]]
 
dist [[ 0.09447224 -0.23710961  0.00037585 -0.00044418  0.1098414 ]]

cv2.undistortPoints((939, 562), mtx, dst) > returns vector!


"""

