### 安装 node 到 linux 系统

```
wget https://nodejs.org/dist/v12.14.0/node-v12.14.0-linux-x64.tar.xz 
&& tar -xvf node-v12.14.0-linux-x64.tar.xz 
&& echo export PATH=$PATH:`pwd`/node-v12.14.0-linux-x64/bin > ~/.bashrc 
&& source ~/.bashrc && npm -v
```

### 加速 Python pip 包安装速度

- 添加 -i https://pypi.tuna.tsinghua.edu.cn/simple 后缀可以从国内清华源来下载 Python 包

`
python -m pip install -i https://pypi.tuna.tsinghua.edu.cn/simple packages_name
`

### ubuntu wsl 子系统使用win10 系统ss代理步骤

1. 安装python pip
`
apt install python-pip
 `
 
2. 升级pip
`
pip install --upgrade pip
`

3. 安装genpac 工具
`
pip install genpac
 `
 
4. 生成配置

```
genpac --proxy="SOCKS5 127.0.0.1:1080" -o autoproxy.pac 
--gfwlisturl="https://raw.githubusercontent.com/gfwlist/gfwlist/master/gfwlist.txt"
```

如果上述 github 地址不能访问，可以手动下载该文件，然后使用 --gfwlist-local=FILE 指令来指定 List.

5. 编辑 /etc/profile 在文件最后添加（具体端口配置和ss 客户端保持一致）

```
export http_proxy=http://127.0.0.1:1080
export https_proxy=http://127.0.0.1:1080
export ftp_proxy=http://127.0.0.1:1080
```

6. 让配置生效就可以了
`
source /etc/profile 
`

### 在 Linux 上使用本地局域网中的代理服务上网

在 linux 下配置代理上网操作比较复杂，非常麻烦，而且有些最新的加密算法还不支持。而在 win 下，有很多好用的代理软件，
支持负载均衡等常用功能，配置起来也十分方便，因此我们可以换一种方式来解决 linux 机器上网的问题。

即在 linux 中通过 win 下的正向代理 socket 服务上网。

可以使用如下语句配置：

```
export https_proxy=http://xxx.xxx.xxx.xxx:port
```

通过上述语句，可以使本地的 https 连接通过局域网的 `http://xxx.xxx.xxx.xxx:port` 端口提供的代理服务来访问互联网，
但是这样设置是一次性的，因此可以将其添加到启动初始化文件中：

```
vi ~/.bashrc
```

在最后添加上述配置语句，即可每次开机自动使用本地局域网代理。

### Package libffi was not found in the pkg-config search path

site:
https://config9.com/linux/package-libffi-was-not-found-in-the-pkg-config-search-path-redhat6-5/

```
sudo apt-get install libffi-dev
```

### gcc 编译出现 /usr/include/stdio.h:27:10: fatal error: bits/libc-header-start.h

可以尝试输入如下命令，主要是gcc安装环境没有安装完善。

```
sudo apt-get install gcc-multilib
```

### 定时任务管理器 crontab 常用操作

- 查看当前系统任务
```
crontab -l
```

- 指定文件为当前系统任务列表
```
crontab file
```

注意事项：

1. 使用 sh 脚本作为系统任务，便于收集日志，统计运行时间
2. 如果需要设置一次性的环境变量，那么需要在 sh 脚本中重新设置，否则系统启动的临时环境变量可能不生效





