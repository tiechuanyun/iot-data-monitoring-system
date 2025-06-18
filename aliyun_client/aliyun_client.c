#include<sys/types.h>
#include<sys/socket.h> //socket函数头文件

#include <stdio.h>//ptintf函数头文件

#include<netinet/in.h>//socketaddr_in结构体头文件

#include<arpa/inet.h>//inet_addr函数的

#include "unistd.h"//sleep函数头文件

#include "aliyun_client.h"

void * routing_aliyun(void * arg)
{
    /*1.创建套接字   AF_INET：IPV4协议， SOCK_STREAM：TCP通信，通常0*/
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1)
    {
        /*套接字创建失败*/
        printf("sokect FAIL \n");
        /*结束程序*/
        return NULL;
    }
    printf("sokect SUCCES \n");
    /*2.绑定*/
    struct sockaddr_in myaddr;//myaddr保存服务器的IP与端口

    myaddr.sin_family = AF_INET;//ipv4协议
    myaddr.sin_port = htons(8888);//端口8080，htons函数：将数据转成网络字节序
    myaddr.sin_addr.s_addr = inet_addr("8.138.223.127");/*本机IP地址，0.0.0.0监听所有端口，
                                            inet_addr函数：将点分十进制转成32二进制
                                            */
    int state =  connect(socket_fd,  (struct sockaddr *)&myaddr, sizeof(myaddr));
    if(state == -1)
    {
        /*放回-1绑定失败*/
        perror("bind");
        printf("connect FAIL \n");
        /*程序结束*/
        return NULL;
    }
    printf("connect succes\n");
    while(1)
    {
        send(socket_fd, "wu yi da xue", 12, 0);
        sleep(1);
    }
    
}