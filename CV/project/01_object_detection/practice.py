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

# import cv2
# # 读取图片并灰度处理
# imgpath = 'cat.jpg'
# img = cv2.imread(imgpath)
# gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
# # 创建SIFT对象
# sift = cv2.xfeatures2d.SIFT_create()
# # 将图片进行SURF计算，并找出角点keypoints，keypoints是检测关键点
# # descriptor是描述符，这是图像一种表示方式，可以比较两个图像的关键点描述符，可作为特征匹配的一种方法。
# keypoints, descriptor = sift.detectAndCompute(gray, None)
#
# # cv2.drawKeypoints() 函数主要包含五个参数：
# # image: 原始图片
# # keypoints：从原图中获得的关键点，这也是画图时所用到的数据
# # outputimage：输出
# # color：颜色设置，通过修改（b,g,r）的值,更改画笔的颜色，b=蓝色，g=绿色，r=红色。
# # flags：绘图功能的标识设置，标识如下：
# # cv2.DRAW_MATCHES_FLAGS_DEFAULT  默认值
# # cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS
# # cv2.DRAW_MATCHES_FLAGS_DRAW_OVER_OUTIMG
# # cv2.DRAW_MATCHES_FLAGS_NOT_DRAW_SINGLE_POINTS
# img = cv2.drawKeypoints(image=img, outImage=img, keypoints = keypoints, flags=cv2.DRAW_MATCHES_FLAGS_DEFAULT, color = (51, 163, 236))
# img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
# plt.imshow(img)
# plt.show()

# # 读取图片内容
# img1 = cv2.imread('cat_face.jpg', 0)
# img2 = cv2.imread('cat.jpg', 0)
#
# # 使用ORB特征检测器和描述符，计算关键点和描述符
# # orb = cv2.ORB_create()
# orb = cv2.xfeatures2d.SURF_create(float(4000))
# kp1, des1 = orb.detectAndCompute(img1, None)
# kp2, des2 = orb.detectAndCompute(img2, None)
#
# # 暴力匹配BFMatcher，遍历描述符，确定描述符是否匹配，然后计算匹配距离并排序
# # BFMatcher函数参数：
# # normType：NORM_L1, NORM_L2, NORM_HAMMING, NORM_HAMMING2。
# # NORM_L1和NORM_L2是SIFT和SURF描述符的优先选择，NORM_HAMMING和NORM_HAMMING2是用于ORB算法
# # bf = cv2.BFMatcher(normType=cv2.NORM_HAMMING, crossCheck=True)
# bf = cv2.BFMatcher(normType=cv2.NORM_L1, crossCheck=True)
# matches = bf.match(des1, des2)
# matches = sorted(matches, key=lambda x: x.distance)
# # matches是DMatch对象，具有以下属性：
# # DMatch.distance - 描述符之间的距离。 越低越好。
# # DMatch.trainIdx - 训练描述符中描述符的索引
# # DMatch.queryIdx - 查询描述符中描述符的索引
# # DMatch.imgIdx - 训练图像的索引。
#
# # 使用plt将两个图像的匹配结果显示出来
# img3 = cv2.drawMatches(img1=img1, keypoints1=kp1, img2=img2, keypoints2=kp2, matches1to2=matches, outImg=img2, flags=2)
# plt.imshow(img3), plt.show()


# import numpy as np
# import cv2
# from matplotlib import pyplot as plt
#
# # 读取图片内容
# img1 = cv2.imread('cat_face.jpg', 1)
# img2 = cv2.imread('cat.jpg', 1)
# img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2RGB)
# img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2RGB)
#
# # 使用 SURF 特征检测器和描述符，计算关键点和描述符
# orb = cv2.xfeatures2d.SURF_create(float(1000))
# # orb = cv2.xfeatures2d.SURF_create(float(4000))
# kp1, des1 = orb.detectAndCompute(img1, None)
# kp2, des2 = orb.detectAndCompute(img2, None)
#
# bf = cv2.BFMatcher(normType=cv2.NORM_L1, crossCheck=True)
#
# # knnMatch 函数参数k是返回符合匹配的个数，暴力匹配match只返回最佳匹配结果。
# matches = bf.knnMatch(des1, des2, k=1)
#
# # 使用plt将两个图像的第一个匹配结果显示出来
# # 若使用 knnMatch 进行匹配，则需要使用drawMatchesKnn函数将结果显示
# img3 = cv2.drawMatchesKnn(img1=img1, keypoints1=kp1, img2=img2, keypoints2=kp2, matches1to2=matches, outImg=img2,
#                           flags=2)
# plt.imshow(img3), plt.show()


# import numpy as np
# import cv2
# from matplotlib import pyplot as plt
#
# img1 = cv2.imread('cat_face.jpg', 1)
# img2 = cv2.imread('cat.jpg', 1)
# queryImage = cv2.cvtColor(img1, cv2.COLOR_BGR2RGB)
# trainingImage = cv2.cvtColor(img2, cv2.COLOR_BGR2RGB)
#
#
# # queryImage = cv2.imread('aa.jpg',0)
# # trainingImage = cv2.imread('bb.png',0)
#
# # 只使用SIFT 或 SURF 检测角点
# sift = cv2.xfeatures2d.SIFT_create()
# # sift = cv2.xfeatures2d.SURF_create(float(4000))
# kp1, des1 = sift.detectAndCompute(queryImage,None)
# kp2, des2 = sift.detectAndCompute(trainingImage,None)
#
# # 设置FLANN匹配器参数
# # algorithm设置可参考https://docs.opencv.org/3.1.0/dc/d8c/namespacecvflann.html
# indexParams = dict(algorithm=0, trees=5)
# searchParams = dict(checks=50)
# # 定义FLANN匹配器
# flann = cv2.FlannBasedMatcher(indexParams,searchParams)
# # 使用 KNN 算法实现匹配
# matches = flann.knnMatch(des1,des2,k=2)
#
# # 根据matches生成相同长度的matchesMask列表，列表元素为[0,0]
# matchesMask = [[0,0] for i in range(len(matches))]
#
# # 去除错误匹配
# for i,(m,n) in enumerate(matches):
#     if m.distance < 0.7*n.distance:
#         matchesMask[i] = [1,0]
#
# # 将图像显示
# # matchColor是两图的匹配连接线，连接线与matchesMask相关
# # singlePointColor是勾画关键点
# drawParams = dict(matchColor = (0,255,0),
#                    singlePointColor = (255,0,0),
#                    matchesMask = matchesMask,
#                    flags = 0)
# resultImage = cv2.drawMatchesKnn(queryImage,kp1,trainingImage,kp2,matches,None,**drawParams)
# plt.imshow(resultImage,),plt.show()

# 基于FLANN的匹配器(FLANN based Matcher)定位图片
import numpy as np
import cv2
from matplotlib import pyplot as plt

MIN_MATCH_COUNT = 10  # 设置最低特征点匹配数量为10
template1 = cv2.imread('cat_face.jpg', 0)  # queryImage
# target = cv2.imread('cat.jpg', 0)  # trainImage

img1 = cv2.imread('cat_face.jpg', 1)
img2 = cv2.imread('cat.jpg', 1)
template = cv2.cvtColor(img1, cv2.COLOR_BGR2RGB)
target = cv2.cvtColor(img2, cv2.COLOR_BGR2RGB)


# Initiate SIFT detector创建sift检测器
sift = cv2.xfeatures2d.SIFT_create()
# find the keypoints and descriptors with SIFT
kp1, des1 = sift.detectAndCompute(template, None)
kp2, des2 = sift.detectAndCompute(target, None)
# 创建设置FLANN匹配
FLANN_INDEX_KDTREE = 0
index_params = dict(algorithm=FLANN_INDEX_KDTREE, trees=5)
search_params = dict(checks=50)
flann = cv2.FlannBasedMatcher(index_params, search_params)
matches = flann.knnMatch(des1, des2, k=2)
# store all the good matches as per Lowe's ratio test.
good = []
# 舍弃大于0.7的匹配
for m, n in matches:
    if m.distance < 0.7 * n.distance:
        good.append(m)
if len(good) > MIN_MATCH_COUNT:
    # 获取关键点的坐标
    src_pts = np.float32([kp1[m.queryIdx].pt for m in good]).reshape(-1, 1, 2)
    dst_pts = np.float32([kp2[m.trainIdx].pt for m in good]).reshape(-1, 1, 2)
    # 计算变换矩阵和MASK
    M, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 5.0)
    matchesMask = mask.ravel().tolist()
    h, w = template1.shape
    # 使用得到的变换矩阵对原图像的四个角进行变换，获得在目标图像上对应的坐标
    pts = np.float32([[0, 0], [0, h - 1], [w - 1, h - 1], [w - 1, 0]]).reshape(-1, 1, 2)
    dst = cv2.perspectiveTransform(pts, M)
    cv2.polylines(target, [np.int32(dst)], True, 0, 2, cv2.LINE_AA)
else:
    print("Not enough matches are found - %d/%d" % (len(good), MIN_MATCH_COUNT))
    matchesMask = None
draw_params = dict(matchColor=(0, 255, 0),
                   singlePointColor=None,
                   matchesMask=matchesMask,
                   flags=2)
result = cv2.drawMatches(template, kp1, target, kp2, good, None, **draw_params)
# plt.imshow(result, 'gray')
plt.imshow(result)
plt.show()
