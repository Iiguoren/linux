## 网络套接字
跨主机传输需要注意的问题:
 1. 字节序问题
        网络传输可能需要统一成某种固定的字节序（大端或小端）：
        大端：低地址处放高字节（常用于网络通信）。
        小段：低地址处放低字节（常见于 x86 架构）。

        主机字节序：host
        网络字节序：network
        __ to __ ：htons(两位从主机发往network),ntohs(四位从network到host)
 2. 对齐
    在跨主机的网络传输中，对齐是必要的，因为不同平台可能对数据的存储方式有不同的对齐规则。

### 结构体的对齐
**问题：一个结构体编译后的内存大小是多大？**
结构体的内存大小取决于以下因素：

字段的类型和顺序：
每个字段的类型决定了它的对齐要求。
字段顺序可能影响填充字节的位置和数量。

对齐规则：
编译器的默认对齐方式（如4字节对齐或8字节对齐）。
手动指定的对齐规则(通过 `#pragma pack` 或 `__attribute__((packed))`)。

填充字节：
为了满足对齐要求，结构体内的某些字段之间可能会添加填充字节。
结构体本身的大小通常也会按其最大对齐要求进行填充（以便数组元素对齐）。
```c
struct Example {
    char a;      // 1字节
    int b;       // 4字节
    short c;     // 2字节
};
```
每个结构体成员的起始地址为自己sizeof的整数倍
a 占1字节，后面会填充3字节，使 b 按4字节对齐。
b 占4字节。
c 占2字节，后面会填充2字节，使整个结构体按4字节对齐。
如果使用 #pragma pack(1) 或 __attribute__((packed)) 禁用对齐，内存大小为7字节

### 类型长度问题
标准C中，16位int 占2个字节，32位占4个字节
方案：使用int32_t, int8_t,int16_t

## htons/ntohs函数
htons: 将 16 位数据从主机字节序转换为网络字节序。
ntohs: 将 16 位数据从网络字节序转换为主机字节序。

```c
#include <arpa/inet.h>
uint16_t htons(uint16_t hostshort);
uint16_t ntohs(uint16_t netshort);
```
htons 和 ntohs 仅用于 16 位数据。对于 32 位数据（如 IP 地址），请使用 htonl 和 ntohl。

### socket函数
套接字（Socket）是网络通信的基础，它是一种抽象概念，为不同设备之间的数据传输提供了统一的编程接口。通过套接字，应用程序可以在同一台主机或者不同的主机之间发送和接收数据。

简单来说，套接字是网络通信的端点，负责：

标识通信的参与方（IP 地址和端口号）。
提供通信的协议支持（如 TCP 或 UDP）。

socket提供了**stream** **datagram** 两种通信机制，即流socket和数据包socket
**流socket**基于TCP协议，是一个有序、可靠、双向字节刘的通道，传输数据不会丢失、不会重复、顺序也不会错乱

**数据报socket**基于UDP协议，不需要建立和尉迟连接，可能会丢失或错乱。UDP不是一个可靠的协议，对数据的长度有限制，但是效率较高
```c
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
/*
domain (地址域/协议族)
指定套接字使用的协议家族（即通信的“领域”）。
常见值：
AF_INET：IPv4 互联网协议。
AF_INET6：IPv6 互联网协议。
AF_UNIX 或 AF_LOCAL：本地套接字（同一主机内进程间通信）。
其他值如 AF_PACKET（底层网络接口通信）等。

type (套接字类型)
指定套接字的类型，定义通信的语义。
常见值：
SOCK_STREAM：面向连接的流式套接字（通常用于 TCP）。
SOCK_DGRAM：无连接的报文套接字（通常用于 UDP）。
SOCK_RAW：原始套接字，用于发送和接收底层数据包。

protocol (协议)
指定要使用的具体协议。
常见值：
0：通常表示默认协议。例如，SOCK_STREAM 默认使用 TCP，SOCK_DGRAM 默认使用 UDP。
明确的协议编号可以从 /etc/protocols 文件中查找。
*/
```

被动端：
取得Socket
给socket取得地址
收/发消息
关闭socket

主动端：
取得socket
给socket取得地址(可省略)
发/收消息
关闭Socket
## 静态数据包套接字
### bind()
bind() 函数用于将一个套接字与一个特定的地址（IP 地址）和端口号绑定。
```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
/*
sockfd
套接字文件描述符，由 socket() 函数返回，用于标识要绑定的套接字。

addr
一个指向 struct sockaddr 结构的指针，指定要绑定的地址和端口号。
实际上，通常会传递一个更具体的地址结构（如 struct sockaddr_in 或 struct sockaddr_in6）的指针。

addrlen
地址结构的大小（以字节为单位）。
例如，对于 IPv4 地址，传递 sizeof(struct sockaddr_in)；对于 IPv6 地址，传递 sizeof(struct sockaddr_in6)。

返回值：
成功：返回 0。
失败：返回 -1，并设置 errno 指示错误原因。
*/
```
```c
struct sockaddr {
    sa_family_t sa_family;   // 地址族，例如 AF_INET（IPv4），AF_INET6（IPv6）
    char sa_data[14];        // 地址和端口信息，具体含义由地址族决定
};
```
16字节
sockaddr 本身是通用的，但在实际使用中，需要用特定的子类结构来存储地址和端口信息。这些子类会被强制类型转换为 struct sockaddr *，以便传递给相关函数
```c
struct sockaddr_in (IPv4 地址)
用于存储 IPv4 地址信息。
#include <netinet/in.h>
struct sockaddr_in {
    sa_family_t    sin_family;  // 地址族：AF_INET（IPv4）
    in_port_t      sin_port;    // 端口号（网络字节序）
    struct in_addr sin_addr;    // IP 地址（网络字节序）
    char           sin_zero[8]; // 填充字段，保持与 sockaddr 大小一致
};
sin_family: 必须设为 AF_INET。
sin_port: 使用 htons() 将主机字节序转换为网络字节序。
sin_addr: 使用 inet_pton() 或直接设置为 INADDR_ANY（绑定到所有可用 IP）。

2. struct sockaddr_in6 (IPv6 地址)
用于存储 IPv6 地址信息。
#include <netinet/in.h>
struct sockaddr_in6 {
    sa_family_t     sin6_family;   // 地址族：AF_INET6（IPv6）
    in_port_t       sin6_port;     // 端口号（网络字节序）
    uint32_t        sin6_flowinfo; // 流量类别和流标签
    struct in6_addr sin6_addr;     // IPv6 地址
    uint32_t        sin6_scope_id; // 作用域 ID（如接口索引）
};
sin6_family: 必须设为 AF_INET6。
sin6_port: 与 IPv4 一样，使用 htons() 设置端口号。
sin6_addr: 使用 inet_pton() 设置具体的 IPv6 地址。
sin6_scope_id: 在本地链接地址中，指定网络接口（例如 eth0 的索引）。
```

### recvfrom()
```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/*
int sockfd
描述：用于接收数据的套接字文件描述符。
要求：必须是一个已经创建的、绑定到本地地址的套接字（通常是通过 socket() 和 bind() 创建和绑定的）。

void *buf
描述：指向接收缓冲区的指针，用于存储接收到的数据。
要求：必须有足够的内存来容纳接收的数据。

size_t len
描述：接收缓冲区的大小，指定可以接收的最大字节数。
建议：应确保缓冲区大小足够大，以避免数据截断。

int flags
描述：指定接收行为的标志，可以是以下值的组合：
0：普通接收。
MSG_PEEK：窥探数据，但不移除数据（数据仍留在缓冲区中）。
MSG_WAITALL：等待接收完整的数据长度（可能会阻塞）。
MSG_DONTWAIT：非阻塞接收。
MSG_TRUNC：如果数据超出缓冲区大小，不截断并返回完整长度。

struct sockaddr *src_addr
描述：指向存储发送方地址的结构（通常是 struct sockaddr_in 或 struct sockaddr_in6 的指针）。
用法：可以是 NULL，如果不需要获取发送方地址信息。

socklen_t *addrlen
描述：src_addr 的地址结构大小的指针。
用法：必须初始化为 src_addr 的实际大小，调用后会更新为接收的地址大小。
可以为 NULL：如果 src_addr 为 NULL，此参数可忽略。
*/
```
netstat -anu 查看使用udp协议的端口
 
 ### recvfrom
 ```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
/*
int sockfd
描述：套接字文件描述符，标识一个已创建的套接字。
要求：必须通过 socket() 创建，且通常已通过 bind() 绑定本地地址和端口。

void *buf
描述：指向接收缓冲区的指针，用于存储接收到的数据。
大小：由参数 len 指定的最大字节数。

size_t len
描述：接收缓冲区的大小，指定可以接收的最大字节数
注意：如果接收到的数据超过缓冲区大小，数据会被截断，但返回值会指示完整消息长度。

int flags
描述：接收数据的行为控制标志。常用值包括：
0：普通接收。
MSG_PEEK：窥探数据，但不从套接字缓冲区移除数据。
MSG_WAITALL：等待接收完整的数据长度。
MSG_DONTWAIT：非阻塞模式。
MSG_TRUNC：返回消息的完整长度，即使缓冲区太小。

struct sockaddr *src_addr
描述：指向存储发送方地址的结构（如 sockaddr_in 或 sockaddr_in6）。
功能：如果不需要获取发送方地址，可以设置为 NULL。

socklen_t *addrlen
描述：指向地址长度的变量。
要求：调用前必须初始化为 src_addr 指向的结构的大小，调用后会更新为接收的地址实际大小。
可以为 NULL：如果 src_addr 为 NULL，此参数也可忽略。
*/
 ```

 ### sendto()
 sendto 是一个用于发送数据到指定目的地的系统调用，常用于无连接协议（如 UDP）。与 send 函数不同，sendto 允许在每次调用时指定目标地址和端口，无需事先建立连接。
 ```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
/*
int sockfd
描述：套接字文件描述符，标识一个通过 socket() 创建的套接字。
要求：
对于 UDP 等无连接协议，可直接使用。
对于 TCP 等面向连接协议，通常需要在 connect() 之后使用（尽管更常用 send）。

const void *buf
描述：指向待发送数据的缓冲区。
注意：缓冲区应包含需要发送的数据。

size_t len
描述：待发送数据的长度（字节数）。
限制：
受操作系统和网络协议限制，单次发送的数据大小可能受限。

int flags
描述：发送数据的行为控制标志，常用值包括：
0：普通发送。
MSG_DONTWAIT：非阻塞发送。
MSG_CONFIRM：确认连接仍然可达（主要用于 UDP）。

const struct sockaddr *dest_addr
描述：指向目标地址结构的指针，用于指定数据发送的目的地。
要求：
必须根据套接字的协议族（如 AF_INET 或 AF_INET6）初始化合适的结构（如 sockaddr_in 或 sockaddr_in6）。
如果已用 connect() 指定目标地址，可设置为 NULL。

socklen_t addrlen
描述：目标地址结构的大小（字节数）。
要求：需与 dest_addr 对应的地址结构类型匹配，例如 sizeof(struct sockaddr_in)。

返回值
成功：
返回实际发送的字节数（可能小于 len，需检查返回值以确认是否需要重新发送）。
失败：
返回 -1，并设置 errno 表示错误类型：
EACCES：权限不足（如使用广播地址时未设置 SO_BROADCAST）。
EAGAIN 或 EWOULDBLOCK：非阻塞模式下无法发送。
ENETDOWN：网络不可用。
EFAULT：缓冲区指针无效。
EINVAL：参数无效。
*/
 ```
 ## 