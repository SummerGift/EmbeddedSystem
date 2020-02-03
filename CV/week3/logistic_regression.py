import numpy as np
import matplotlib.pyplot as plt


# def loaddata
def loaddata(filename):
    file = open(filename)
    x = []
    y = []
    for line in file.readlines():
        line = line.strip().split()
        x.append([1, float(line[0]), float(line[1])])
        y.append(float(line[-1]))
    xmat = np.mat(x)
    ymat = np.mat(y).T
    file.close()
    return xmat, ymat


# w calc
def w_calc(xmat, ymat, alpha=0.001, maxIter=10001):
    # W init
    W = np.mat(np.random.randn(3, 1))
    w_save = []
    # W update
    for i in range(maxIter):
        H = 1 / (1 + np.exp(-xmat * W))
        dw = xmat.T * (H - ymat)  # dw:(3,1)
        W -= alpha * dw
        if i % 100 == 0:
            w_save.append([W.copy(), i])
    return W, w_save


def main():
    xmat, ymat = loaddata('lr_data.txt')
    print('xmat:', xmat, xmat.shape)
    print('ymat:', ymat, ymat.shape)
    W, w_save = w_calc(xmat, ymat, 0.001, 10001)  # w save
    print('W:', W)

    # show
    for wi in w_save:
        plt.clf()
        w0 = wi[0][0, 0]
        w1 = wi[0][1, 0]
        w2 = wi[0][2, 0]
        plotx1 = np.arange(2, 6, 0.01)
        plotx2 = -w0 / w2 - w1 / w2 * plotx1
        plt.plot(plotx1, plotx2, c='r', label='decision boundary')

        plt.scatter(xmat[:, 1][ymat == 0].A, xmat[:, 2][ymat == 0].A, marker='^', s=150, label='label=0')
        plt.scatter(xmat[:, 1][ymat == 1].A, xmat[:, 2][ymat == 1].A, s=150, label='label=1')
        plt.grid()
        plt.legend()
        plt.title('iter:%s' % np.str(wi[1]))
        plt.pause(0.001)
    plt.show()


if __name__ == "__main__":
    main()
