import cv2

sequence_fp = "D:\\Users\\Sam\\GIT\\Aurora\\LEDMapper\\seqences\\head_test\\"
blur_radius = 15
threshold = 200

for cam_id in range(3):

    display_image = None

    for led_id in range(64):

        filename = sequence_fp + "cam_%i\\led_%i.png" % (cam_id, led_id)

        image = cv2.imread(filename, 0)

        if display_image is None:
            display_image = image.copy()

        image_blurred = cv2.GaussianBlur(image, (blur_radius, blur_radius), 0)

        _, max_brightness, _, max_location = cv2.minMaxLoc(image_blurred)

        if max_brightness > threshold:
            cv2.putText(display_image, "L%i" % led_id, max_location, cv2.FONT_HERSHEY_SIMPLEX, .5, 255)

    cv2.imshow("cam_%i" % cam_id, display_image)

cv2.waitKey(-1)
