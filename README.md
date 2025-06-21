### 简介
 :earth_africa: 
该仓库代码实现：本地温湿度的显示、实时时间的显示、实时天气的显示，并将数据上传到阿里云ECS服务器。

### lvgl源码
LVGL仓库链接：https://github.com/lvgl
搜索linux后，使用的是lv_port_linux_v8.4

### 使用方法
这一份代码在linux中直接make -j 后，会产生可执行的demo文件，将demo文件传输到开发板上执行即可。

（适用于粤嵌的GEC6818开发板，当然其他的它也可以）
触摸屏文件：/dev/input/event0  坐标范围 1024---600
LCD文件：/dev/fb0 像素范围 800---600
### 注意事项
代码中的高德天气API和实时时间API的密钥KEY都需要自己去更改。