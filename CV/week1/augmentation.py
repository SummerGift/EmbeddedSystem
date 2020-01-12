import cv2
import random
import numpy as np
from matplotlib import pyplot as plt


def image_crop(src, x1, y1, x2, y2):
    """
    Crop image from (x1, y1) to (x2, y2).

    Parameters
    ----------
    :param src: Input image in BGR format
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
    :param img: Input image in BGR format
    :param change2b: Adjustment value of blue channel
    :param change2g: Adjustment value of green channel
    :param change2r: Adjustment value of red channel
    """

    B, G, R = cv2.split(img)

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


def rotation(img, angle):
    """
    Rotate the input image.

    Parameters
    ----------
    :param img: Input image in BGR format
    :param angle: Rotation angle
    """

    M = cv2.getRotationMatrix2D((img.shape[1] / 2, img.shape[0] / 2), angle, 1)  # center, angle, scale
    return cv2.warpAffine(img, M, (img.shape[1], img.shape[0]))


def perspective_transform(img, src_pts, des_pts):
    """
    Perspective transform for image.

    Parameters
    ----------
    :param img: Input image in BGR format
    :param src_pts: Initial coordinates for image transform
    :param des_pts: End coordinates for image transform
    """

    height, width, channels = img.shape

    assert len(src_pts) == 4
    assert len(des_pts) == 4

    pts_from = np.float32(src_pts)
    pts_to = np.float32(des_pts)
    M_warp = cv2.getPerspectiveTransform(pts_from, pts_to)
    return cv2.warpPerspective(img, M_warp, (width, height))
