#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/input.h>

struct input_dev* my_dev;
int ret;

static int my_input_dev_init(void)
{

    my_dev = input_allocate_device();// 分配一个input_dev结构体
    if(my_dev == NULL)
    {
        printk("input_allocate_device failed\n");
        return -1;
    }

    my_dev->name = "my_input_dev";
    __set_bit(EV_KEY, my_dev->evbit);// 设置支持按键事件
    __set_bit(KEY_1, my_dev->keybit);// 设置支持1键
    
    ret = input_register_device(my_dev);// 注册输入设备
    if(ret < 0)
    {
        printk("input_register_device failed\n");
        goto err_free_dev;
        return -1;
    }
    return 0;

err_free_dev:
    input_free_device(my_dev);
    return ret;
}

static void my_input_dev_exit(void)      // 驱动出口函数
{
    input_unregister_device(my_dev);
}

module_init(my_input_dev_init);
module_exit(my_input_dev_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");