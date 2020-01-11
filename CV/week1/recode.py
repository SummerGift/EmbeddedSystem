import cv2
import random
import numpy as np
from matplotlib import pyplot as plt

# 读取操作
img_ori = cv2.imread('lenna.jpg', 1)  # 彩色图片
img_gray = cv2.imread('lenna.jpg', 0)  # 黑白图片
print(img_ori.shape)

# 打印操作，默认通道为 BGR
# cv2.imshow("lenna", img_ori)
# key = cv2.waitKey()
# if key == 27:
#     cv2.destroyAllWindows()

# matplotlib 打印操作，默认通道为 RGB
plt.imshow(img_ori)
plt.show()

plt.imshow(img_gray)
plt.show()

plt.imshow(cv2.cvtColor(img_ori, cv2.COLOR_BGR2RGB))  # 将通道转换为 matplotlib 的默认格式 RGB
plt.show()

# 对比通道转换前后的效果图
plt.subplot(121)
plt.imshow(img_ori)
plt.subplot(122)
plt.imshow(cv2.cvtColor(img_ori, cv2.COLOR_BGR2RGB))
plt.show()

# 编写一个自己使用的 show 函数，自动进行通道转换和大小调整
def my_show(img, size=(4, 4)):
    plt.figure(figsize=size)
    plt.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    plt.show()

# 图像裁剪 image crop
img_crop = img_ori[0:500, 100:500]
my_show(img_crop)

# 打印黑白图片
plt.figure(figsize=(2, 2))
plt.imshow(img_gray, cmap='gray')
plt.show()

# 对不同通道进行操作
B, G, R = cv2.split(img_ori)
# cv2.imshow('B', B)
# cv2.imshow('G', G)
# cv2.imshow('R', R)
# key = cv2.waitKey(0)
# if cv2.waitKey(1) & 0xFF == ord('q'):
#     cv2.destroyAllWindows()

plt.figure(figsize=(5, 5))
plt.subplot(131)
plt.imshow(B, cmap='gray')
plt.subplot(132)
plt.imshow(G, cmap='gray')
plt.subplot(133)
plt.imshow(R, cmap='gray')
plt.show()

# 图像的存储形式
print(img_ori)
print(img_ori.dtype)  # 打印图片的数据类型，这里是 uint8
print(img_ori.shape)  # 打印图片的长宽

# 图像的灰阶平移
def img_cooler(img, b_increase, r_decrease):
    B, G, R = cv2.split(img)
    b_lim = 255 - b_increase
    B[B > b_lim] = 255
    B[B <= b_lim] = (b_increase + B[B <= b_lim]).astype(img.dtype)
    r_lim = r_decrease
    R[R < r_lim] = 0
    R[R >= r_lim] = (R[R >= r_lim] - r_decrease).astype(img.dtype)
    return cv2.merge((B, G, R))

img_cool = img_cooler(img_ori, 50, 0)
my_show(img_cool)

img_dark = cv2.imread('dark.jpg')
my_show(img_dark, size=(6, 6))

# 伽马校正 gamma correction
def adjust_gamma(image, gamma=1.0):
    invGamma = 1.0 / gamma
    table = []
    for i in range(256):
        table.append(((i / 255.0) ** invGamma) * 255)
    table = np.array(table).astype("uint8")
    return cv2.LUT(img_dark, table)


img_brighter = adjust_gamma(img_dark, 1.5)     # 设置值大于 1，整体亮度变亮
my_show(img_brighter, size=(6, 6))

# img_brighter = adjust_gamma(img_dark, 0.5)   # 设置值小于 1，整体亮度变暗
# my_show(img_brighter, size=(6, 6))

# 直方图操作 histogram
plt.subplot(121)
plt.hist(img_dark.flatten(), 256, [0, 256], color='b')
plt.subplot(122)
plt.hist(img_brighter.flatten(), 256, [0, 256], color='r')
plt.show()

img_yuv = cv2.cvtColor(img_brighter, cv2.COLOR_BGR2YUV)
# equalize the histogram of the Y channel
img_yuv[:, :, 0] = cv2.equalizeHist(img_yuv[:, :, 0])  # only for 1 channel
# convert the YUV image back to RGB format
img_output = cv2.cvtColor(img_yuv, cv2.COLOR_YUV2BGR)  # y: luminance(明亮度), u&v: 色度饱和度

my_show(img_output, size=(6, 6))

plt.subplot(131)
plt.hist(img_dark.flatten(), 256, [0, 256], color='b')
plt.subplot(132)
plt.hist(img_brighter.flatten(), 256, [0, 256], color='r')
plt.subplot(133)
plt.hist(img_output.flatten(), 256, [0, 256], color='g')
plt.show()

# 相似变换 rotation
M = cv2.getRotationMatrix2D((img_ori.shape[1] / 2, img_ori.shape[0] / 2), 66, 0.8)  # center, angle, scale
img_rotate = cv2.warpAffine(img_ori, M, (img_ori.shape[1], img_ori.shape[0]))
my_show(img_rotate)

# 仿射变换 Affine Transform
rows, cols, ch = img_ori.shape
pts1 = np.float32([[0, 0], [cols - 1, 0], [0, rows - 1]])
pts2 = np.float32([[cols * 0.2, rows * 0.1], [cols * 0.7, rows * 0.2], [cols * 0.1, rows * 0.9]])

M = cv2.getAffineTransform(pts1, pts2)
dst = cv2.warpAffine(img_ori, M, (cols, rows))
my_show(dst)

# 投影变换 perspective transform
import random

def random_warp(img, row, col):
    height, width, channels = img.shape

    # warp:
    random_margin = 60
    x1 = random.randint(-random_margin, random_margin)
    y1 = random.randint(-random_margin, random_margin)
    x2 = random.randint(width - random_margin - 1, width - 1)
    y2 = random.randint(-random_margin, random_margin)
    x3 = random.randint(width - random_margin - 1, width - 1)
    y3 = random.randint(height - random_margin - 1, height - 1)
    x4 = random.randint(-random_margin, random_margin)
    y4 = random.randint(height - random_margin - 1, height - 1)

    dx1 = random.randint(-random_margin, random_margin)
    dy1 = random.randint(-random_margin, random_margin)
    dx2 = random.randint(width - random_margin - 1, width - 1)
    dy2 = random.randint(-random_margin, random_margin)
    dx3 = random.randint(width - random_margin - 1, width - 1)
    dy3 = random.randint(height - random_margin - 1, height - 1)
    dx4 = random.randint(-random_margin, random_margin)
    dy4 = random.randint(height - random_margin - 1, height - 1)

    pts1 = np.float32([[x1, y1], [x2, y2], [x3, y3], [x4, y4]])
    pts2 = np.float32([[dx1, dy1], [dx2, dy2], [dx3, dy3], [dx4, dy4]])
    M_warp = cv2.getPerspectiveTransform(pts1, pts2)
    img_warp = cv2.warpPerspective(img, M_warp, (width, height))
    return M_warp, img_warp

M_warp, img_warp = random_warp(img_ori, img_ori.shape[0], img_ori.shape[1])
print(M_warp)
my_show(img_warp)

