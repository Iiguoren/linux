S端发送：
    数据源：medialib
        定义频道，每个频道定义文件
    流量控制
    多线程发送多个channel
C端接受:
    ，通过进程间通讯使用解码器播放

getoptlong


### 设置套接字多播组
`setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof(mreqn));`
设置套接字加入一个多播组
```c
struct ip_mreqn {
    struct in_addr imr_multiaddr;  // 多播组地址
    struct in_addr imr_address;   // 本地接口地址
    int imr_ifindex;              // 接口索引
};
```