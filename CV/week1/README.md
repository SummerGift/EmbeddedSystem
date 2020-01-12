# API Function For Details

## Source Code

- [augmentation source code](augmentation.py)

## Examples

### 1. image crop

```python
import cv2
from matplotlib import pyplot as plt
from augmentation import *
img_ori = cv2.imread('lenna.jpg', 1)
img_ori = image_crop(img_ori, 100, 100, 500, 500)
plt.imshow(cv2.cvtColor(img_ori, cv2.COLOR_BGR2RGB))
plt.show()
```

### 2. image color shift

```python
import cv2
from matplotlib import pyplot as plt
from augmentation import *
img_ori = cv2.imread('lenna.jpg', 1)
img_random_color = color_shift(img_ori, 60, 0, 0)  # BGR, B channel plus 60
plt.imshow(cv2.cvtColor(img_random_color, cv2.COLOR_BGR2RGB))
plt.show()
```

### 3. image rotation

```python
import cv2
from matplotlib import pyplot as plt
from augmentation import *
img_ori = cv2.imread('lenna.jpg', 1)
img_random_color = rotation(img_ori, 66)
plt.imshow(cv2.cvtColor(img_random_color, cv2.COLOR_BGR2RGB))
plt.show()
```

### 4. image perspective transform

```python
import cv2
from matplotlib import pyplot as plt
from augmentation import *
img_ori = cv2.imread('lenna.jpg', 1)
src_pts1 = [[0, 0], [0, 150], [500, 0], [500, 500]]
des_pts2 = [[20, 20], [40, 180], [400, 25], [300, 460]]
img_random_color = perspective_transform(img_ori, src_pts1, des_pts2)
plt.imshow(cv2.cvtColor(img_random_color, cv2.COLOR_BGR2RGB))
plt.show()
```
