import cv2
import numpy as np
import matplotlib.pyplot as plt


def MedianFilter(img, kernel=3, padding_way=None):
    """
    Do median filter to image.

    Parameters
    ----------
    :param src: Input image in BGR format
    :param kernel: Median Filter kernel size
    :param padding_way: set the way to padding
    """

    height, width = img.shape

    # get the edge of kernel
    edge = int((kernel - 1)/2)
    if height - 1 - edge <= edge or width - 1 - edge <= edge:
        print("Kernel size is to large.")
        return None

    if padding_way == "REPLICA":
        img = np.pad(img, ((edge, edge), (edge, edge)), 'edge')
    elif padding_way == "ZERO":
        img = np.pad(img, ((edge, edge), (edge, edge)), 'constant')

    height, width = img.shape
    img_medianBlur = np.zeros((height, width), dtype="uint8")

    for i in range(height):
        for j in range(width):
            if i <= edge - 1 or i >= height - 1 - edge or j <= edge - 1 or j >= height - edge - 1:
                continue
            else:
                img_medianBlur[i, j] = np.median(img[i - edge:i + edge + 1, j - edge:j + edge + 1])

    # delete useless data set
    new_crop = np.delete(img_medianBlur, list(range(0, 2)) + list(range(height - 3, height)), axis=1)
    img_medianBlur = np.delete(new_crop,  list(range(0, 2)) + list(range(width - 3, width)), axis=0)

    return img_medianBlur


init_array = cv2.imread("noisy_lenna.jpg", cv2.IMREAD_GRAYSCALE)
img_medianBlur = MedianFilter(init_array, kernel=13, padding_way="ZERO")

plt.figure(figsize=(7, 7), dpi=120)
plt.subplot(121)
plt.imshow(init_array, cmap='gray')
plt.subplot(122)
plt.imshow(img_medianBlur, cmap='gray')
