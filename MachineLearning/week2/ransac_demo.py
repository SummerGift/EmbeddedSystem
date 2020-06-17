#  Ransac many noisy data

import numpy as np
import matplotlib.pyplot as plt
import random

SIZE = 50
ERROR = 50

x = np.linspace(0, 10, SIZE)
y = 3 * x + 10
random_x = [x[i] + random.uniform(-0.6, 0.2) for i in range(SIZE)]
random_y = [y[i] + random.uniform(-0.6, 0.2) for i in range(SIZE)]

# add some error points
for i in range(ERROR):
    random_x.append(random.uniform(0, 20))
    random_y.append(random.uniform(10, 40))

RANDOM_X = np.array(random_x)
RANDOM_Y = np.array(random_y)
# fig = plt.figure()
# ax1 = fig.add_subplot(1, 1, 1)
# ax1.scatter(RANDOM_X, RANDOM_Y)
# plt.show()

# use OLS to fit the model
# from sklearn.linear_model import LinearRegression
#
# data_X = RANDOM_X.reshape(-1, 1)
# data_Y = RANDOM_Y.reshape(-1, 1)
#
# reg = LinearRegression(fit_intercept=True)
# reg.fit(data_X, data_Y)
# slope = reg.coef_
# intercept = reg.intercept_
# PREDICT_Y = reg.predict(data_X)
#
# fig = plt.figure()
# ax1 = fig.add_subplot(1, 1, 1)
# ax1.scatter(RANDOM_X, RANDOM_Y)
# ax1.plot(RANDOM_X, PREDICT_Y, c='red')
# plt.show()

## RANSAC
# 1. 要得到一个直线模型，需要两个点唯一确定一个直线方程。所以第一步随机选择两个点
# 2. 通过这两个点，可以计算出这两个点所表示的模型方程 y=ax+b
# 3. 将所有的数据点套到这个模型中计算误差
# 4. 找到所有满足误差阈值的点
# 5. 重复前 4 步迭代过程，直到达到一定迭代次数后，选出那个被支持的最多的模型，作为问题的解

iterations = 100
tolerent_sigma = 1
thresh_size = 0.5

best_slope = -1
best_intercept = 0
pretotal = 0

plt.ion()
plt.figure()

for i in range(iterations):
    # 1. 每次迭代随机选取两个样本点
    sample_index = random.sample(range(SIZE + ERROR), 2)
    x_1 = RANDOM_X[sample_index[0]]
    x_2 = RANDOM_X[sample_index[1]]
    y_1 = RANDOM_Y[sample_index[0]]
    y_2 = RANDOM_Y[sample_index[1]]

    # 2. 根据随机选取的点来计算参数值
    slope = (y_2 - y_1) / (x_2 - x_1)
    intercept = y_1 - slope * x_1

    # 3. 计算负荷要求的内点个数
    total_inliers = 0
    for index in range(SIZE + ERROR):
        PREDICT_Y = slope * RANDOM_X[index] + intercept
        if abs(PREDICT_Y - RANDOM_Y[index]) < tolerent_sigma:
            total_inliers += 1

    if total_inliers > pretotal:
        pretotal = total_inliers
        best_slope = slope
        best_intercept = intercept

    # 4. 如果内点个数大于阈值则停止迭代
    if total_inliers > (SIZE + ERROR) * thresh_size:
        break

    plt.title(f"RANSAC in Linear Regression: Iter {i + 1}, Inliers {pretotal}")
    plt.scatter(RANDOM_X, RANDOM_Y)
    Y = best_slope * RANDOM_X + best_intercept
    plt.plot(RANDOM_X, Y,'black')
    plt.pause(0.2)
    plt.clf()
