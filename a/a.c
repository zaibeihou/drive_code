#include <linux/module.h>
#include <linux/init.h>

extern int add(int a, int b){
    return a+b;
}
EXPORT_SYMBOL(add);
static int helloworld_init(void)
{
    
    printk("helloworld_init\n");
    return 0;
}

static void helloworld_exit(void)      
{
    printk("helloworld_exit\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");