import numpy as np
from numpy.linalg import cholesky
import matplotlib.pyplot as plt


# Make dataset

# ground truth label: 0 or 1
# predict probs: (0, 1)
# logistic loss

def gen_sample_data():
    sampleNo = 1000
    mu = np.array([[1, 5]])
    sigma = np.array([[2, 0], [0, 3]])
    R = cholesky(sigma)
    s = np.dot(np.random.randn(sampleNo, 2), R) + mu
    x1 = np.hstack((s, np.ones((sampleNo, 1))))
    # plt.plot(s[:, 0], s[:, 1], "+")

    mu = np.array([[6, 0]])
    sigma = np.array([[2, 1], [1, 2]])
    R = cholesky(sigma)
    s = np.dot(np.random.randn(sampleNo, 2), R) + mu
    x2 = np.hstack((s, np.zeros((sampleNo, 1))))
    # plt.plot(s[:, 0], s[:, 1], "x")
    # plt.show()

    X = np.vstack((x1, x2))
    return X


# inference from data to probs
def sigmoid(w1, w2, b, x):
    pred_y = 1 / (1 + np.exp(-(w1 * x[:, 0] +
                               w2 * x[:, 1] + b)))
    return pred_y


# cost function
def eval_loss(w1, w2, b, x, y):
    # 根据逻辑回归 loss 值计算公式计算 loss 值
    loss = -(y * np.log(sigmoid(w1, w2, b, x)) + \
             (1 - y) * np.log(1 - sigmoid(w1, w2, b, x)))
    return np.mean(loss)


# single sample's gradient
def gradient(pred_y, y, x):
    diff = pred_y - y
    dw1 = diff * x[:, 0]
    dw2 = diff * x[:, 1]
    db = diff
    return dw1, dw2, db


# update w,b
def cal_step_gradient(batch_x, batch_y, w1, w2, b, lr):
    pred_y = sigmoid(w1, w2, b, batch_x)
    dw1, dw2, db = gradient(pred_y, batch_y, batch_x)
    w1 -= lr * np.mean(dw1)
    w2 -= lr * np.mean(dw2)
    b -= lr * np.mean(db)
    return w1, w2, b


def train(x, batch_size, lr, max_iter):
    w1 = w2 = b = 0
    x_axe = np.linspace(np.min(x[:, 0]), np.max(x[:, 0]), 1000)
    plt.ion()
    fig, ax = plt.subplots()
    for i in range(max_iter):
        batch_idxs = np.random.choice(len(x), batch_size, False)
        batch_x = np.array([x[j][:2] for j in batch_idxs])
        batch_y = np.array([x[j][2] for j in batch_idxs])
        w1, w2, b = cal_step_gradient(batch_x, batch_y, w1, w2, b, lr)
        print(f"w1:{w1}, w2:{w2}, b:{b}")
        print(f"loss: {eval_loss(w1, w2, b, batch_x, batch_y)}")

        plt.xlim(np.min(x[:, 0]) * 1.1, np.max(x[:, 0]) * 1.1)
        plt.ylim(np.min(x[:, 1]) * 1.1, np.max(x[:, 1]) * 1.1)
        plt.scatter(x[:, 0], x[:, 1], c=x[:, 2])
        # y_axe*w2 + x_axe*w1 +b = 0
        # Construct line with predict params w1, w2, b
        y_axe = (-b - x_axe * w1) / w2
        plt.plot(x_axe, y_axe, linewidth=2)
        plt.title(f"LOGISTIC REGRESSION ITER: {i+1}")
        plt.pause(0.5)
        if i != max_iter - 1:
            ax.cla()
    plt.ioff()
    plt.show()

    return w1, w2, b


X = gen_sample_data()
train(X, 100, 0.1, 50)
