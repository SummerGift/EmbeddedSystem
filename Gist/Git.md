# Git  常用操作及技巧

[GIT 命令大全](http://www.cnblogs.com/Small-music/p/9075681.html)

## Git 分支命令

- 创建name的分支，并切换过去
```sh
      git checkout -b name
```
- 拉取远程分支，创建并切换到这个分支
```sh
      git checkout -b name origin/remote-branch
      git checkout -b 本地分支名 origin/远程分支名
```
- 查看所有分支
```sh
      git branch -v
```
- 删除分支
```sh
      git branch -d name
      git branch -D name
```
- 合并到master
```sh
      git checkout master
      git merge name
```
- 暂存所有更改到本地分支
```sh
      git stash
```
- 恢复
```sh
      git stash pop
```

## Git 撤销本地提交
```
git reset --mixed HEAD~2
```

## Git 中设置代理
```sh
git config --global https.proxy http://127.0.0.1:1080
git config --global https.proxy https://127.0.0.1:1080

git config --global --unset http.proxy
git config --global --unset https.proxy
```

## Git 推本地 master 到远端非 master 分支
```sh
git push origin master:fix_branch
```

## 添加 submodule

为当前工程添加submodule，命令如下：

```
git submodule add repo_url path
```

其中，repo_url 是指子模块仓库地址，path 指将子模块放置在当前工程下的路径。 
注意：路径不能以 / 结尾（会造成修改不生效）、不能是现有工程已有的目录（不能顺利 Clone）。

命令执行完成，会在当前工程根路径下生成一个名为“.gitmodules”的文件，其中记录了子模块的信息。添加完成以后，再将子模块所在的文件夹添加到工程中即可。

强制添加可以使用  --force 参数。

## 更新 submodule

更新当前仓库中的 submodule，命令如下：

```sh
git submodule update --init --recursive
```

参数 `--recursive` 的意思是递归更新子模块，也就是会更新子模块的子模块，如果不加这个参数，则只会更新第一级的子模块。

## 删除 submodule

删除当前仓库中的 submodule，方法如下：

- 删除 .gitsubmodule  文件中对应的 submodule。

- 删除 .git/config 中对应的 submodule 项（这一步我测试的时候如果不做也可以，后面再次添加子模块的时候加上 --force 参数也可以再次添加）。

- 执行命令 `git rm --cached <submodule_path>`，执行命令前，请确保该 Git 仓库没有未提交的内容，否则会执行失败，示例如下：

```sh
git rm --cached hello
```

## GIT 与远程 REPOSITORY 同步 TAG 和BRANCH

参考如下地址：http://smilejay.com/2013/04/git-sync-tag-and-branch-with-remote/

## 输入输出均不转换换行符

```sh
git config --global core.autocrlf false
```
## 克隆时指定日志层数

这种方法可以减少下载文件的数量，大大提高下载速度。

```sh
git clone --depth=1 Git_URL
```
