# from __future__ import print_function
# import argparse
# import torch
# import torch.nn as nn
# import torch.nn.functional as F
# import torch.optim as optim
# from torch.utils.data.sampler import SubsetRandomSampler
# from torchvision import datasets, transforms
#
# import runpy
# import numpy as np
# import os
# import cv2
#
# from data import get_train_test_set
# from predict import predict
#
# torch.set_default_tensor_type(torch.FloatTensor)
#
#
# class Net(nn.Module):
#     def __init__(self):
#         super(Net, self).__init__()
#         # Backbone:
#         # in_channel, out_channel, kernel_size, stride, padding
#         # block 1
#         self.conv1_1 = nn.Conv2d(1, 8, 5, 2, 0)
#         # block 2
#         self.conv2_1 = nn.Conv2d(8, 16, 3, 1, 0)
#         self.conv2_2 = nn.Conv2d(16, 16, 3, 1, 0)
#         # block 3
#         self.conv3_1 = nn.Conv2d(16, 24, 3, 1, 0)
#         self.conv3_2 = nn.Conv2d(24, 24, 3, 1, 0)
#         # block 4
#         self.conv4_1 = nn.Conv2d(24, 40, 3, 1, 1)
#         # points branch
#         self.conv4_2 = nn.Conv2d(40, 80, 3, 1, 1)
#         self.ip1 = nn.Linear(4 * 4 * 80, 128)
#         self.ip2 = nn.Linear(128, 128)
#         self.ip3 = nn.Linear(128, 42)
#         # common used
#         self.prelu1_1 = nn.PReLU()
#         self.prelu2_1 = nn.PReLU()
#         self.prelu2_2 = nn.PReLU()
#         self.prelu3_1 = nn.PReLU()
#         self.prelu3_2 = nn.PReLU()
#         self.prelu4_1 = nn.PReLU()
#         self.prelu4_2 = nn.PReLU()
#         self.preluip1 = nn.PReLU()
#         self.preluip2 = nn.PReLU()
#         self.ave_pool = nn.AvgPool2d(2, 2, ceil_mode=True)
#
#     def forward(self, x):
#         # block 1
#         # print('x input shape: ', x.shape)
#         x = self.ave_pool(self.prelu1_1(self.conv1_1(x)))
#         # print('x after block1 and pool shape should be 32x8x27x27: ', x.shape)     # good
#         # block 2
#         x = self.prelu2_1(self.conv2_1(x))
#         # print('b2: after conv2_1 and prelu shape should be 32x16x25x25: ', x.shape) # good
#         x = self.prelu2_2(self.conv2_2(x))
#         # print('b2: after conv2_2 and prelu shape should be 32x16x23x23: ', x.shape) # good
#         x = self.ave_pool(x)
#         # print('x after block2 and pool shape should be 32x16x12x12: ', x.shape)
#         # block 3
#         x = self.prelu3_1(self.conv3_1(x))
#         # print('b3: after conv3_1 and pool shape should be 32x24x10x10: ', x.shape)
#         x = self.prelu3_2(self.conv3_2(x))
#         # print('b3: after conv3_2 and pool shape should be 32x24x8x8: ', x.shape)
#         x = self.ave_pool(x)
#         # print('x after block3 and pool shape should be 32x24x4x4: ', x.shape)
#         # block 4
#         x = self.prelu4_1(self.conv4_1(x))
#         # print('x after conv4_1 and pool shape should be 32x40x4x4: ', x.shape)
#
#         # points branch
#         ip3 = self.prelu4_2(self.conv4_2(x))
#         # print('pts: ip3 after conv4_2 and pool shape should be 32x80x4x4: ', ip3.shape)
#         ip3 = ip3.view(-1, 4 * 4 * 80)
#         # print('ip3 flatten shape should be 32x1280: ', ip3.shape)
#         ip3 = self.preluip1(self.ip1(ip3))
#         # print('ip3 after ip1 shape should be 32x128: ', ip3.shape)
#         ip3 = self.preluip2(self.ip2(ip3))
#         # print('ip3 after ip2 shape should be 32x128: ', ip3.shape)
#         ip3 = self.ip3(ip3)
#         # print('ip3 after ip3 shape should be 32x42: ', ip3.shape)
#
#         return ip3
#
#
# def train(args, train_loader, valid_loader, model, criterion, optimizer, device):
#     # save model
#     if args.save_model:
#         if not os.path.exists(args.save_directory):
#             os.makedirs(args.save_directory)
#
#     epoch = args.epochs
#     pts_criterion = criterion
#
#     train_losses = []
#     valid_losses = []
#
#     for epoch_id in range(epoch):
#         # monitor training loss
#         train_loss = 0.0
#         valid_loss = 0.0
#         ######################
#         # training the model #
#         ######################
#         model.train()
#         for batch_idx, batch in enumerate(train_loader):
#             img = batch['image']
#             landmark = batch['landmarks']
#
#             # ground truth
#             input_img = img.to(device)
#             target_pts = landmark.to(device)
#
#             # clear the gradients of all optimized variables
#             optimizer.zero_grad()
#
#             # get output
#             output_pts = model(input_img)
#
#             # get loss
#             loss = pts_criterion(output_pts, target_pts)
#
#         # do BP automatically
#         loss.backward()
#         optimizer.step()
#
#         # show log info
#         if batch_idx % args.log_interval == 0:
#             print('Train Epoch: {} [{}/{} ({:.0f}%)]\t pts_loss: {:.6f}'.format(
#                 epoch_id,
#                 batch_idx * len(img),
#                 len(train_loader.dataset),
#                 100. * batch_idx / len(train_loader),
#                 loss.item()
#             )
#             )
#
#     ######################
#     # validate the model #
#     ######################
#     valid_mean_pts_loss = 0.0
#
#     model.eval()  # prep model for evaluation
#     with torch.no_grad():
#         valid_batch_cnt = 0
#
#         for valid_batch_idx, batch in enumerate(valid_loader):
#             valid_batch_cnt += 1
#             valid_img = batch['image']
#             landmark = batch['landmarks']
#
#             input_img = valid_img.to(device)
#             target_pts = landmark.to(device)
#
#             output_pts = model(input_img)
#
#             valid_loss = pts_criterion(output_pts, target_pts)
#
#             valid_mean_pts_loss += valid_loss.item()
#
#         valid_mean_pts_loss /= valid_batch_cnt * 1.0
#         print('Valid: pts_loss: {:.6f}'.format(
#             valid_mean_pts_loss
#         )
#         )
#
#
# print('====================================================')
# # save model
# if args.save_model:
#     saved_model_name = os.path.join(args.save_directory, 'detector_epoch' + '_' + str(epoch_id) + '.pt')
#     torch.save(model.state_dict(), saved_model_name)
# return loss, 0.5
#
#
# def main_test():
#     parser = argparse.ArgumentParser(description='Detector')
#     parser.add_argument('--batch-size', type=int, default=64, metavar='N',
#                         help='input batch size for training (default: 64)')
#     parser.add_argument('--test-batch-size', type=int, default=64, metavar='N',
#                         help='input batch size for testing (default: 64)')
#     parser.add_argument('--epochs', type=int, default=100, metavar='N',
#                         help='number of epochs to train (default: 100)')
#     parser.add_argument('--lr', type=float, default=0.001, metavar='LR',
#                         help='learning rate (default: 0.001)')
#     parser.add_argument('--momentum', type=float, default=0.5, metavar='M',
#                         help='SGD momentum (default: 0.5)')
#     parser.add_argument('--no-cuda', action='store_true', default=False,
#                         help='disables CUDA training')
#     parser.add_argument('--seed', type=int, default=1, metavar='S',
#                         help='random seed (default: 1)')
#     parser.add_argument('--log-interval', type=int, default=20, metavar='N',
#                         help='how many batches to wait before logging training status')
#     parser.add_argument('--save-model', action='store_true', default=True,
#                         help='save the current Model')
#     parser.add_argument('--save-directory', type=str, default='trained_models',
#                         help='learnt models are saving here')
#     parser.add_argument('--phase', type=str, default='Train',  # Train/train, Predict/predict, Finetune/finetune
#                         help='training, predicting or finetuning')
#     args = parser.parse_args()
#     ###################################################################################
#     torch.manual_seed(args.seed)
#     # For single GPU
#     use_cuda = not args.no_cuda and torch.cuda.is_available()
#     device = torch.device("cuda" if use_cuda else "cpu")  # cuda:0
#     # For multi GPUs, nothing need to change here
#     kwargs = {'num_workers': 1, 'pin_memory': True} if use_cuda else {}
#
#     print('===> Loading Datasets')
#     train_set, test_set = get_train_test_set()
#     train_loader = torch.utils.data.DataLoader(train_set, batch_size=args.batch_size, shuffle=True)
#     valid_loader = torch.utils.data.DataLoader(test_set, batch_size=args.test_batch_size)
#
#     print('===> Building Model')
#     # For single GPU
#     model = Net().to(device)
#     ####################################################################
#     criterion_pts = nn.MSELoss()
#     optimizer = optim.SGD(model.parameters(), lr=args.lr, momentum=args.momentum)
#     ####################################################################
#     if args.phase == 'Train' or args.phase == 'train':
#         print('===> Start Training')
#         train_losses, valid_losses = \
#             train(args, train_loader, valid_loader, model, criterion_pts, optimizer, device)
#         print('====================================================')
#         elif args.phase == 'Test' or args.phase == 'test'
#         print('===> Test')
#     # how to do test?
#
# elif args.phase == 'Finetune' or args.phase == 'finetune':
# print('===> Finetune')
# # how to do finetune?
# elif args.phase == 'Predict' or args.phase == 'predict':
# print('===> Predict')
# # how to do predict?
#
#
# if __name__ == '__main__':
#     main_test()
