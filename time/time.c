#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>//socket connect
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "time.h"
#include "../lvgl/src/core/lv_obj.h"
#include "../lvgl/src/widgets/lv_label.h"

void *routing_time(void* arg)
{
    //创建套接字
    int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd == -1)
    {
        printf("socket error\n");
    }
    printf("socket success\n");
    //连接服务器http
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;//IPV4协议
    server_addr.sin_port = htons(80); //http默认端口80
    server_addr.sin_addr.s_addr = inet_addr("101.201.210.218");//时间的IP地址
    int flag = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(flag == -1)
    {
        printf("connect error\n");
    }
    printf("connect success\n");
    
    /*http报文

    https://timezone.market.alicloudapi.com/timezone


    %E5%8D%97%E6%98%8C  南昌的URL编码
    请求报文
    GET https://timezone.market.alicloudapi.com/timezone?city=%E5%8D%97%E6%98%8C HTTP/1.1\r\n
    Host:timezone.market.alicloudapi.com\r\n
    Authorization:APPCODE 7bdc2765fd2f457fb864410085783ed6\r\n
    \r\n


    */
    char * http_str = "GET https://timezone.market.alicloudapi.com/timezone?city=%E5%8D%97%E6%98%8C HTTP/1.1\r\nHost:timezone.market.alicloudapi.com\r\nAuthorization:APPCODE 7bdc2765fd2f457fb864410085783ed6\r\n\r\n";
    //发送Http报文
    write(socket_fd, http_str, strlen(http_str));
    //接收服务器返回的数据
    char recv_data[1024] = {0};
    read(socket_fd, recv_data, sizeof(recv_data));
    printf("%s\n", recv_data);

    char time[20] = {0};
    //记录引号
    int num = 0;
    int j = 0;
    for(int i = 0; i < strlen(recv_data); i++)
    {
        if(recv_data[i] == '\"')
        {
            num ++;
            continue;
        }
        if(num == 15)
        {
            time[j++] = recv_data[i];
        }
    }
    printf("time:%s\n", time);
    //将字符串转换为时间
    int year, month, day, hour, minute, second = 0;
    
    sscanf(time, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

    printf("year:%d, month:%d, day:%d, hour:%d, minute:%d, second:%d\n", year, month, day, hour, minute, second);


    while(1)
    {
        sleep(1);
        second++;
        if(second == 60)
        {
            minute++;
            second = 0;
        }
        if(minute == 60)
        {
            hour++;
            minute = 0;
        }
        //printf("year:%d, month:%d, day:%d, hour:%d, minute:%d, second:%d\n", year, month, day, hour, minute, second);
        char time_str[20] = {0};
        sprintf(time_str, "%d-%d-%d\n%d:%d:%d", year, month, day, hour, minute, second);
        printf("time_str:%s\n",time_str);
        lv_label_set_text((lv_obj_t *)arg, time_str); //更新时间标签
    
    }


    //关闭套接字
    close(socket_fd);

}