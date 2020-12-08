# 通过 Clang 生成 AST

通过查看 C 语言的 BNF （[The syntax of C in Backus-Naur Form](https://cs.wmich.edu/~gupta/teaching/cs4850/sumII06/The%20syntax%20of%20C%20in%20Backus-Naur%20form.htm)）可以知道：

```
<selection-statement> ::= if ( <expression> ) <statement>
                        | if ( <expression> ) <statement> else <statement>
                        | switch ( <expression> ) <statement>
```

switch 语句后只要是 `<statement>` 即可，也就是说，可以是任意合法语句，通过下面的实验，通过 C 语言的 AST 推断程序的运行行为。

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

运行结果如下图所示：

![1607412094452](assets/1607412094452.png)