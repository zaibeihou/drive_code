#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>

static int my_probe(struct platform_device * dev){
    printk("my_probe\n");
    return 0;
}

static int my_remove(struct platform_device *dev){
    printk("my_remove\n");
    return 0;
}

const struct platform_device_id my_id_table = {
    .name = "platform_device_test",
};

struct platform_driver my_driver = {
    .driver = {
        .name = "platform_device_test",
        .owner = THIS_MODULE,
    },
    .probe = my_probe,
    .remove = my_remove,
    .id_table = &my_id_table,
};



static int __init platform_driver_init(void)
{
    printk("platform_driver_init\n");
    platform_driver_register(&my_driver);
    return 0;
}

static void __exit platform_driver_exit(void)
{
    printk("platform_driver_exit\n");
    platform_driver_unregister(&my_driver);
}



module_init(platform_driver_init);
module_exit(platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("why");