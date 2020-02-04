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
        y = w * x + b + random.random() * random.randint(-1, 100) + random.randint(-1, 20)

        x_list.append(x)
        y_list.append(y)

    return x_list, y_list


# define cost function
def cost_function(theta, x_array, y_array):
    diff = np.dot(x_array, theta) - y_array
    return (1 / (2 * len(y_array))) * np.dot(diff.transpose(), diff)


# get gradient value of the cost function
def gradient_function(theta, x_array, y_array):
    diff = np.dot(x_array, theta) - y_array  # sum of all predict value - true value
    return (1 / len(x_array)) * np.dot(x_array.transpose(), diff)


def train_func(x_array, y_array, lr):
    theta_save = []
    theta = np.array([1, 1]).reshape(2, 1)
    gradient = gradient_function(theta, x_array, y_array)
    times = 1

    # if direction gradient of cost func <= 0.1, then stop
    while not all(abs(gradient) <= 0.2):
        # calculate new coefficient of cost function
        theta = theta - lr * gradient
        gradient = gradient_function(theta, x_array, y_array)

        if times % 50 == 0:
            theta_save.append([theta.copy(), times])

        times += 1
    return theta, theta_save


def main():
    # generate train data
    x_list_init, y_list_init = gen_sample_data()
    x_list = np.array(x_list_init).reshape(len(x_list_init), 1)
    y_list = np.array(y_list_init).reshape(len(y_list_init), 1)

    # Generate a vector of m rows and 1 column, which is add_column_with_1, which is all 1
    add_column_with_1 = np.ones((len(x_list), 1))
    x_list = np.hstack((add_column_with_1, x_list))

    # set init learn rate
    learn_rate = 0.001382
    trained_theta, theta_saved = train_func(x_list, y_list, learn_rate)
    print("trained_theta:\n", trained_theta)
    x = np.arange(0, len(x_list_init), 0.2)

    for theta in theta_saved:
        # calculate cost value
        cost_value = cost_function(np.array(theta[0]).reshape(2, 1), x_list, y_list)[0][0]
        print("theta:", theta[0][0], theta[0][1], "times: ", theta[1], "cost: ", cost_value)

        # update plot
        plt.clf()
        y = theta[0][0] + theta[0][1] * x
        plt.scatter(x_list_init, y_list_init, s=30, c="red", marker="s")
        plt.xlabel("X")
        plt.ylabel("Y")
        plt.plot(x, y, label='fitting equation')
        plt.grid()
        plt.legend()
        plt.title('iteration times : %s' % theta[1])
        plt.pause(0.001)

    plt.show()


if __name__ == "__main__":
    main()
