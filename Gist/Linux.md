### 安装 node 到 linux 系统

```
wget https://nodejs.org/dist/v12.14.0/node-v12.14.0-linux-x64.tar.xz 
&& tar -xvf node-v12.14.0-linux-x64.tar.xz 
&& echo export PATH=$PATH:`pwd`/node-v12.14.0-linux-x64/bin > ~/.bashrc 
&& source ~/.bashrc && npm -v
```
