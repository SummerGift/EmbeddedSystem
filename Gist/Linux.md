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

