# 文档

## 背景

1. 在本实验中，接受数据使用`recvfrom`，而上一个代理服务器的实验使用了`recv`。二者的不同：

- `recv`既可以用在面向连接，也可以用在无连接。`recv`会仅接受对应地址的信息. 对应地址通过`bind`或者`accept`指定。`recv`的socket必须已经建立连接. 因此`recv`一般用在TCP连接中, 因为TCP连接建立后地址是固定的, 在`recv`的参数里也没有地址.
- `recvfrom`既可以用在已经建立的socket,也可以用在还没有连接的socket. `recvfrom`一般是用在无连接类型的socket中. 一般是对指定端口全部的地址进行接收. `recvfrom`一般用在UDP中, 可以方便地获得地址并进行回复. 微软关于[recvfrom](https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-recvfrom)的文档里给出了关于`recvfrom`的用例, 即从一个端口接受全部信息.

    `send`和`sendto`也具有类似区别. 


2. `ioctlsocket`用来设置socket的状态，有更强大的版本`WSAIoctl`。参考微软的[文档](https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-ioctlsocket)

3. 阻塞模式与非阻塞模式

   - 阻塞模式：等待对方的报文，例如`accept()`方法将会阻塞服务器线程直到一个呼叫到来.
   - 非阻塞模式：执行此套接字的网络调用时，不管是否执行成功，都立即返回。比如调用`recv()`函数读取网络缓冲区中数据，不管是否读到数据都立即返回，而不会一直挂在此函数调用上。


## 说明

GBN_client和GBN_server需要分开运行，因此这里开了两个项目。分别编译运行。

