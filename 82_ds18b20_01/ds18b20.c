#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

int ds18b20_probe(struct platform_device *dev){
    printk("ds18b20_probe\n");
    return 0;
}

struct of_device_id ds18b20__match_table[] = {
    { .compatible = "ds18b20" },
    { },
};

struct platform_driver ds18b20_driver = {
    .driver = {
        .name = "ds18b20",
        .owner = THIS_MODULE,
        .of_match_table = ds18b20__match_table,
    },
    .probe = ds18b20_probe,
};

static int __init ds18b20_init(void)
{
    int ret;
    ret = platform_driver_register(&ds18b20_driver);
    if(ret < 0){
        printk("ds18b20 driver register failed\n");
        return ret;
    }
    return 0;
}

static void __exit ds18b20_exit(void)
{
    platform_driver_unregister(&ds18b20_driver);
}

module_init(ds18b20_init);
module_exit(ds18b20_exit);   
MODULE_LICENSE("GPL");