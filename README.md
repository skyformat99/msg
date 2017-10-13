# msg

使用说明

1.将MSG.C MSG.H文件拷贝到自己工程下，如果有命名冲突 自行解决
2.
    #include "malloc.h"
    #include "stdlib.h"
    #include "stdio.h"
  这是外部的头文件，使用了malloc 所以如果在低端设备使用时候注意
  设置堆区的大小
3. msg_create  创建对象，主要有缓存之类
4. msg_subscribe  建立标号回调函数
5. msg_publish    发布消息
6. msg_recv_data  中断接受消息函数


在subscribe中，有两种模式，一种是内存拷贝模式，不需要回调，即来了标号
的数据，就会自动将内存复制拷贝到特定的区域。一种回调机制，即来了标号，自行拷贝
可以加一些标志位。