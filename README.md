# epoll-frame

`epoll-frame` 是一个使用 `epoll` I/O 模型进行网络通信的高性能服务器框架。此项目提供了可定制化的底层框架代码，可以根据需要自定义服务器逻辑。

特性
- 基于 epoll I/O 模型，具有卓越的性能表现
- 使用 C++11 语言风格编写，代码结构清晰易读
- 支持多线程并发处理网络请求
- 支持定时器管理和事件回调机制
- 提供详细的注释和文档，方便二次开发和应用

## 环境要求

- Linux 操作系统
- GCC 4.8 或更高版本
- CMake 3.0 或更高版本

## 项目文件说明
src文件目录下包含的是旧的C语言文件，基于C语言开发的程序内容。

源项目目录下的结果是基于C++11的代码，使用CMake进行构建，并且包含了一些示例代码和测试用例。


## 应用示例
在linux环境下，使用CMake进行构建，并运行示例程序。
```bash
mkdir build
cd build
cmake ..
make
./epoll_frame_example
```

## 许可证

`epoll-frame` 项目采用 MIT 许可证。有关详细信息，请查看项目中的 LICENSE 文件。

## 参考

如果您在使用过程中遇到任何问题，欢迎联系开发者 [@doctordragon666](https://github.com/doctordragon666)。

感谢您对 `epoll-frame` 框架的支持和使用！
