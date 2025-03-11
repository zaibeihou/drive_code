#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/input.h>
#include <linux/time.h>
#include <linux/kernel.h>

struct input_dev* my_dev;
int ret;

void func(struct timer_list *timer);

DEFINE_TIMER(test_timer,func);

void func(struct timer_list *timer)  // 修改参数类型
{
    static int value = 0;
    value = !value;

    input_event(my_dev, EV_KEY, KEY_1, value);// 模拟按键事件
    input_sync(my_dev);//作用是告诉系统，这个事件已经结束了，可以处理了
    
    mod_timer(&test_timer,jiffies_64 + msecs_to_jiffies(1000));
}

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

    test_timer.expires = jiffies_64 + msecs_to_jiffies(1000);
    add_timer(&test_timer);

    return 0;

err_free_dev:
    input_free_device(my_dev);
    return ret;
}

static void my_input_dev_exit(void)      // 驱动出口函数
{
    del_timer(&test_timer);//删除定时器
    input_unregister_device(my_dev);//取消注册输入设备
    input_free_device(my_dev);//释放输入设备内存
}

module_init(my_input_dev_init);
module_exit(my_input_dev_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");