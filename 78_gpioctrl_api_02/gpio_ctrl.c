#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_device.h>

struct pinctrl *pinctrl;
struct pinctrl_state *func1_state;
struct pinctrl_state *func2_state;

ssize_t select_mux_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count){
    unsigned long select;
    select = simple_strtoul(buf,NULL,10);
    if(select==1){
        pinctrl_select_state(pinctrl,func1_state); //选择功能1状态
    }else if(select==0){
        pinctrl_select_state(pinctrl,func2_state); //选择功能2状态
    }
    return count;      
}

DEVICE_ATTR_WO(select_mux);



static int my_probe(struct platform_device * dev){
    printk("my_probe\n");
    
    device_create_file(&dev->dev, &dev_attr_select_mux);

    pinctrl = pinctrl_get(&dev->dev);

    func1_state = pinctrl_lookup_state(pinctrl, "mygpio_func1");
    func2_state = pinctrl_lookup_state(pinctrl, "mygpio_func2");


    return 0;
}

static int my_remove(struct platform_device *dev){
    printk("my_remove\n");
    return 0;
}

const struct of_device_id	of_match_table_id[] = {
    { .compatible = "my_gpio", },
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