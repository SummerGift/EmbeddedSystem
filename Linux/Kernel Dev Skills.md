# Kernel Dev Skills


## kernel 代码格式化 

### Astyle 格式化

```C
astyle --style=linux --indent=force-tab=8 --align-pointer=name --max-code-length=90 --break-after-logical -p -H -U drivers/folder/*.c  drivers/folder/*.h
```

### 文件夹格式化检查

```C
scripts/checkpatch.pl -f drivers/folder/*.c
```

### 检查后将修改存为 patch

```C
scripts/checkpatch.pl -f drivers/folder/*.c --fix
```

### 检查后在文件上修改

```C
scripts/checkpatch.pl -f drivers/folder/*.c --fix-inplace
```

### 将修改整理成 patch 后检查

```C
git format-patch -1
scripts/checkpatch.pl 0001-kernel-xxx-init.patch
```