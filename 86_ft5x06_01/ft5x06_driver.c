#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>

int ft5x06_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    printk("ft5x06 probe\n");
    return 0;
}

int ft5x06_remove(struct i2c_client *client)
{
    
    return 0;
}

struct of_device_id ft5x06__match_table[] = {
    { .compatible = "my-ft5x06" },
    { },
};

struct i2c_driver ft5x06_driver = {
    .driver = {
        .name = "my-ft5x06",
        .owner = THIS_MODULE,
        .of_match_table = ft5x06__match_table,
    },
    .probe = ft5x06_probe,
    .remove = ft5x06_remove,
};

static int __init ft5x06_driver_init(void)
{
    int ret;
    ret = i2c_add_driver(&ft5x06_driver);
    if(ret < 0){
        printk("ft5x06 driver register failed\n");
        return ret;
    } 
    return 0;
}

static void __exit ft5x06_driver_exit(void)
{
    i2c_del_driver(&ft5x06_driver);
}

module_init(ft5x06_driver_init);
module_exit(ft5x06_driver_exit);

MODULE_LICENSE("GPL"); 