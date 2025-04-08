## ROS分布式远程启动
环境/平台：
虚拟机ubuntu20.04 + ros1
jetson控机ubuntu18.04 + ros1
### ROS多机通讯
ROS本身支持分布式运行，在ROS1中多个设备可以通过共享一个roscore进行ros式的topic通讯，当然需要一定的配置：
    * 所有端口上的所有机器之间必须有完整的双向连接(双向ping通)。
    * 每台计算机必须通过所有其他计算机都可以解析的名称来公告自己。
#### 配置
1. 要保证不同计算机处于同一网络中，最好分别设置固定IP，如果为虚拟机，需要将网络适配器改为桥接模式。
    查看IP:`ifconfig`
    ![alt text](pic/ros远程通讯(1).png)
    虚拟机IP为192.168.97.190，作为ROS从机
    同样方式查看工控机IP为192.168.97.102作为ROS主机

    ROS从机中终端输入:
    `export ROS_MASTER_URI=http://192.168.97.237:11311`
    `export ROS_HOSTNAME=192.168.97.190`

    将ROSCORE的配置设在ROS主机中；
    ROS主机终端输入：
    `export ROS_MASTER_URI=http://192.168.97.102:11311`
    `export ROS_HOSTNAME=192.168.97.102`
    或者写入~/.bashrc终端自动载入
2. 测试
    ROS从机输入roscore，显示master不在本机则配置成功；
    ROS主机运行节点后，从机`rosnode list`可查则配置成功；

#### ROS远程启动节点
在上面的基础上，配置roslaunch mashine关键字的使用，mashine用于声明远程计算机的 SSH 和 ROS 环境变量设置，可以通过SSH远程执行roslaunch。
#### 配置SSH
SSH配置，ubuntu系统中，安装ssh服务：
`sudo apt-get install openssh-server`
成功安装后启动：
`sudo systemctl enable --now ssh`
查询状态：
`sudo systemctl status ssh`
如果状态为activating则服务启动。
#### ssh连接
确保多台机器在同一局域网并且相互ping通，新终端执行：
`ssh username@ipaddress`
username可通过在终端中查看：在ubuntu中终端为username@hostname；
或者直接终端输入hostname查询；
如果运行正常：输出类似:
![alt text](pic/ros远程通讯(2).png)
如果出错：
permission denied,检查username是否正确，检查ssh config
无法连接，检查ssh是否启动，检查ip地址和是否ping通；

双向ssh连接成功则ssh配置成功
#### roslaunch 配置
ssh配置成功后尝试配置roslaunch machine关键字，
ROS从机中创建Launch文件
![alt text](pic/ros远程通讯(3).png)
配置mashine关键字:
name: 自定义的机器人名称
password: 用户密码
address:ip地址
env-loader:要加载的环境脚本

node配置和启动节点的配置大致相同，加入mashine关键字就好。

环境脚本：
![alt text](pic/ros远程通讯(4).png)
注：以#!/bin/bash开头，exec"$@"结尾

在ros从机中运行launch文件，正常运行如：
![alt text](pic/ros远程通讯(5).png)
在主机中可以看懂节点正常启动：
![alt text](pic/ros远程通讯(6).png)
如果正常运行，那么就可以到此为止了。
但实际情况往往不是如此，经历各种报错，
![alt text](pic/ros远程通讯(9).png)
查询资料：ros所用的ssh客户端只能识别rsa算法生成的hostkey，所以之前使用ssh而保存的对于ros来说都是无效的。
于是重新配置ssh

#### 重新配置ssh-rsa免密登录
1. 删除原来的known_hosts文件，rm ~/.ssh/known_hosts
2. 配置rsa的时候顺带配置了免密登录，
![alt text](pic/ros远程通讯(7).png)
使用`ssh-keygen`获取rsa key pair，全部选择默认设置
确保ssh仍然可用，使用`scp ~/.ssh/id_rsa.pub username@ipaddr:/home/username`
![alt text](pic/ros远程通讯(8).png)
3. 将公钥文件id_rsa.pub拷贝到远程计算机后，SSH登录到远程计算机：
`ssh username@ip_address`
4. 将id_rsa.pub的文件内容追加写入到远程计算机的~/.ssh/authorized_keys文件中，并修改authorized_keys文件的权限：
 `cat id_rsa.pub >> ~/.ssh/authorized_keys`
`chmod 600 ~/.ssh/authorized_keys`
之后完成rsa配置并且退出登录(exit)就无需密码。

重新执行launch:
![alt text](pic/ros远程通讯(10).png)
修改主机环境脚本权限：`chmod +x <sh文件>`
再次执行成功。




