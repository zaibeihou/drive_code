#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static void	platform_device_release(struct device *dev){
    printk("this is latform_device_release\n"); 
};

static struct resource platform_resource[] = {
    [0] = {
        .start = 0xFDD60000,
        .end   = 0xFDD60004,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = 13,
        .end = 13,
        .flags = IORESOURCE_IRQ,
    },
};

struct platform_device platform_device = {
    .name = "platform_device_test",
    .id = -1,
    .resource = platform_resource,
    .num_resources = ARRAY_SIZE(platform_resource),
    .dev = {
        .release = platform_device_release,
    }
};


static int __init platform_device_init(void)
{
    // 使用带有日志级别的 printk
    printk("platform_device_init\n");   // KERN_ALERT 确保日志会被显示
    platform_device_register(&platform_device);
    return 0;
}

static void __exit platform_device_exit(void)      // 驱动出口函数
{
    printk("platform_device_exit\n");
    platform_device_unregister(&platform_device);
}



module_init(platform_device_init);
module_exit(platform_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("why");