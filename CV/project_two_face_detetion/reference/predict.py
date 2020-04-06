# from __future__ import print_function
# import argparse
# import torch
# import torch.nn as nn
# import torch.nn.functional as F
# import torch.optim as optim
# from torch.utils.data.sampler import SubsetRandomSampler
# from torchvision import datasets, transforms
# import numpy as np
# import os
# import cv2
#
# from data import get_train_test_set
#
#
# # 此部分代码针对stage 1中的predict。 是其配套参考代码
# # 对于stage3， 唯一的不同在于，需要接收除了pts以外，还有：label与分类loss。
#
# def predict(args, trained_model, model, valid_loader):
#     model.load_state_dict(torch.load(os.path.join(args.save_directory, trained_model)))  # , strict=False
#     model.eval()  # prep model for evaluation
#     with torch.no_grad():
#         for i, batch in enumerate(valid_loader):
#             # forward pass: compute predicted outputs by passing inputs to the model
#             img = batch['image']
#             landmark = batch['landmarks']
#             print('i: ', i)
#             # generated
#             output_pts = model(img)
#             outputs = output_pts.numpy()[0]
#             print('outputs: ', outputs)
#             x = list(map(int, outputs[0: len(outputs): 2]))
#             y = list(map(int, outputs[1: len(outputs): 2]))
#             landmarks_generated = list(zip(x, y))
#             # truth
#             landmark = landmark.numpy()[0]
#             x = list(map(int, landmark[0: len(landmark): 2]))
#             y = list(map(int, landmark[1: len(landmark): 2]))
#             landmarks_truth = list(zip(x, y))
#
#             img = img.numpy()[0].transpose(1, 2, 0)
#             img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
#             for landmark_truth, landmark_generated in zip(landmarks_truth, landmarks_generated):
#                 cv2.circle(img, tuple(landmark_truth), 2, (0, 0, 255), -1)
#                 cv2.circle(img, tuple(landmark_generated), 2, (0, 255, 0), -1)
#
#             cv2.imshow(str(i), img)
#             key = cv2.waitKey()
#             if key == 27:
#                 exit()
#             cv2.destroyAllWindows()
