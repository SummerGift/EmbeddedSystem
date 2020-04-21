import numpy as np
import matplotlib.pyplot as plt
import os
import copy


class GenerateTrainDataset:
    def __init__(self, folder_list):
        self.folder_list = folder_list

    def load_metadata(self):
        tmp_lines = list()
        for folder_name in self.folder_list:
            label_file = os.path.join(os.path.abspath('.'), "data", folder_name, "label.txt")
            with open(label_file) as f:
                lines = f.readlines()
            tmp_lines += [os.path.join(os.path.abspath('.'), "data", folder_name, line) for line in lines]

        print("tmp_lines", tmp_lines)

        res_lines = self.remove_invalid_image(tmp_lines)
        return res_lines

    @staticmethod
    def remove_invalid_image(lines):
        images = []
        for line in lines:
            line_ = line.split()
            name = line_[0]
            rect = list(map(int, list(map(float, line_[1:5]))))
            landm = list(map(float, line_[5:]))
            # 去掉坐标越界的人脸
            rect = np.array(rect)
            landm = np.array(landm)
            if (rect >= 0).all() == False or (landm >= 0).all() == False:
                continue
            if os.path.isfile(name):
                images.append(line)
        return images

    @staticmethod
    def change_data_format(lines):
        """process metadata to new format {‘图片地址’：[(边框1，关键点1),(边框2，关键点2)...]}"""
        truth = {}
        for line in lines:
            line = line.strip().split()
            name = line[0]
            if name not in truth:
                truth[name] = []
            rect = list(map(int, list(map(float, line[1:5]))))
            x = list(map(float, line[5::2]))
            y = list(map(float, line[6::2]))
            landmarks = list(zip(x, y))
            truth[name].append((rect, landmarks))
        return truth

    # 将显示框线的四个值转换为坐标点
    @staticmethod
    def rect_trans(rect):
        r_x = np.array([rect[0], rect[0], rect[2], rect[2], rect[0]])
        r_y = np.array([rect[1], rect[3], rect[3], rect[1], rect[1]])
        return r_x, r_y

    def key_show(self, key_name, data):
        """show specific picture"""
        img = plt.imread(key_name)
        fig = plt.figure(figsize=(10, 10))
        ax = fig.subplots()
        ax.axis('off')
        ax.imshow(img)
        for i in range(len(data[key_name])):
            rect = data[key_name][i][0]
            landmarks = np.array(data[key_name][i][1])
            r_x, r_y = self.rect_trans(rect)
            ax.plot(r_x, r_y, color='g', linewidth=2)
            ax.scatter(landmarks[:, 0], landmarks[:, 1], s=5, c='r')
        plt.show()

    def check_show(self, data):
        """show picture randomly"""
        names = []
        for key in data:
            if key not in names:
                names.append(key)
        index = np.random.randint(0, len(names))
        name = names[index]
        self.key_show(name, data)
        return name

    # 检验扩增效果
    def compare_show(self, data1, data2):
        names = []
        for key in data1:
            if key not in names:
                names.append(key)
        index = np.random.randint(0, len(names))
        name = names[index]
        self.key_show(name, data1)
        self.key_show(name, data2)

    # 将人脸框扩大（默认0.25倍）；保证人脸框不超过图像大小
    @staticmethod
    def expand_roi(rect, img_width, img_height, ratio=0.25):
        # 扩大框
        x1, y1, x2, y2 = rect[0], rect[1], rect[2], rect[3]
        width = x2 - x1 + 1
        height = y2 - y1 + 1
        padding_width = int(width * ratio)
        padding_height = int(height * ratio)
        x1 = x1 - padding_width
        y1 = y1 - padding_height
        x2 = x2 + padding_width
        y2 = y2 + padding_height
        # 保证不超过图像
        x1 = 0 if x1 < 0 else x1
        y1 = 0 if y1 < 0 else y1
        x2 = img_width - 1 if x2 >= img_width else x2
        y2 = img_height - 1 if y2 >= img_height else y2
        rect[0], rect[1], rect[2], rect[3] = x1, y1, x2, y2

    def expand_figure_rect(self, data):
        for key in data:
            img = plt.imread(key)
            img_h, img_w = img.shape[:2]
            value = data[key]
            for i in range(len(value)):
                self.expand_roi(value[i][0], img_w, img_h)
        return data

    # 数据类型转换
    @staticmethod
    def trans_value(key, value):
        rect = ''
        for r in value[0]:
            rect += ' ' + str(r)
        landmarks = ''
        for lms in value[1]:
            landmark = ''
            for lm in lms:
                landmark += ' ' + str(lm)
            landmarks += landmark
        line = key + rect + landmarks
        return line

    # 样本个数train：test=8:2，即train有1570个，test有392个
    @staticmethod
    def generate_train_test_data(self, data, rate=4):
        lines = []
        for key in data:
            values = data[key]
            for i in range(len(values)):
                line = self.trans_value(key, values[0])
                lines.append(line)
                values.remove(values[0])
        number = len(lines)
        train = lines[:int(number * (4 / 5))]
        test = lines[int(number * (4 / 5)):]
        return train, test, lines

    def load_data(self, path):
        """load valid dataset"""
        lines = []
        with open(path) as f:
            lines = f.readlines()
        data = self.change_data_format(lines)
        return data

    @staticmethod
    def data_key_show(key, data):
        # 读取原图像
        img = plt.imread(key)
        value = data[key]
        print("value", value)
        num = len(value)
        fig = plt.figure(figsize=(10, 10))
        axes = fig.subplots(nrows=1, ncols=num)
        for i in range(num):
            # 画出截图头像
            crop = value[i][0]
            crop_img = img[crop[1]:crop[3], crop[0]:crop[2]]
            if num == 1:
                ax = axes
            else:
                ax = axes[i]
            ax.imshow(crop_img)
            # 画出关键点
            landmarks = np.array(value[i][1])
            ax.scatter(landmarks[:, 0], landmarks[:, 1], s=5, c='r')
        plt.show()

    # 随机选取样本画图
    def data_show_face_rect(self, data):
        # 随机选取
        names = []
        for key in data:
            names.append(key)
        index = np.random.randint(0, len(names))
        name = names[index]
        print(name)
        self.data_key_show(name, data)

    # 人脸关键点坐标变更 landmarks -= np.array([roi x1,roi y1])
    # 将关键点的坐标改为相对于人脸框的坐标，也就是减去人脸框左上角的坐标
    @staticmethod
    def change_data_landmarks(data):
        delete_value1 = {}
        delete_value2 = {}
        delete_key = []
        for key in data:
            value = data[key]
            deletes1 = []
            deletes2 = []

            for i in range(len(value)):
                r = np.array([value[i][0][0], value[i][0][1]])
                w = value[i][0][2] - value[i][0][0]
                h = value[i][0][3] - value[i][0][1]

                for j in range(len(value[i][1])):
                    value[i][1][j] -= r

                    if value[i][1][j][0] < 0 or value[i][1][j][1] < 0:
                        deletes1.append(value[i])
                        break

                    if value[i][1][j][0] > w or value[i][1][j][1] > h:
                        deletes2.append(value[i])
                        break

            if len(deletes1) != 0:
                delete_value1[key] = []
                for delete in deletes1:
                    value.remove(delete)
                    delete_value1[key].append(delete)

            if len(deletes2) != 0:
                delete_value2[key] = []
                for delete in deletes2:
                    value.remove(delete)
                    delete_value2[key].append(delete)

            if len(value) == 0:
                delete_key.append(key)

        for key in delete_key:
            del data[key]

        return data, delete_value1, delete_value2

    @staticmethod
    def save_dataset(data, path):
        """save train/test.txt file to FS"""
        with open(path, "w") as f:
            for d in data:
                f.write(d + '\n')
        print('Save {0} successfully!'.format(path))


def main():
    folder_list = ['I', 'II']
    data_set = GenerateTrainDataset(folder_list)

    # 1. load metadata
    # meta_data = data_set.load_metadata()

    # 2. change metadata to new format
    # data = data_set.change_data_format(meta_data)

    # 3. expand face box
    # data_expand_rect = copy.deepcopy(data)
    # data_expand_rect = data_set.expand_figure_rect(data_expand_rect)
    # data_set.compare_show(data, data_expand_rect)

    # 4. change key point base on rect
    # data_change_landmarks = copy.deepcopy(data_expand_rect)
    # data_change_landmarks, delete_value1, delete_value2 = data_set.change_data_landmarks(data_change_landmarks)
    # data_set.check_show(delete_value2)
    # print(len(data_change_landmarks), len(delete_value1), len(delete_value2))

    # 5. generate train and test dataset
    # data = copy.deepcopy(data_change_landmarks)
    # train_dataset, test_dataset, lines = data_set.generate_train_test_data(data)
    # print(len(train_dataset), len(test_dataset), len(lines))
    # data_set.write_txt(train_dataset, "train_dataset.txt")
    # data_set.write_txt(test_dataset, "test_dataset.txt")

    # 6. load train dataset and check them
    train = data_set.load_data("train_dataset.txt")
    data_set.data_show_face_rect(train)
    print("Run done.")


if __name__ == '__main__':
    main()
