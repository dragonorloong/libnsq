# 
  nsq c++版本客户端，第三方库使用libevent和jsoncpp
#
  download下来以后直接运行build.sh就行，nsq_test目录是nsq客户端使用的demo
# 
  目前消费者读取消息以后直接返回FIN，可能会造成数据丢失
#
  生产者在push数据的时候，没有连接直接返回-1,缓存机制依赖于libevent的发送缓存，里面上缓存无限大，所以程序core掉的时候丢失很多
#
  记得安装libtool和automake，编译通不过一般是三方库没有安装好的原因
#
  嗯，本来只想简简单单写个客户端，没想到写成了框架，虽然同事在笑话我机械时代还学着打铁，但是我决定给启动函数取个很嚣张的名字，就叫StartUniverseInvincibleSuperServer了

# 后续我会加上作为服务器的demo，并且附上压测结果，很多地方还有待改进......

