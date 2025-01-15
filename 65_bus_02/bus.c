#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关
#include <linux/device.h>      // 设备相关
#include <linux/sysfs.h>       

int my_bus_match(struct device *dev, struct device_driver *drv) {
    
    return ( strcmp(drv->name,dev_name(dev)) == 0 );
}

int my_bus_probe(struct device *dev){
    struct device_driver *driver = dev->driver;
    printk("my_bus_probe\n");
    if(driver->probe){
        driver->probe(dev);
    }
    return 0;
}

struct bus_type my_bus = {
    .name = "my_bus",
    .match = my_bus_match,
    .probe = my_bus_probe,
};
 
ssize_t bus_attr_show(struct bus_type *bus, char *buf){
    return sprintf(buf, "my_bus_attr_show\n");
}

ssize_t bus_attr_store(struct bus_type *bus, const char *buf, size_t count){
    printk("my_bus_attr_store\n");
    return count;
}

struct bus_attribute bus_attr = {
    .attr = {
        .name = "my_bus_attr",
        .mode = 0664,
    },
    .show = bus_attr_show,
    .store = bus_attr_store,
};


// 模块初始化函数
static int __init bus_init(void) {
    int ret;//定义一个返回值
    ret = bus_register(&my_bus);
    if(ret){
        printk("bus_register failed\n");
        return ret;
    }
    bus_create_file(&my_bus, &bus_attr);
    return 0;
}

// 模块卸载函数
static void __exit bus_exit(void) {
    bus_remove_file(&my_bus, &bus_attr);
    bus_unregister(&my_bus);
}

// 指定模块的入口和出口函数
module_init(bus_init);
module_exit(bus_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");