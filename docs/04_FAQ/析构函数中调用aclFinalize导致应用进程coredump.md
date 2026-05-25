# 析构函数中调用去初始化接口aclFinalize导致应用进程coredump

## 问题现象描述

应用程序运行过程中出现core dump，应用程序异常终止。

## 原因分析

1.  生成coredump文件。
    -   物理机场景，执行**ulimit -c unlimited**命令，表示在程序崩溃时生成coredump文件：

        完成问题定位后，如果不需要生成coredump文件，可执行**ulimit -c 0**命令。

    -   Docker场景，在Docker启动命令中增加**--ulimit core=-1**设置。

2.  运行应用程序，若进程崩溃，即可在当前目录下生成coredump文件。
3.  使用gdb工具调试core文件、打印堆栈信息。

    进入gdb模式，调试coredump文件，命令示例如下。其中，_main_表示产生coredump文件的可执行程序名称，可根据实际情况修改；coredump文件名需根据实际文件名称修改。

    ```
    gdb main core*.*
    ```

    执行命令后，gdb工具会将发生异常的代码、其所在的函数、文件名和所在文件的行数打印到屏幕，堆栈信息的最上面是最底层的调用栈信息，方便定位问题。堆栈信息举例如下：

    ```
    Thread 1 "main" received signal SIGSEGV, Segmentation fault.
    0x0000ffffa70747c8 in ge::PluginManager::~PluginManager() () from ******/lib64/libge_common.so
    (gdb) bt
    #0 0x0000ffffa70747c8 in ge::PluginManager::~PluginManager() () from ******/lib64/libge_common.so
    #1 0x0000ffffa707c900 in ge::RuntimePluginLoader::Finalize() () from ******/lib64/libge_common.so
    #2 0x0000ffffa29485d0 in ge::GeExecutor::FinalizeEx() () from ******/lib64/libge_executor.so
    #3 0x0000ffffb06fabc in aclFinalize() from ******/lib64/libascendcl.so
    #4 0x0000ffffbd5a98ec in ResourceManager::~ResourceManager() () from ******/envs/gly/lib/pythons3.7/site-packages/mindspore/_c_dataengine.cpython-37m-aarch64-linux-gnu.so
    #5 0x0000ffffbd5a9f80 in std::Sp_counted_ptr<ResourceManager*, (__gnu_cxx::Lock_policy)2>::_M_dispose() () from ******/envs/gly/lib/pythons3.7/site-packages/mindspore/_c_dataengine.cpython-37m-aarch64-linux-gnu.so
    #6 0x0000ffffbd5a97f0 in std::shared_ptr<ResourceManager>::~shared_ptr() () from ******/envs/gly/lib/pythons3.7/site-packages/mindspore/_c_dataengine.cpython-37m-aarch64-linux-gnu.so
    ```

    **注意**，调试coredump文件、打印堆栈信息要在出现问题的运行环境中，如果换一套环境，可能导致调试的堆栈信息不准确。

    若环境中未安装gdb，则需要安装gdb，可通过包管理（如apt-get install gdb、yum install gdb）进行安装，详细安装步骤及使用方法请参见[GDB官方文档](https://sourceware.org/gdb/)。

4.  分析堆栈信息。

    生成coredump文件、检查打印的堆栈信息后，发现应用程序在调用aclFinalize接口后异常退出，因此初步判断可能是aclFinalize接口使用问题。

5.  排查应用程序代码中aclFinalize接口的调用逻辑。

    排查代码逻辑，发现该aclFinalize接口在析构函数中被调用，但该接口存在使用约束：不建议在析构函数中调用aclFinalize接口，否则在进程退出时可能由于单例析构顺序未知而导致进程异常退出的问题。因此判断是由于在析构函数中调用aclFinalize接口导致应用进程coredump。

## 处理步骤

优化应用程序的代码逻辑，不能在析构函数中调用aclFinalize接口，下文给出正确、错误的代码示例。

-   aclFinalize接口的正确调用示例如下：

    ```
    int main() {
      // 初始化
      // 此处的..表示相对路径，相对可执行文件所在的目录，例如，编译出来的可执行文件存放在out目录下，此处的..就表示out目录的上一级目录
      const char *aclConfigPath = "../src/acl.json";
      aclError ret = aclInit(aclConfigPath);

      // 业务处理代码

      // 去初始化，没有退出main函数，所有资源都可用
      ret = aclFinalize();
      return 0;
    }
    ```

-   aclFinalize接口的错误调用示例如下，使用单例析构去初始化：

    ```
    class ResourceManager {
     public:
      ResourceManager() = default;
      // 单例析构
      ~ResourceManager() {
        // 去初始化
        (void) aclFinalize();
      }
      // 单例构造
      static ResourceManager &Instance() {
        static ResourceManager instance;
        return instance;
      }
      aclError Init() {
        // 初始化 
        // 此处的..表示相对路径，相对可执行文件所在的目录，例如，编译出来的可执行文件存放在out目录下，此处的..就表示out目录的上一级目录
        const char *aclConfigPath = "../src/acl.json";
        return aclInit(aclConfigPath);
      }
    };
    int main() {
      // 初始化
      aclError ret = ResourceManager::Instance().Init();
      // 业务处理代码
      // 没有显式去初始化，最后ResourceManager单例析构时调用aclFinalize
      // 由于单例析构是在main函数退出后才执行，单例析构和进程依赖so的卸载顺序无法控制
      // 会出现aclFinalize访问的一些资源所在so已经被卸载，从而导致进程退出异常
      return 0;
    }
    ```

-   aclFinalize接口的错误调用示例如下，使用全局变量析构去初始化：

    ```
    class ResourceManager {
     public:
      ResourceManager() = default;
      // 全局变量析构
      ~ResourceManager() {
        // 去初始化
        (void) aclFinalize();
      }
      aclError Init() {
        // 初始化
        // 此处的..表示相对路径，相对可执行文件所在的目录，例如，编译出来的可执行文件存放在out目录下，此处的..就表示out目录的上一级目录
        const char *aclConfigPath = "../src/acl.json";
        return aclInit(aclConfigPath);
      }
    };
    // 全局变量构造
    ResourceManager g_resource_manager;
    int main() {
      // 初始化
      aclError ret = g_resource_manager.Init();
      // 业务处理代码
      // 没有显式去初始化，最后ResourceManager全局变量析构时调用aclFinalize
      // 由于全局变量析构是在main函数退出后才执行，全局变量析构和进程依赖so的卸载顺序无法控制
      // 会出现aclFinalize访问的一些资源所在so已经被卸载，从而导致进程退出异常
      return 0;
    }
    ```
