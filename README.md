# 
  nsq c++版本客户端，第三方库使用libevent和jsoncpp
#
  download以后直接运行build.sh就行，nsq_test目录是nsq客户端使用的demo
# 
  目前消费者读取消息以后直接返回FIN，可能会造成数据丢失
#
  生产者在push数据的时候，没有连接直接返回-1,缓存机制依赖于libevent的发送缓存，理论上缓存无限大，所以程序core掉的时候丢失很多
#
  记得安装libtool和automake，编译通不过一般是三方库没有安装好的原因
#
 恩, 我决定给启动函数取个很嚣张的名字，就叫StartUniverseInvincibleSuperServer了

#
 server_test下面是关于作为服务器的压力测试结果，客户端短连接用ab，长连接用tcptest里面的golang
# 
  16G 内存，网卡1000M，cpu:Intel(R) Xeon(R) CPU E5-2603 0 @ 1.80GHz

  用ab压力测试，两个客户端每个2万并发，4万短连接，每个请求200个字节，内存几百兆，忘记记录了，cpu 利用率300%  qps8000  响应时间等于3s，没有条件往上压了，往上压的话瓶颈应该在cpu

  12万长连接，每个连接30s发一个200个字节的包，内存占用大概1G，cpu利用率72%，往上压内存和cpu都是瓶颈

#
 总的来说,锁用得太多，内存拷贝太多，导致系统调用很多，vmstat的in和cs值很大,还有很多待改进......

