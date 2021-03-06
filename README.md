# 空间索引测试

## 1、测试简要说明

- 1、测试数据必须有矩形外包框范围，单位为度，限制在地球范围内
- 2、测试数据还可以有至少一个时间  dtme ,以及一个卫星类型 sat
- 3、主要测试插入时间和查询时间

测试数据采用[code/test.hpp](code/test.hpp)文件中的`getNextData`函数生成，查询参数使用`getQueryArg`函数生成。
现有通过继承`TestBase`类，并实现`init/insert/query`来进行测试的代码，可供参考。

为保证测试过程生成的随机数据是一致的，生成数据前必须使用相同的随机数种子进行初始化，可以参考`TestIter`实现。

测试结果**以[code/testiter.cpp](code/testiter.cpp)代码编译程序输出为准(顺序遍历)**，必须输出一致，部分空间索引框架检索粒度较粗，可以采用二次过滤。

## 2、测试项

1、顺序遍历
> 直接遍历每一个索引节点，匹配符合查询条件的节点输出。顺序遍历结果可确保没有检索错误情况。

2、GEOS STRtree
> 采用GEOS库的STRtree空间索引树进行空间索引，其它非空间索引部分采用二次遍历实现

3、GEOS Quadtree
> 采用GEOS库的Quadtree（四叉树）空间索引树进行空间索引，其它非空间索引部分采用二次遍历实现

4、SQLite R*Tree
> 采用SQLite数据库组织需要检索数据，对空间范围建立索引，其它字段不建索引，采用SQL语句进行查询


## 3、测试结果

测试环境：
> 操作系统 : CentOS 7.0 x86_64
> 内存    ：16GiB
> CPU     :Intel(R) Core(TM) i7-7700 CPU @ 3.60GHz
> 编译器  ：gcc 4.8.5

测试结果:

测试输出可见[resultlog](resultlog)目录

![测试结果](resultlog/测试结果.png)
![测试结果对比](resultlog/测试结果对比.png)




