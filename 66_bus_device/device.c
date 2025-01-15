#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关
#include <linux/device.h>      // 设备相关
#include <linux/sysfs.h>       


extern struct bus_type my_bus;

void my_release(struct device *dev){
    printk("my_release\n");
};

struct device device = {
    .init_name = "my_device",
    .release = my_release,
    .bus = &my_bus,
    .devt = ((255 << 20) | 0),
};   

// 模块初始化函数
static int __init device_init(void) {
    int ret;//定义一个返回值
    ret = device_register(&device);//注册一个设备
    if(ret){
        printk("device_register failed\n"); 
    }
    return ret;
}

// 模块卸载函数
static void __exit device_exit(void) {
    device_unregister(&device);//注销一个设备
}

// 指定模块的入口和出口函数
module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");