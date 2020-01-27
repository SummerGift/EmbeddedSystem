# 图像基础处理算法

## 基本概念

### 什么是灰度图

灰度可以认为是亮度，简单说就是色彩的深浅程度。

灰度就是没有色彩，RGB色彩分量全部相等（可以使用画图工具来做测试，如果 RGB 三个通道的值都相等，那么最终的颜色就是从纯黑到白之间不同等级的灰色）。如果一个二值灰度图像，它的像素值只能为 0 或 1，我们说它的灰度级为2.用个例子来说吧：一个256级灰度图你，　　　

- RGB（100，100，100）就代表灰度为100，RGB（50，50，50）就代表灰度为50。

灰度是指黑白图像中的颜色深度，范围一般0-255，白色为255，黑色为0，故黑白图片也称为灰度图像。
　　若是彩色图像的灰度其实是在转化为黑白图像后的像素值（是一种广义的提法），转化的方法看应用领域而定，一般按加权的方法转换，R,G,B的一般比例为3： 6：1。
任何颜色都有红、绿、蓝三原色组成，假如原来某点的颜色为RGB（R,G,B），那么，我们可以通过下面几种方法，将其转换为灰度：
```
　　1.浮点算法：Gray = R*0.3 + G*0.59 + B*0.11
　　2.整数方法：Gray = (R*30+G*59+B*11)/100
　　3.移位方法：Gray =（R*28+G*151+B*77）>> 8
　　4.平均值法：Gray = (R+G+B)/3
　　5.仅取绿色：Gray = G
```

通过以上任何一种方法求得 Gray 后，将原来的 RGB（r,g,b) 中的 r,g,b 统一用 Gray 替换，形成新的颜色 RGB(Gray,Gray,Gray)，用它替换原来的 RGB（r,g,b)  就是灰度图了。

## 卷积


## 图像滤波

图像滤波既可以在实域进行，也可以在频域进行。图像滤波可以更改或者增强图像。通过滤波，可以强调一些特征或者去除图像中一些不需要的部分。滤波是一个邻域操作算子，利用给定像素周围的像素的值确定次像素的最终输出值。

$$
O(i,j)=∑_{m,n}I(i+m,j+n)∗K(m,n)
$$

其中 K 为滤波器，在很多文献中也成为核（kernel）。常见的应用包括去噪、图像增强、检测边缘、检测角点、模板匹配等。

###  图像锐化与边缘检测应用

图像锐化，求边缘等是常见的图像滤波应用。这类滤波器常常使用一节或者二阶差分（或微分，对于数字图像而言，其为离散信号，长用差分代替导数）核算子对图像进行滤波。一节差分常用于求取图像边缘。二阶差分常用于图像增强。常用的这类算子包括：

#### Sobel

Sobel operator：Sobel 算子通过计算水平和垂直方向上的一节差分来进行计算。在 OpenCV 函数中，可通过使用 [Sobel](http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html?highlight=sobel#sobel) 函数进行计算。

#### Laplacian

 Laplacian operator：Laplacian 算子通过计算二阶差分（微分）来进行计算。在 OpenCV 函数中，可通过使用 [Laplacian](http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html?highlight=laplacian#laplacian) 函数进行计算。

### 图像平滑应用

用于平滑图像的常见滤波算子包括：

#### 均值滤波

用其像素点周围像素的平均值代替原像素值，在滤除噪声的同时也会滤掉图像的边缘信息。在 OpenCV 中，可以使用 `boxFilter` 和 blur 函数进行均值滤波，均值滤波的核为：

![\frac{1}{ksize.width{\cdot}ksize.height}\begin{bmatrix} {1}&{1}&{\cdots}&{1}\\ {1}&{1}&{\cdots}&{1}\\ {\vdots}&{\vdots}&{\ddots}&{\vdots}\\ {1}&{1}&{\cdots}&{1}\\ \end{bmatrix}](http://latex.codecogs.com/gif.latex?%5Cfrac%7B1%7D%7Bksize.width%7B%5Ccdot%7Dksize.height%7D%5Cbegin%7Bbmatrix%7D%20%7B1%7D%26%7B1%7D%26%7B%5Ccdots%7D%26%7B1%7D%5C%5C%20%7B1%7D%26%7B1%7D%26%7B%5Ccdots%7D%26%7B1%7D%5C%5C%20%7B%5Cvdots%7D%26%7B%5Cvdots%7D%26%7B%5Cddots%7D%26%7B%5Cvdots%7D%5C%5C%20%7B1%7D%26%7B1%7D%26%7B%5Ccdots%7D%26%7B1%7D%5C%5C%20%5Cend%7Bbmatrix%7D)

#### 中值滤波

中值滤波用测试像素周围邻域像素集中的中值代替原像素。中值滤波去除椒盐噪声和斑块噪声时，效果非常明显。在 OpenCV 中可以用函数 `medianBlur` 进行操作。

#### 高斯滤波

高斯滤波为最常用的滤波器，具有可分离性质，可以把二维高斯运算转换为一维高斯运算，其本质上为一个低通滤波器。在OpenCV中可通过函数 [GaussianBlur](http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html?highlight=gaussianblur#gaussianblur) 进行操作。

模糊的算法有很多种，其中一种叫做高斯模糊（Gaussian Blur）。它将正态分布（又名高斯分布）用于图像处理。高斯模糊本质是一种数据平滑技术（data smoothing），适用于多个场合，图像处理提供了一个直观的应用实例可参考 [《高斯模糊（高斯滤波）的原理与算法》](https://blog.csdn.net/nima1994/article/details/79776802)。

- 均值滤波是简单的取平均值，模板系数都是 1。而图像上的像素实际上是坐标离散但是值却连续的，因为与靠近点的关系越密切，越远离的点关系越疏远。因此，加权平均更合理，举例越近的点权重越大，距离越远的点权重越小

- 既然是依据距离来加权平均，那么很容易想到高斯函数 $$f(x) = \frac{1}{\sigma\sqrt{2\pi }}{e}^{\frac{-(x-\mu )^{2}}{2\sigma^2}}$$，从高斯函数来看，离原点距离越近，得到的权重越高，越远离原点，得到的权重越小

- 一维的高斯函数，当中心点为原点时，x 的均值 μ = 0， 此时 $$f(x) = \frac{1}{\sigma\sqrt{2\pi }}{e}^{\frac{-x^{2}}{2\sigma^2}}$$
- 由于图像是二维矩阵，则采用二维高斯函数 $$f(x,y) = \frac{1}{{2\pi }\sigma^2}{e}^{\frac{-(x^2+y^2 )}{2\sigma^2}}$$，有了这个函数就可以计算滤波模板中各个点的权重了

##### 生成高斯核

通过 cv2 中的 `getGaussianKernel` 函数可以根据指定参数规模和方差生成高斯卷积核：

```
k = cv2.getGaussianKernel(3,1.5)
```

通过该函数生成的一维卷积核 k 已经进行过归一化处理。

- 规模为`3*3`，方差为 1.5 的一维卷积核  k 为：

```
[[0.30780133]
 [0.38439734]
 [0.30780133]]
```

- 通过 k*(k.T) 运算可以获得二维卷积核

```
[[0.09474166 0.11831801 0.09474166]
 [0.11831801 0.14776132 0.11831801]
 [0.09474166 0.11831801 0.09474166]]
```

##### 高斯滤波加速

对图像进行高斯滤波而言，假设滤波器半径为 r，我们常用的高斯模板则对于图像每个像素的算法复杂度是 O(r^2)。高斯滤波器的 kernel 是可分离的(separable)，也就是说，可以将 2D 的高斯 kernel 分解为两个 1D 的 kernel，先沿 x 方向对图像进行 1D 高斯 kernel 的卷积，然后沿 y 方向对图像进行 1D 的高斯 kernel 卷积，最后的结果和使用一个 2D 高斯 kernel 对图像卷积效果是一样的。这样一来，针对每个像素，滤波器的算法复杂度降为 O(r)。

### 双边滤波

双边滤波在平滑图像时能够很好的保留边缘特性，但是其运算速度比较慢。在 OpenCV 中，可以使用函数 [bilateralFilter](http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html?highlight=bilateralfilter#bilateralfilter) 进行操作。

### 自定义滤波器

除了上面列举的较为经典的滤波器（或者说是核算子）外，在 OpenCV 中也可以自己定义自己的滤波器，然后使用 [filter2D](http://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html?highlight=filter2d#filter2d) 函数进行运算。

## 特征描述算法

### What is a good feature point?
#### Harris Corner
- Very informational (Harris Corner Detector)
- Rotation/Brightness resistance (Harris Corner Detector)
- Scale resistance (Harris Corner Detector)

### What is the form of a feature point?
- Physical in location
- Abstract in formation (usually a vector) -> Feature Descriptor

### How to get a feature point/descriptor?
#### SIFT

SIFT，即尺度不变特征变换（Scale-invariant feature transform，SIFT），是用于[图像处理](https://baike.baidu.com/item/%E5%9B%BE%E5%83%8F%E5%A4%84%E7%90%86/294902)领域的一种描述。这种描述具有尺度不变性，可在图像中检测出关键点，是一种局部特征描述子。

SIFT 特征是基于物体上的一些局部外观的兴趣点而与影像的大小和旋转无关。对于光线、噪声、微视角改变的容忍度也相当高。基于这些特性，它们是高度显著而且相对容易撷取，在母数庞大的特征数据库中，很容易辨识物体而且鲜有误认。使用 SIFT 特征描述对于部分物体遮蔽的侦测率也相当高，甚至只需要 3 个以上的 SIFT 物体特征就足以计算出位置与方位。在现今的电脑硬件速度下和小型的特征数据库条件下，辨识速度可接近即时运算。SIFT 特征的信息量大，适合在海量数据库中快速准确匹配。

1. Generate Scale-space: DoG (Difference of Gaussian)
2. Scale-space Extrema Detection
3. Accurate Keypoint Localization
4. Eliminating Edge Responses
5. Orientation Assignment
6. Keypoint Descriptor

#### HoG


