import cv2
import random
import numpy as np
from matplotlib import pyplot as plt


def image_crop(src, x1, y1, x2, y2):
    """
    Crop image from (x1, y1) to (x2, y2).

    Parameters
    ----------
    :param src: Image source from import in BGR format
    :param x1: Initial coordinates for image cropping
    :param y1: Initial coordinates for image cropping
    :param x2: End coordinates of image cropping
    :param y2: End coordinates of image cropping
    """
    return src[x1:x2, y1:y2]


def color_shift(img, change2b=0, change2g=0, change2r=0):
    """
    Shift the color of the input image.

    Parameters
    ----------
    :param img: image source
    :param change2b: Adjustment value of the channel
    :param change2r: Adjustment value of the channel
    :param change2g: Adjustment value of the channel
    """

    B, G, R = cv2.split(img)

    print(change2b)
    print(change2g)
    print(change2r)

    if change2b == 0:
        pass
    elif change2b > 0:
        lim = 255 - change2b
        B[B > lim] = 255
        B[B <= lim] = (change2b + B[B <= lim]).astype(img.dtype)
    elif change2b < 0:
        lim = 0 - change2b
        B[B < lim] = 0
        B[B >= lim] = (change2b + B[B >= lim]).astype(img.dtype)

    if change2g == 0:
        pass
    elif change2g > 0:
        lim = 255 - change2g
        G[G > lim] = 255
        G[G <= lim] = (change2g + G[G <= lim]).astype(img.dtype)
    elif change2g < 0:
        lim = 0 - change2g
        G[G < lim] = 0
        G[G >= lim] = (change2g + G[G >= lim]).astype(img.dtype)

    if change2r == 0:
        pass
    elif change2r > 0:
        lim = 255 - change2r
        R[R > lim] = 255
        R[R <= lim] = (change2r + R[R <= lim]).astype(img.dtype)
    elif change2r < 0:
        lim = 0 - change2r
        R[R < lim] = 0
        R[R >= lim] = (change2r + R[R >= lim]).astype(img.dtype)

    return cv2.merge((B, G, R))


def rotation():
    pass


def perspective_transform():
    pass
