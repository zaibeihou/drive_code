#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/time.h>

void func(struct timer_list *timer);

DEFINE_TIMER(test_timer,func);

void func(struct timer_list *timer)  // 修改参数类型
{
    printk("timer function running!\n");
    mod_timer(&test_timer,jiffies_64 + msecs_to_jiffies(1000));
}

static int __init helloworld_init(void)
{
    // 使用带有日志级别的 printk
    printk("helloworld_init\n");

    test_timer.expires = jiffies_64 + msecs_to_jiffies(1000);
    add_timer(&test_timer);
    return 0;
}

static void __exit helloworld_exit(void)      // 驱动出口函数
{
    printk("helloworld_exit\n");
    del_timer(&test_timer);
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");