# 函数使用说明

## 1. image_crop

```python
import cv2
from matplotlib import pyplot as plt
img_ori = cv2.imread('lenna.jpg', 1)
img_ori = image_crop(img_ori, 100, 100, 500, 500)
plt.imshow(cv2.cvtColor(img_ori, cv2.COLOR_BGR2RGB))
plt.show()
```

## 2. image_color_shift

```python
img_ori = cv2.imread('lenna.jpg', 1)
img_random_color = color_shift(img_ori, 0, 0, 50)
plt.imshow(cv2.cvtColor(img_random_color, cv2.COLOR_BGR2RGB))
plt.show()
```