# Simple JIT for Brainfxxk

一个[brainfxxk](https://en.wikipedia.org/wiki/Brainfuck)的jit实现，使用方法如下

```bash
usage: jit [-i] filePath
```

`-i`参数表示采用解释执行的方法而不是jit。`test/`目录下为一些测试样例，分别是计算2+5、打印helloworld、一个bf解释器的bf程序以及一个绘制[mandelbrot](https://en.wikipedia.org/wiki/Mandelbrot_set)图像的bf程序。



[`test/nested_interpreter_hello_world.txt`](https://github.com/Paul-Georgeee/brainfxxk-jit/blob/main/test/nested_interpreter_hello_world.txt)为[`bf_interpreter.bf`](https://github.com/Paul-Georgeee/brainfxxk-jit/blob/main/test/bf_interpreter.bf)的一个输入，其为在双层嵌套的bf解释器上运行输出helloworld的程序。在使用jit的情况下，运行时间为

```bash
time ./build/bfjit test/bf_interpreter.bf  <test/nested_interpreter_hello_world.txt 
Hello World!

real    0m2.784s
user    0m2.785s
sys     0m0.000s
```

采用解释执行的方式，运行时间为

```bash
time ./build/bfjit -i test/bf_interpreter.bf  <test/nested_interpreter_hello_world.txt 
Hello World!

real    0m44.882s
user    0m44.882s
sys     0m0.000s
```

[`test/mandelbrot.bf`](https://github.com/Paul-Georgeee/brainfxxk-jit/blob/main/test/mandelbrot.bf)的运行时间为

```bash
time ./build/bfjit test/mandelbrot.bf
real    0m0.795s
user    0m0.795s
sys     0m0.000s

time ./build/bfjit -i test/mandelbrot.bf
real    0m12.174s
user    0m12.174s
sys     0m0.000s
```

可以看到，jit的方式确实大幅度地提升了速度



## Just-in-time compilation

所知的编程语言的执行方式一般可以分为两类：先编译后再执行，例如C/C++；直接对代码解释执行，例如python。[JIT](https://en.wikipedia.org/wiki/Just-in-time_compilation)结合了二者的特性，即一边编译一边执行，这样子除了比直接解释执行快之外，还有另外一个优点是可以根据运行时的信息来进行优化

>  A system implementing a JIT compiler typically continuously analyses the code being executed and identifies parts of the code where the speedup gained from compilation or recompilation would outweigh the overhead of compiling that code.  																--From wiki

第一次听到jit的概念是在[南京大学的20年的pa手册中](https://nju-projectn.github.io/ics-pa-gitbook/ics2020/5.3.html)，（不知道什么原因从21年开始手册便不再出现这个）。在这里面也解释了jit为何能比解释执行快很多的原因

>  执行一次`isa_exec_once()`, 客户程序才执行一条指令, 但运行NEMU的真机却需要执行上百条native指令. 这也是NEMU性能不高的根本原因.
>
> ....
>
> ```
> 100000:    b8 34 12 00 00        mov    $0x1234,%eax
> ```
>
> 每次执行到这条指令的时候, 取指都是取到相同的比特串, 译码总是发现"要将0x1234移动到`eax`中", 更新PC后其值也总是`0x100005`. 反正执行客户指令的结果就是改变计算机的状态, 这不就和执行
>
> ```
> mov $0x1234,(cpu.eax的地址)
> mov $0x100005,(cpu.pc的地址)
> ```
>
> 这两条native指令的效果一样吗?

当时看到这里还是懵懵懂懂，怎么生成这两条指令？`cpu.eax`和`cpu.pc`的地址怎么知道？怎么执行生成的代码？



下面以一个简单的例子来说明代码中是如何实现jit的：

对于一段bf程序`[++>>++--]`，采用对应的[中间代码](https://github.com/Paul-Georgeee/brainfxxk-jit/blob/main/include/IR.hpp)来表示，这里会对连续的加减进行合并

```
Jz;
AddVal 2;
AddPtr 2;
Jnz
```

那么我们只需要将对应转化成对应的机器代码，这里以汇编形式给出，具体的生成过程[见代码](https://github.com/Paul-Georgeee/brainfxxk-jit/blob/main/src/jit.cpp#L41)

```assembly
	cmp  byte ptr [rbx], 0
	je   L2
L1:
	add  byte ptr [rbx], 2
	add  rbx, 2
	cmp  byte ptr [rbx], 0
	jne   L1
L2:
```

并跳转到对应的位置开始执行即可。

转化成目标代码可以自己手写，由于只有几个类型，写起来也不麻烦。更方便的一种做法是借用[luajit中的dynasm](https://luajit.org/dynasm.html)，具体用法可以参照[这篇文章](https://blog.reverberate.org/2012/12/hello-jit-world-joy-of-simple-jits.html)，此外还有一个[unoffcial Doc](http://corsix.github.io/dynasm-doc/index.html)可以参考，官方文档给的东西实在是太少了。不过采用dynasm的话使用C++时有时候编译会报一些奇怪的错误，在网上也找不到对应的解释😥

