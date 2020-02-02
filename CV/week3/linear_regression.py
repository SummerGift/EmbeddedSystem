import numpy as np
import matplotlib.pyplot as plt
import random




# def gen_sample_data():
#     w = random.randint(0, 10) + random.random()
#     b = random.randint(0, 5) + random.random()
#
#     num_sample = 100
#     x_list = []
#     y_list = []
#     for i in range(num_sample):
#         x = random.randint(0, 100) * random.random()
#         y = w * x + b + random.random() * random.randint(-1, 100)
#
#         x_list.append(x)
#         y_list.append(y)
#
#     return x_list, y_list
#
#
# x_list, y_list = gen_sample_data()


# define cost function
def cost_function(theta, x_list, y_list):
    diff = np.dot(x_list, theta) - y_list
    return (1 / (2 * m)) * np.dot(diff.transpose(), diff)


# get gradient value of the cost function
def gradient_function(theta, X, Y):
    diff = np.dot(X, theta) - Y  # sum of all predict value - true value
    return (1 / m) * np.dot(X.transpose(), diff)


def train_func(X, Y, learn_rate):

    # print(len(X))
    # X0 = np.ones((len(X), 1))  # 生成一个m行1列的向量，也就是x0，全是1
    # X = np.hstack((X0, X))  # 按照列堆叠形成数组，其实就是样本数据
    # print(X)

    theta = np.array([1, 1]).reshape(2, 1)
    print("final_theta:", theta[0][0], theta[1][0])
    gradient = gradient_function(theta, X, Y)

    print("gradient:\n", gradient)

    while not all(abs(gradient) <= 1e-5):
        theta = theta - learn_rate * gradient
        gradient = gradient_function(theta, X, Y)
    return theta


def plot_result(X, Y, theta):
    ax = plt.subplot(121)
    ax.scatter(X, Y, s=30, c="red", marker="s")
    plt.xlabel("X")
    plt.ylabel("Y")
    x = np.arange(0, 21, 0.2)  # x的范围
    y = theta[0] + theta[1] * x
    ax.plot(x, y)
    plt.show()


# data set size
m = 20
# x的坐标以及对应的矩阵
X0 = np.ones((m, 1))  # 生成一个m行1列的向量，也就是x0，全是1
X1 = np.arange(1, m + 1).reshape(m, 1)  # 生成一个m行1列的向量，也就是x1，从1到m
x_list = np.hstack((X0, X1))  # 按照列堆叠形成数组，其实就是样本数据

# x_list = np.arange(1, m + 1).reshape(m, 1)
# 对应的y坐标
y_list = np.array([
    3, 4, 5, 5, 2, 4, 7, 8, 11, 8, 12,
    11, 13, 13, 16, 17, 18, 17, 19, 21
]).reshape(m, 1)

learn_rate = 0.01
trained_theta = train_func(x_list, y_list, learn_rate)
print("final_theta:", trained_theta[0][0], trained_theta[1][0])
print('cost function:', cost_function(trained_theta, x_list, y_list)[0][0])
plot_result(X1, y_list, trained_theta)
