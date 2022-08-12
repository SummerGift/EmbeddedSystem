# GCC 对特定代码段设置优化等级

## 单个文件优化等级修改

如果对单个文件修改优化等级：

- 使用 `scons --verbose` 打印出所有编译步骤，并整理为单个批处理编译文件
- 手动修改该文件，按单个文件修改编译等级
- 找出被优化后无法正常工作的源码文件

## 文件内代码段优化等级修改

可使用如下扩展语法设置代码段的优化等级。

```c
#pragma GCC push_options         // 入栈
#pragma GCC optimize ("O0")      // 设置优化等级

//your code...

#pragma GCC pop_options          // 出栈
```

