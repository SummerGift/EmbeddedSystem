import numpy as np
import matplotlib.pyplot as plt


# get data from file
def load_data(filename):
    file = open(filename)
    x = []
    y = []
    for line in file.readlines():
        line = line.strip().split()
        x.append([1, float(line[0]), float(line[1])])
        y.append(float(line[-1]))
    x_array = np.mat(x)
    y_array = np.mat(y).T
    file.close()
    return x_array, y_array


# Verify the accuracy of the model
def validation_predictions(x_array, y_array, coefficient):
    right_count = 0
    right_flag = "false"
    pre_value = 0

    predict_values = 1 / (1 + np.exp(-x_array * coefficient))
    print("\nPrediction validation:")
    for i in range(len(y_array)):
        if predict_values[i] < 0.5:
            pre_value = 0
        else:
            pre_value = 1

        right_flag = "false"
        if pre_value == y_array[i]:
            right_count += 1
            right_flag = "right"

        print("instance {0} :".format(i), "pre_value:", pre_value, ", true value:", y_array[i].astype(int), right_flag)

    print('Prediction accuracy: {:.2f}%'.format(right_count / len(y_array) * 100))


# Train Logistic Regression Model
def model_train(x_array, y_array, alpha=0.001, max_iter=10001):
    # Initialization coefficient value
    coefficient = np.mat(np.random.randn(3, 1))
    w_save = []

    for i in range(max_iter):
        # calculate the gradient value of the current coefficient
        H = 1 / (1 + np.exp(-x_array * coefficient))
        dw = x_array.T * (H - y_array)

        # update coefficient
        coefficient -= alpha * dw

        # sampling records training process
        if i % 100 == 0:
            w_save.append([coefficient.copy(), i])

    return coefficient, w_save


def main():
    x_array, y_array = load_data('lr_data.txt')
    # print('x_array:', x_array, x_array.shape)
    # print('y_array:', y_array, y_array.shape)
    trained_w, w_save = model_train(x_array, y_array, 0.001, 10001)
    print('\ntrained_w:', trained_w[0], trained_w[1], trained_w[2])
    validation_predictions(x_array, y_array, trained_w)

    # show iteration process
    for wi in w_save:
        plt.clf()
        w0 = wi[0][0, 0]
        w1 = wi[0][1, 0]
        w2 = wi[0][2, 0]
        plot1 = np.arange(2, 6, 0.01)
        plot2 = -w0 / w2 - w1 / w2 * plot1
        plt.plot(plot1, plot2, c='r', label='classify boundary')
        plt.scatter(x_array[:, 1][y_array == 0].A, x_array[:, 2][y_array == 0].A, marker='^', s=150, label='label=0')
        plt.scatter(x_array[:, 1][y_array == 1].A, x_array[:, 2][y_array == 1].A, s=150, label='label=1')
        plt.grid()
        plt.legend()
        plt.title('iteration times : %s' % np.str(wi[1]))
        plt.pause(0.001)
    plt.show()




if __name__ == "__main__":
    main()
