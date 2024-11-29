#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

extern int add(int a, int b);

static int __init helloworld_init(void)
{
    
    printk("%d\n",add(1,2));   // KERN_ALERT 确保日志会被显示
    return 0;
}

static void __exit helloworld_exit(void)      // 驱动出口函数
{
    printk("helloworld_exit_b\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");