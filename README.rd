	在一个非阻塞的socket上调用read/write，返回EAGAIN或者EWOULDBLOCK。意思是:EAGAIN: 再试一次，EWOULDBLOCK: 如果这是一个阻塞socket, 操作将被block，perror输出: Resource temporarily unavailable。
原因：
	这个错误表示资源暂时不够，能read时，读缓冲区没有数据，或者write时，写缓冲区满了。遇到这种情况，如果是阻塞socket，read/write就要阻塞掉。而如果是非阻塞socket，read/write立即返回-1， 同时errno设置为EAGAIN。所以，对于阻塞socket，read/write返回-1代表网络出错了。但对于非阻塞socket，read/write返回-1不一定网络真的出错了。可能是Resource temporarily unavailable。这时你应该再试，直到Resource available。
所以：
	对于non-blocking的socket，正确的读写操作为：
		读：忽略掉errno=EAGAIN错误，下次继续读
		写：忽略掉errno=EAGAIN错误，下次继续写
	对于select和epoll的LT模式，这种读写方式是没有问题的。但对于epoll的ET模式，这种方式还有漏洞。。

ET和LT的差异在于LT模式下只要某个socket处于readable/writeable状态，无论什么时候进行epoll_wait都会返回该socket；而ET模式下只有某个socket从unreadable或重unwritable变为writable时，epoll_wait才会返回该socket。
ET:
	socket接收缓冲区不为空 有数据可读 读事件一直触发
	socket发送缓冲区不满 可以继续写入数据 写事件一直触发	
LT:
	socket的接收缓冲区状态变化时触发读事件，即空的接收缓冲区刚接收到数据时触发读事件
	socket的发送缓冲区状态变化时触发写事件，即满的缓冲区刚空出空间时触发读事件
