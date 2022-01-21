#  使用 `$Super$$ and $Sub$$` 修改已有符号

在 ARM Clang  链接器中，对于无法修改的外部链接，可以使用一些特殊的模式来修改他们。

下面的例子展示了使用 `$Super$$` 和 `$Sub$$` 在函数 `foo()` 前插入 `ExtraFunc()` 函数调用。

```c
extern void ExtraFunc(void);
extern void $Super$$foo(void);

/* this function is called instead of the original foo() */
void $Sub$$foo(void)
{
     ExtraFunc();    /* does some extra setup work */
     $Super$$foo();  /* calls the original foo() function */
                     /* To avoid calling the original foo() function
                      * omit the $Super$$foo(); function call.
                      */
}
```

从上面的例子可以看出，使用 `$Sub$$foo` 可以在原函数 `foo()` 运行前加入函数调用，该特性可以用于替换掉原 `foo()` 函数，也可以用于在 `foo()` 函数前后添加更多辅助功能。

上述代码如果不调用 `$Super$$foo()`，就完全替换了 `foo()` 函数，如果调用的话，就会再执行原 `foo()` 函数。
