import numpy as np
import matplotlib.pyplot as plt
import random


def gen_sample_data():
    w = random.randint(0, 10) + random.random()
    b = random.randint(0, 5) + random.random()

    num_sample = 100
    x_list = []
    y_list = []
    for i in range(num_sample):
        x = random.randint(0, 100) * random.random()
        y = w * x + b + random.random() * random.randint(-1, 100)

        x_list.append(x)
        y_list.append(y)

    return x_list, y_list

# define cost function
def cost_function(theta, x_list, y_list):
    diff = np.dot(x_list, theta) - y_list
    return (1 / (2 * len(y_list))) * np.dot(diff.transpose(), diff)


# get gradient value of the cost function
def gradient_function(theta, X, Y):
    diff = np.dot(X, theta) - Y  # sum of all predict value - true value
    m = len(X)
    return (1 / m) * np.dot(X.transpose(), diff)


def train_func(x_list, y_list, learn_rate):
    theta = np.array([1, 1]).reshape(2, 1)
    print("first_theta:", theta[0][0], theta[1][0])
    gradient = gradient_function(theta, x_list, y_list)
    print(gradient)

    while not all(abs(gradient) <= 1e-5):
        theta = theta - learn_rate * gradient
        gradient = gradient_function(theta, x_list, y_list)
    return theta


def plot_result(X, Y, theta):
    ax = plt.subplot(111)
    ax.scatter(X, Y, s=30, c="red", marker="s")
    plt.xlabel("X")
    plt.ylabel("Y")
    x = np.arange(0, 100, 0.2)
    y = theta[0][0] + theta[1][0] * x
    ax.plot(x, y)
    plt.show()


x_list_init, y_list_init = gen_sample_data()
x_list = np.array(x_list_init).reshape(len(x_list_init), 1)
y_list = np.array(y_list_init).reshape(len(y_list_init), 1)
X0 = np.ones((len(x_list), 1))    # 生成一个m行1列的向量，也就是x0，全是1
x_list = np.hstack((X0, x_list))  # 按照列堆叠形成数组，其实就是样本数据

learn_rate = 0.001
trained_theta = train_func(x_list, y_list, learn_rate)
print("final_theta:", trained_theta[0][0], trained_theta[1][0])
print('cost function:', cost_function(trained_theta, x_list, y_list)[0][0])
plot_result(x_list_init, y_list_init, trained_theta)
