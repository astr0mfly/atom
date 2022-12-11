# DFX

DFX(Design For X)，本意是为了设计的辅助测试，回头看相关软件评估的方方面面已经成熟，此处框架的搭建有利于提高软件的健壮性，有利于软件重构以及软件的性能评估。

## meta

在知乎上偶尔看到`tlp`模板元编程的单元测试，一发不可收拾，此处引入用来测试模板元编程中的`DSL`，并且尝试修复它不能在`MSVC`运行的问题。

## unitest

使用著名的`catch2`测试库，只需要一个头文件(2.13.10)即可，后续根据情况支持新版本(catch2-v3)，当前的版本已经能够满足要求。

## bench

`bench`文件夹，主要是用于相关类的性能测试, 沿用`catch2`中的相关定义与接口。


## 未完待续

更多周边测试库正在路上。
[FakeIt](https://blog.csdn.net/hatter110/article/details/107744386)

# 引用

[catch2](https://github.com/catchorg/Catch2)

[tlp](https://github.com/MagicBowen/tlp)