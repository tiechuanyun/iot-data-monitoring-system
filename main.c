#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "weather/weather.h"
#include "time/time.h"
#include <pthread.h> //pthread_create
#include <unistd.h>
#include "dht11/dht11.h"
#include "aliyun_client/aliyun_client.h"

#define DISP_BUF_SIZE (128 * 1024)

//剑阁县的adcode
//#define  ADCODE "510823" 
//韶关市的adcode
#define  ADCODE "440200"



int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 800;
    disp_drv.ver_res    = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);


    /*Set a cursor for the mouse*/
    LV_IMG_DECLARE(mouse_cursor_icon)
    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/


    /*Create a Demo*/
    //lv_demo_widgets();
    char weather[100];
    //获取天气
    strcpy(weather, get_weather(ADCODE));
    //strcpy(weather, "晴天");
    printf("weather:%s", weather);
    
    LV_IMG_DECLARE(sunny);
    lv_obj_t * lv_weather_img = lv_img_create(lv_scr_act());
    lv_img_set_src(lv_weather_img, &sunny);
    lv_obj_move_background(lv_weather_img);


    //主界面背景
    lv_obj_t * obj = lv_obj_create(lv_scr_act()); 
    lv_obj_set_size(obj, 800, 480);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0xADD8E6), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(obj, 100, LV_STATE_DEFAULT);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);

    LV_FONT_DECLARE(Font60) /* 声明字体 */ 
    lv_obj_t * weather_lable = lv_label_create(obj);
    lv_obj_align(weather_lable, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(weather_lable, &Font60, LV_STATE_DEFAULT); 
    lv_label_set_text(weather_lable, "物联网IOT数据监测平台");

    LV_FONT_DECLARE(Font) /* 声明字体 */ 
    //天气框
    lv_obj_t * obj1 = lv_obj_create(obj); 
    lv_obj_set_size(obj1, 200, 200);
    lv_obj_align(obj1, LV_ALIGN_CENTER, 250, 20);
    lv_obj_set_style_bg_color(obj1, lv_color_hex(0xF0F8FF), LV_STATE_DEFAULT);

    //天气文本
    lv_obj_t * weather_lable1 = lv_label_create(obj1);
    lv_obj_align(weather_lable1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(weather_lable1, &Font, LV_STATE_DEFAULT);
    lv_label_set_text(weather_lable1, weather);

    //温湿度框
    lv_obj_t * obj2 = lv_obj_create(obj); 
    lv_obj_set_size(obj2, 200, 200);
    lv_obj_align(obj2, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_color(obj2, lv_color_hex(0xF0F8FF), LV_STATE_DEFAULT);

    //温湿度文本
    lv_obj_t * DHT11_lable1 = lv_label_create(obj2);
    lv_obj_align(DHT11_lable1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(DHT11_lable1, &Font, LV_STATE_DEFAULT);
    //lv_label_set_text(DHT11_lable1, "60°C\n80%RH");

    //实时时间框
    lv_obj_t * obj3 = lv_obj_create(obj); 
    lv_obj_set_size(obj3, 200, 200);
    lv_obj_align(obj3, LV_ALIGN_CENTER, -250, 20);
    lv_obj_set_style_bg_color(obj3, lv_color_hex(0xF0F8FF), LV_STATE_DEFAULT);

    //实时时间文本
    lv_obj_t * time_lable = lv_label_create(obj3);
    lv_obj_align(time_lable, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(time_lable, &Font, LV_STATE_DEFAULT);
    //lv_label_set_text(time_lable, "2022-12-12\n12:12:12");
    
    


    //创建时间现场
    pthread_t time_id;
    int pf = pthread_create(&time_id, NULL, routing_time, (void *)time_lable);//(线程id, 默认线程属性, 线程函数, 不传递参数给线程函数)
    if(pf != 0)
    {
        //线程创建失败
        printf("pthread_create error\n");
    } 

    //创建温湿度现场
    pthread_t DHT11_id;
    int pf2 = pthread_create(&DHT11_id, NULL, routing_dht11, (void *)DHT11_lable1);//(线程id, 默认线程属性, 线程函数, 不传递参数给线程函数)
    if(pf2 != 0)
    {
        //线程创建失败
        printf("pthread_create error\n");
    } 

    //创建阿里云现场
    pthread_t aliyun_id;
    int pf3 = pthread_create(&aliyun_id, NULL, routing_aliyun, NULL);//(线程id, 默认线程属性, 线程函数, 不传递参数给线程函数)
    if(pf3 != 0)
    {
        //线程创建失败
        printf("pthread_create error\n");
    } 

    

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
