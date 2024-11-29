#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init helloworld_init(void)
{
    // 使用带有日志级别的 printk
    printk(KERN_ALERT "helloworld_init\n");   // KERN_ALERT 确保日志会被显示
    return 0;
}

static void __exit helloworld_exit(void)      // 驱动出口函数
{
    printk(KERN_ALERT "helloworld_exit\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");