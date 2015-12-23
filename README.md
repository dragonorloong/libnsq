# 
  nsq c++版本客户端，第三方库使用libevent和jsoncpp
#
  目前在build.sh里面直接从github上面下载libevent和jsoncpp的Dev分支，假如线上使用需要额外部署Release版本
# 
  main.cpp中有关于生产者和消费者的使用，目前读取到消费者以后直接返回FIN，可能会造成数据丢失
  生产者在push数据的时候，没有连接直接返回-1,没有缓存机制
  我决定给启动函数取个很嚣张的名字，就叫了StartSuperServer
