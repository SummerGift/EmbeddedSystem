import cv2
import numpy as np
from matplotlib import pyplot as plt

img_cat = cv2.imread('cat.jpg', 1)
gray = cv2.cvtColor(img_cat, cv2.COLOR_BGR2GRAY)
plt.imshow(gray)
plt.show()
