#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>//socket connect
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "weather.h"
char * get_weather(char * city_adcode) 
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
    server_addr.sin_addr.s_addr = inet_addr("120.77.134.169");//高德天气的IP地址
    int flag = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(flag == -1)
    {
        printf("connect error\n");
    }
    printf("connect success\n");
    
    /*http报文

    GET https://restapi.amap.com/v3/weather/weatherInfo?parameters HTTP/1.1\r\n
    Host:restapi.amap.com\r\n
    \r\n

    */
    char temp_str[500];
    sprintf(temp_str, "GET https://restapi.amap.com/v3/weather/weatherInfo?key=0649080a12f9a64eaaf69340fb8a5374&city=%s HTTP/1.1\r\nHost:restapi.amap.com\r\n\r\n", city_adcode);
    //发送Http报文
    write(socket_fd, temp_str, strlen(temp_str));
    //接收服务器返回的数据
    char recv_data[1024] = {0};
    read(socket_fd, recv_data, sizeof(recv_data));
    printf("%s\n", recv_data);

    //保存省份的数组
    char province[20] = {0};
    //保存城市的数组
    char city[20] = {0};
    //保存adcode的数组
    char adcode[20] = {0};
    //保存天气的数组
    char weather[20] = {0};
    //标记引号
    int num = 0; 
    //遍历专门的数据
    int j = 0;
    //遍历数据
    for(int i = 0; i < strlen(recv_data); i++)
    {
        if(recv_data[i] == '\"')
        {
            num ++;
            j = 0;
            continue;
        }
        if(num == 21)
        {
            province[j++] = recv_data[i];
        }
        if(num == 25)
        {
            city[j++] = recv_data[i];
        }
        if(num == 29)
        {
            adcode[j++] = recv_data[i];
        }
        if(num == 33)
        {
            weather[j++] = recv_data[i];
        }
    }
    printf("省份：%s\n", province);
    printf("城市：%s\n", city);
    printf("adcode：%s\n", adcode);
    printf("天气：%s\n", weather);
    static char temp_str2[500];
    sprintf(temp_str2, "%s\n%s\n%s\n", province, city, weather);
    //关闭套接字
    close(socket_fd);
    return temp_str2;
   
}