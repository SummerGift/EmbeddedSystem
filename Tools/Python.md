## install opencv with sift algorithem

```
pip uninstall opencv-python
pip install opencv-python==3.4.2.16 -i "https://pypi.doubanio.com/simple/"
pip install opencv-contrib-python==3.4.2.16 -i "https://pypi.doubanio.com/simple/"
```

## conda 命令行启动及取消方法 

### 激活 conda 命令行环境

- 激活命令
```
conda activate base
```
- 自动进入 conda base 环境命令
```
conda config --set auto_activate_base true
```

### 取消激活 conda 的基础环境

- 通过 `conda deactivate` 可以退出 base 环境回到系统自动的环境

- 取消每次自动进入 conda base 环境
```
conda config --set auto_activate_base false
```
