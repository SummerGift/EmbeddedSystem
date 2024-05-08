# ubuntu 下搭建 tftp 服务

## 安装tftp-server

- sudo apt-get install tftpd-hpa (server)
- sudo apt-get install tftp-hpa（client）

## 配置TFTP服务器

sudo vim /etc/default/tftpd-hpa

将原来的内容改为:

```
TFTP_USERNAME=”tftp”
TFTP_ADDRESS=”0.0.0.0:69″
TFTP_DIRECTORY=”tftp根目录” #服务器目录,需要设置权限为777,chomd 777
TFTP_OPTIONS=”-l -c -s”
```

## 重新启动TFTP服务

`sudo service tftpd-hpa restart`
