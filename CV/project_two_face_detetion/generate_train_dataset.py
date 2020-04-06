#
# Copyright (c) 2020 SummerGift
#
# Change Logs:
# Date           Author          Notes
# 2020-4-05      SummerGift      create first version
#

import os
import numpy as np
import cv2
from matplotlib import pyplot as plt


class ShowFigures:
    def __init__(self, figure_path):
        self.figure_path = figure_path

    def show(self):
        template = cv2.cvtColor(cv2.imread(self.figure_path, 1), cv2.COLOR_BGR2RGB)
        plt.figure(figsize=(7, 7), dpi=120)
        plt.imshow(template)
        plt.show()


class GenTrainDataset:
    def __init__(self, figure_path, sourcefile):
        self.figures_root_path = figure_path
        self.key_point_file = sourcefile

    def load_metadata(self):
        print(self.figures_root_path)
        print(self.key_point_file)

    @staticmethod
    def remove_invalid_image(self, lines):
        images = list()
        for line in lines:
            name = line.split()[0]
            if os.path.isfile(name):
                images.append(line)
        return images

    def load_metadata(self, data_folder_list):
        tmp_lines = list()

        for folder_name in data_folder_list:
            folder = os.path.join('data', folder_name)
            metadata_file = os.path.join(folder, 'label.txt')
            with open(metadata_file) as f:
                lines = f.readlines()
            tmp_lines.extend(list(map((folder + '/').__add__, lines)))

        res_lines = self.remove_invalid_image(tmp_lines)
        return res_lines


def main():
    data_folder_list = ['I', 'II']
    figure = ShowFigures('data/I/000010.jpg')
    figure.show()


if __name__ == "__main__":
    main()
