# ATOM

## 来源

本人之前学习过`CMake`,深感其繁琐操作，3年高性能计算的开发经验匆匆而过，对`CPP`的理解也更上一层楼。

为了熟悉`Xmake`的编译内容，也是为了熟悉最新的编译系统，特地将本`repo`更改为一个支持多平台，异构形式的项目模板框架。

希望对各位高性能计算的开发者们有所帮助，欢迎多多指点。

## 内容

根据个人经验理解，一个合格的项目框架需要有

1. `asset` 主要用于放置各种参考文件
2. `output`   主要用于版本发布的成品，其中包括二进制文件、调试文件等
3. `build` 主要存放编译时产生的中间文件
4. `include` 主要存放当前项目的整体头文件
5. `src` 主要存放当前项目的源文件
6. `lib` 主要存放当前项目输出的库文件
    - `static`   输出的静态库文件
    - `dynamic`   输出的动态库文件
7. `deps` 主要存放当前项目的第三方依赖
8. `dfx`    主要存放当前项目的bench、tests相关
    - `bench` 主要存放bench_mark相关的文件
    - `tests` 主要存放tests相关的文件
    - `meta` 对编译期类型做单元测试
    - `internal` dfx 构建所需要的相关头文件
9.  `scripts` 主要存放当前项目启动时可能需要的脚本

## 特色

- 基于cpp的跨平台项目工程框架
- 优化了各个模式下的编译选项
- 支持性能bench_mark、功能unit_test


## 展望

- 支持nvcc编译

## 参考

[Cmake链接](https://cmake.org/)

[Xmake参考手册](https://xmake.io/#/getting_started)
