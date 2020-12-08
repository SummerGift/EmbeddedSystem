# 通过 Clang 生成 AST

## 环境准备

- apt-get install llvm
- apt-get install clang
- clang -v

## 测试过程

测试代码:

```
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        printf("need a para.\n");
        return -1;
    }

    printf("argv0: %s\n", argv[0]);
    int cmd = atoi(argv[1]);

    switch (cmd)
    {
        printf("hello3\n");
        break;

        while (1)
        {
        case 1:
            printf("hello1\n");
            break;

        case 2:
            printf("hello2\n");
            break;

        default:
            printf("hello3\n");
            break;
            /* code */
        }

        printf("hello3\n");
        break;
        }

    return 0;
}

```

## 使用 clang 命令生成 AST

```
clang -cc1 -ast-dump hello.c > hello.ast
```
