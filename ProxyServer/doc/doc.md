# 文档

## 0. 基础

参考: https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application

---

需要包含的两个头文件

* The `Winsock2.h` header file contains most of the Winsock functions, structures, and definitions. The `Winsock2.h` header file internally includes core elements from the `Windows.h` header file, so there is no need for to include `Windows.h` in Winsock applications
* The `Ws2tcpip.h` header file contains definitions introduced in the WinSock 2 Protocol-Specific Annex document for TCP/IP that includes newer functions and structures used to retrieve IP addresses.

---
需要引入`Ws2_32.lib`链接库

使用`#pragma comment(lib, "Ws2_32.lib")`

> 关于`pragma`的更多用法, 参考: https://docs.microsoft.com/en-us/cpp/preprocessor/comment-c-cpp?view=vs-2019

---

## 1. 函数

- `int WSAStartup(WORD wVersionRequired, LPWSADATA lpWSAData);`

    参考:https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup
    
    用来启动一个windows socket程序. 

    用法: 

    ```C
    WSADATA wsaData;

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    ```

    `WSADATA`存储了windows运行socks程序需要的信息. 

- `int WSACleanup();`

    程序结束后用来清理, 释放socket资源.

- `int getaddrinfo(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult);`

    `getaddrinfo`是一个封装程度很高的函数, 封装了完整的host和服务地址转换的功能. 

- 

## 2. 流程

![](procedure.png)

1. Server端创建socket: https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-server
2. socket绑定地址和端口: https://docs.microsoft.com/en-us/windows/win32/winsock/binding-a-socket
3. 监听socket: https://docs.microsoft.com/en-us/windows/win32/winsock/listening-on-a-socket
4. server端接收socket连接: https://docs.microsoft.com/en-us/windows/win32/winsock/accepting-a-connection
5. 