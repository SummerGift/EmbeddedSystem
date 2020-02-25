import cv2
import numpy as np
from matplotlib import pyplot as plt
#
# img_cat = cv2.imread('cat.jpg', 1)
# gray = cv2.cvtColor(img_cat, cv2.COLOR_BGR2GRAY)
# # cornerHarris函数图像格式为 float32 ，因此需要将图像转换 float32 类型
# # plt.imshow(img)
# # plt.show()
#
# gray = np.float32(gray)
# # cornerHarris参数：
# # src - 数据类型为 float32 的输入图像。
# # blockSize - 角点检测中要考虑的领域大小。
# # ksize - Sobel 求导中使用的窗口大小
# # k - Harris 角点检测方程中的自由参数,取值参数为 [0,04,0.06].
# dst = cv2.cornerHarris(src=gray, blockSize=5, ksize=23, k=0.04)
# # 变量a的阈值为0.01 * dst.max()，如果dst的图像值大于阈值，那么该图像的像素点设为True，否则为False
# # 将图片每个像素点根据变量a的True和False进行赋值处理，赋值处理是将图像角点勾画出来
# a = dst>0.01 * dst.max()
# img_cat[a] = [0, 0, 255]
# # 显示图像
#
# img = cv2.cvtColor(img_cat, cv2.COLOR_BGR2RGB)
# plt.subplot(122)
# plt.imshow(img)
# plt.show()

import cv2
# 读取图片并灰度处理
imgpath = 'cat.jpg'
img = cv2.imread(imgpath)
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
# 创建SIFT对象
sift = cv2.xfeatures2d.SIFT_create()
# 将图片进行SURF计算，并找出角点keypoints，keypoints是检测关键点
# descriptor是描述符，这是图像一种表示方式，可以比较两个图像的关键点描述符，可作为特征匹配的一种方法。
keypoints, descriptor = sift.detectAndCompute(gray, None)

# cv2.drawKeypoints() 函数主要包含五个参数：
# image: 原始图片
# keypoints：从原图中获得的关键点，这也是画图时所用到的数据
# outputimage：输出
# color：颜色设置，通过修改（b,g,r）的值,更改画笔的颜色，b=蓝色，g=绿色，r=红色。
# flags：绘图功能的标识设置，标识如下：
# cv2.DRAW_MATCHES_FLAGS_DEFAULT  默认值
# cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS
# cv2.DRAW_MATCHES_FLAGS_DRAW_OVER_OUTIMG
# cv2.DRAW_MATCHES_FLAGS_NOT_DRAW_SINGLE_POINTS
img = cv2.drawKeypoints(image=img, outImage=img, keypoints = keypoints, flags=cv2.DRAW_MATCHES_FLAGS_DEFAULT, color = (51, 163, 236))
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
plt.imshow(img)
plt.show()


