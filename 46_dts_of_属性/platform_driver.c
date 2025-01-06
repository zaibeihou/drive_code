#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>

static struct device_node *my_node;
static struct property *my_property;
int size;

static int my_probe(struct platform_device * dev){
    printk("my_probe\n");
    my_node = of_find_node_by_name(NULL, "myled");
    my_property = of_find_property(my_node,"compatible",&size);
    if(my_node == NULL){
        printk("find failed\n");
        return -1;
    }
    printk("myled node name: %s\n", my_node->name);
    printk("myled node property: %s\n", my_property->name);
    
    return 0;
}

static int my_remove(struct platform_device *dev){
    printk("my_remove\n");
    return 0;
}

const struct of_device_id of_match_table_id[] = {
    { .compatible = "my_led", },
    { },
};

struct platform_driver my_driver = {
    .driver = {
        .name = "my_led",
        .owner = THIS_MODULE,
        .of_match_table = of_match_table_id,
    },
    .probe = my_probe,
    .remove = my_remove,
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