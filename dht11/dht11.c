#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "dht11.h"
#include "../lvgl/src/core/lv_obj.h"
#include "../lvgl/src/widgets/lv_label.h"

#define GEC6818_GET_DHTDATA  _IOR('K', 0, unsigned int) //注册宏

void * routing_dht11(void *arg)
{
	int ret, cnt_yes=0, cnt_err=0;
	unsigned char data[2];
	int ultrasonic_fd = open("/dev/dht11_dev", O_RDWR);
	if(ultrasonic_fd < 0)
	{
		perror("open dht11_dev driver");
		return NULL;
	}				
	while(1)
	{
		printf("检测中\n");
		ret = ioctl(ultrasonic_fd, GEC6818_GET_DHTDATA, &data[0]);
		if(ret != 0)
		{
			cnt_err++;
			perror("GEC6818_GET_DHTDATA error");
		}
		else
		{
			cnt_yes++;
			printf("湿度= %hhd  温度=%hhd  错误%d  正确%d\n", data[1], data[0], cnt_err, cnt_yes);
		}
		usleep(1000*2000);
        char dht11_str[20] = {0};
        sprintf(dht11_str, "%hhd°C %hhd%%RH", data[0], data[1]);
        printf("dht11_str:%s\n",dht11_str);
        lv_label_set_text((lv_obj_t *)arg, dht11_str); //更新时间标签
	}
	close(ultrasonic_fd);
	
	return 0;
}