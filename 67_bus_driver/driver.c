#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关
#include <linux/device.h>      // 设备相关
#include <linux/sysfs.h>       

extern struct bus_type my_bus;

static int my_probe(struct device *dev) {
    printk("Driver probed\n");
    return 0;
}

static int my_remove(struct device *dev) {
    printk("Driver removed\n");
    return 0;
}

struct device_driver my_driver = {
    .name = "my_device",
    .bus = &my_bus,
    .probe = my_probe,
    .remove = my_remove,
};

// 模块初始化函数
static int __init mydriver_init(void) {
    int ret;
    ret = driver_register(&my_driver); // 注册驱动
    if (ret) {
        printk("driver_register failed\n");
    }
    return ret;
}

// 模块卸载函数
static void __exit mydriver_exit(void) {
    driver_unregister(&my_driver); // 注销驱动
}

// 指定模块的入口和出口函数
module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");