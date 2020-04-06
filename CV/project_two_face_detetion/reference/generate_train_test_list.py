# import numpy as np
# import matplotlib.pyplot as plt
# import os
# import cv2
# import random
# from random import sample
#
# import torch
# import torch.nn.functional as F
#
# from torchvision import transforms, utils
# from torch.utils.data import Dataset, DataLoader
# from torch.utils.data.sampler import SubsetRandomSampler
#
# # 此文件是为大家学习实现face keypoint detection所提供的参考代码。
# #
# # 主要是关于生成数据训练列表的参考代码
# #
# # 涵盖了stage1与stage3的，对于生成数据列表所需要的操作。
# # 这份代码因为缺少主函数，所以【不能】直接运行，仅供参考！
# #
# # 对于stage 1. 需要的操作说明文档已经十分清楚，因而对应的函数不再赘述
# # 对于stage 3. 大家可能会遇到需要随机生成背景crop、或进行与人脸crop计算iou的操作。此份代码同样
# #             有相对应参考代码。
# #
# # 希望大家仅仅是学习此份代码。并在此基础上，完成自己的代码。
# # 这份代码同样还可能涵盖你可能根本用不上的东西，亦不必深究。 原则就是，挑“对自己有用”的东西学
# #
# # 祝大家学习顺利
#
#
# folder_list = ['I', 'II']
# finetune_ratio = 0.8
# negsample_ratio = 0.3  # if the positive sample's iou > this ratio, we neglect it's negative samples
# neg_gen_thre = 100
# random_times = 3
# random_border = 10
# expand_ratio = 0.25
#
# train_list_name = 'train_list.txt'
# test_list_name = 'test_list.txt'
#
# train_boarder = 112
#
# need_record = False
#
# train_list = 'train.txt'
# test_list = 'test.txt'
#
#
# def remove_invalid_image(lines):
#     images = []
#     for line in lines:
#         name = line.split()[0]
#         if os.path.isfile(name):
#             images.append(line)
#     return images
#
#
# def load_metadata():
#     tmp_lines = []
#     for folder_name in folder_list:
#         folder = os.path.join('data', folder_name)
#         metadata_file = os.path.join(folder, 'metadata')
#         with open(metadata_file) as f:
#             lines = f.readlines()
#         tmp_lines.extend(list(map((folder + '/').__add__, lines)))
#     res_lines = remove_invalid_image(tmp_lines)
#     return res_lines
#
#
# def load_truth(lines):
#     truth = {}
#     for line in lines:
#         line = line.strip().split()
#         name = line[0]
#         if name not in truth:
#             truth[name] = []
#         rect = list(map(int, list(map(float, line[1:5]))))
#         x = list(map(float, line[5::2]))
#         y = list(map(float, line[6::2]))
#         landmarks = list(zip(x, y))
#         truth[name].append((rect, landmarks))
#     return truth
#
#
# def expand_roi(x1, y1, x2, y2, img_width, img_height, ratio):  # usually ratio = 0.25
#     width = x2 - x1 + 1
#     height = y2 - y1 + 1
#     padding_width = int(width * ratio)
#     padding_height = int(height * ratio)
#     roi_x1 = x1 - padding_width
#     roi_y1 = y1 - padding_height
#     roi_x2 = x2 + padding_width
#     roi_y2 = y2 + padding_height
#     roi_x1 = 0 if roi_x1 < 0 else roi_x1
#     roi_y1 = 0 if roi_y1 < 0 else roi_y1
#     roi_x2 = img_width - 1 if roi_x2 >= img_width else roi_x2
#     roi_y2 = img_height - 1 if roi_y2 >= img_height else roi_y2
#     return roi_x1, roi_y1, roi_x2, roi_y2, \
#            roi_x2 - roi_x1 + 1, roi_y2 - roi_y1 + 1
#
#
# def channel_norm(img):
#     img = img.astype('float32')
#     m_mean = np.mean(img)
#     m_std = np.std(img)
#
#     print('mean: ', m_mean)
#     print('std: ', m_std)
#
#     return (img - m_mean) / m_std
#
#
# def get_iou(rect1, rect2):
#     # rect: 0-4: x1, y1, x2, y2
#     left1 = rect1[0]
#     top1 = rect1[1]
#     right1 = rect1[2]
#     bottom1 = rect1[3]
#     width1 = right1 - left1 + 1
#     height1 = bottom1 - top1 + 1
#
#     left2 = rect2[0]
#     top2 = rect2[1]
#     right2 = rect2[2]
#     bottom2 = rect2[3]
#     width2 = right2 - left2 + 1
#     height2 = bottom2 - top2 + 1
#
#     w_left = max(left1, left2)
#     h_left = max(top1, top2)
#     w_right = min(right1, right2)
#     h_right = min(bottom1, bottom2)
#     inner_area = max(0, w_right - w_left + 1) * max(0, h_right - h_left + 1)
#     # print('wleft: ', w_left, '  hleft: ', h_left, '    wright: ', w_right, '    h_right: ', h_right)
#
#     box1_area = width1 * height1
#     box2_area = width2 * height2
#     # print('inner_area: ', inner_area, '   b1: ', box1_area, '   b2: ', box2_area)
#     iou = float(inner_area) / float(box1_area + box2_area - inner_area)
#     return iou
#
#
# def check_iou(rect1, rect2):
#     # rect: 0-4: x1, y1, x2, y2
#     left1 = rect1[0]
#     top1 = rect1[1]
#     right1 = rect1[2]
#     bottom1 = rect1[3]
#     width1 = right1 - left1 + 1
#     height1 = bottom1 - top1 + 1
#
#     left2 = rect2[0]
#     top2 = rect2[1]
#     right2 = rect2[2]
#     bottom2 = rect2[3]
#     width2 = right2 - left2 + 1
#     height2 = bottom2 - top2 + 1
#
#     w_left = max(left1, left2)
#     h_left = max(top1, top2)
#     w_right = min(right1, right2)
#     h_right = min(bottom1, bottom2)
#     inner_area = max(0, w_right - w_left + 1) * max(0, h_right - h_left + 1)
#     # print('wleft: ', w_left, '  hleft: ', h_left, '    wright: ', w_right, '    h_right: ', h_right)
#
#     box1_area = width1 * height1
#     box2_area = width2 * height2
#     # print('inner_area: ', inner_area, '   b1: ', box1_area, '   b2: ', box2_area)
#     iou = float(inner_area) / float(box1_area + box2_area - inner_area)
#     return iou
#
#
# def generate_random_crops(shape, rects, random_times):
#     neg_gen_cnt = 0
#     img_h = shape[0]
#     img_w = shape[1]
#     rect_wmin = img_w  # + 1
#     rect_hmin = img_h  # + 1
#     rect_wmax = 0
#     rect_hmax = 0
#     num_rects = len(rects)
#     for rect in rects:
#         w = rect[2] - rect[0] + 1
#         h = rect[3] - rect[1] + 1
#         if w < rect_wmin:
#             rect_wmin = w
#         if w > rect_wmax:
#             rect_wmax = w
#         if h < rect_hmin:
#             rect_hmin = h
#         if h > rect_hmax:
#             rect_hmax = h
#     random_rect_cnt = 0
#     random_rects = []
#     while random_rect_cnt < num_rects * random_times and neg_gen_cnt < neg_gen_thre:
#         neg_gen_cnt += 1
#         if img_h - rect_hmax - random_border > 0:
#             top = np.random.randint(0, img_h - rect_hmax - random_border)
#         else:
#             top = 0
#         if img_w - rect_wmax - random_border > 0:
#             left = np.random.randint(0, img_w - rect_wmax - random_border)
#         else:
#             left = 0
#         rect_wh = np.random.randint(min(rect_wmin, rect_hmin), max(rect_wmax, rect_hmax) + 1)
#         rect_randw = np.random.randint(-3, 3)
#         rect_randh = np.random.randint(-3, 3)
#         right = left + rect_wh + rect_randw - 1
#         bottom = top + rect_wh + rect_randh - 1
#
#         good_cnt = 0
#         for rect in rects:
#             img_rect = [0, 0, img_w - 1, img_h - 1]
#             rect_img_iou = get_iou(rect, img_rect)
#             if rect_img_iou > negsample_ratio:
#                 random_rect_cnt += random_times
#                 break
#             random_rect = [left, top, right, bottom]
#             iou = get_iou(random_rect, rect)
#
#             if iou < 0.2:
#                 # good thing
#                 good_cnt += 1
#             else:
#                 # bad thing
#                 break
#
#         if good_cnt == num_rects:
#             # print('random rect: ', random_rect, '   rect: ', rect)
#             _iou = check_iou(random_rect, rect)
#
#             # print('iou: ', iou, '   check_iou: ', _iou)
#             # print('\n')
#             random_rect_cnt += 1
#             random_rects.append(random_rect)
#     return random_rects
