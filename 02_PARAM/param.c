#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

static int a = 0;
module_param(a, int ,S_IRUGO);
MODULE_PARM_DESC(a, "e.g: a = 1");

static int array[5] = {0};
static int array_size;
module_param_array(array, int, &array_size, S_IRUGO);
MODULE_PARM_DESC(a, "e.g: array = 1, 2, 3, 4");

static char str1[20] = {0};
module_param_string(str, str1, sizeof(str1), S_IRUGO);
MODULE_PARM_DESC(a, "e.g: str = hello");

static int module_param_init(void)
{
    int i;
    printk("%d\n",a);

    for( i = 0; i < array_size; i++){
        printk("%d\t", array[i]);
    }
    printk("\n");

    printk("%s\n",str1);
    
    return 0;
}

static void module_param_exit(void)      // 驱动出口函数
{
    printk("helloworld_exit\n");
}

module_init(module_param_init);
module_exit(module_param_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");