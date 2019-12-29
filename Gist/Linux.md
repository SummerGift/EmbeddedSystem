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