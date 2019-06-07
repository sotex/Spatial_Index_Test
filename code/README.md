## 代码说明

- 1、[test.hpp](test.hpp) 测试基本定义，包括Node、矩形求交计算、数据生成等。
- 2、[testiter.cpp](testiter.cpp) 顺序遍历测试代码。
- 3、[testgeos.cpp](testgeos.cpp) GEOS STRtree测试代码。
- 4、[testgeos2.cpp](testgeos2.cpp) GEOS Quadtree测试代码。
- 5、[testsqlite.cpp](testsqlite.cpp) SQLITE测试代码。

## 编译说明

**linux下编译：**
1、安装g++、make、GEOS

```bash
# Ubuntu
apt install g++ make libgeos

# Centos
yum install gcc-c++ make geos-devel
```

2、编译
```bash
make
```

**Windows下使用MSVC编译:**

1、需要安装VS，建议安装最新版本。

2、需要编译好的geos，可以vcpkg安装。修改[code/Makefile.vc](code/Makefile.vc)文件里面的`GEOS_INC`和`GEOS_LIB`两个变量，指向正确的路径。可能还需要修改`CXXFLAGS`变量，根据具体编译环境修改。

打开VS开发者命令行工具，使用`nmake -f Makefile.vc`脚本进行编译。
