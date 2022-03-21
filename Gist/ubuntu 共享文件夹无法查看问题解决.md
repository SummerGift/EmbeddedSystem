# ubuntu 共享文件夹看不到

放弃安装 vmware-tools，在 https://askubuntu.com/questions/74825/why-dont-shared-files-show-up-in-hgfs 找到了答案：

```shell
mkdir /mnt/hgfs
sudo vmhgfs-fuse .host:/ /mnt/hgfs/ -o allow_other -o uid=1000
```

原文链接：https://blog.csdn.net/xiajx98/article/details/91042846
