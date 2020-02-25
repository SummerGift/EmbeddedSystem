# Object detection project

## 实现图像特征检测与识别

### 实现源码

- [object detection source code](object_detection.py)

### 实现方法
1. 选择一个小物件，比如一个易拉罐，一本书；再选择一个大场景，场景中包含这个小物件
2. 分别对这两幅图（小物件/大场景）进行特征点检测
3. 找到小物件对大场景的特征点匹配，匹配处即我们检测到的小物件
4. 小物件在原图中的 Bounding Box【就是把小物件框出来的矩形；如果没有手动框出，也可以认为是小图的四个顶点坐标】，经过单应性变换，就是小物件在大场景图中的新边框。

### 实现效果

- ![object detection](cat_match.jpg)