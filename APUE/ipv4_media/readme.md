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
`#define BUFSIZE 320*1024/8*3`:
320 * 1024：
表示 320 kbps（千比特每秒，kilobits per second），是一个常见的 MP3 比特率。
将其转换为比特数时，乘以 1024。
/ 8：
将比特数转换为字节数。1 字节 = 8 比特。结果是 320 kbps 转换为每秒的字节数，即 
320×1024÷8=40,960 字节。
* 3：
表示 3 秒的音频数据量。假设缓冲区需要存储 3 秒的音频数据。
结果是40,960×3=122,880 字节。