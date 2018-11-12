import cv2
import numpy as np
import datetime

# adapted from https://stackoverflow.com/questions/44588279/find-and-draw-the-largest-contour-in-opencv-on-a-specific-color-python

# keep the time slot in increasing hour order
time_slot_info = [
    {
        # early morning
        "area": 20000,
        "start_time": datetime.time(hour=0, minute=0, second=0)
    },
    {
        # morning
        "area": 20000,
        "start_time": datetime.time(hour=7, minute=0, second=0)
    },
    {
        # night
        "area": 30000,
        "start_time": datetime.time(hour=18, minute=0, second=0)
    }
]


def get_area_threshold()->int:
    now = datetime.datetime.time(datetime.datetime.now())
    for time_slot in time_slot_info:
        if (now >= time_slot["start_time"]):
            print(time_slot["start_time"])
            return time_slot["area"]


def detect_cat(image)->bool:
    # red color boundaries (R,B and G)
    # for night
    # lower = [20, 20, 20]
    # upper = [46, 40, 40]

    # for day
    # lower = [10, 10, 10]
    # upper = [30, 30, 30]

    # for yard night
    lower = [15, 20, 25]
    upper = [30, 45, 50]

    # create NumPy arrays from the boundaries
    lower = np.array(lower, dtype="uint8")
    upper = np.array(upper, dtype="uint8")

    # find the colors within the specified boundaries and apply
    # the mask
    mask = cv2.inRange(image, lower, upper)
    output = cv2.bitwise_and(image, image, mask=mask)

    _, thresh = cv2.threshold(mask, 40, 255, 0)

    # retrieve only external contour and keep all points
    _, contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

    if len(contours) != 0:
        # draw in blue the contours that were founded
        cv2.drawContours(output, contours, -1, 255, 3)

        #find the biggest area
        max_area_contour = max(contours, key=cv2.contourArea)

        print(cv2.contourArea(max_area_contour))

        x, y, w, h = cv2.boundingRect(max_area_contour)
        # draw the book contour (in green)
        cv2.rectangle(output, (x, y), (x+w, y+h), (0, 255, 0), 2)

            # show the images
        cv2.imshow("Result", np.hstack([image, output]))

        cv2.waitKey(0)

        if cv2.contourArea(max_area_contour) > get_area_threshold():
            return True
        else:
            return False


def main():
    # load the image
    image = cv2.imread("/home/kd/Dropbox/Projects/In-Progress/Cat_Alert_Station/15.jpg", 1)
    print(detect_cat(image))


if __name__ == '__main__':
    main()
