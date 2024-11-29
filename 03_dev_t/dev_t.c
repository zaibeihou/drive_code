#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>

static int major;
static int minor; 
module_param(major,int ,S_IRUGO);
module_param(minor,int ,S_IRUGO);
static dev_t dev_num;

static int module_param_init(void)
{
    int ret;
    if(major){
        dev_num = MKDEV(major,minor);
        printk("major is %d\n",major);
        printk("minor is %d\n",minor);
        ret = register_chrdev_region(dev_num, 1,"chrdev_name");
        if(ret < 0){
            printk("register_chrdev_region is error\n");
        }
        printk("register_chrdev_region is ok\n");
    }else{
        ret = alloc_chrdev_region(&dev_num, 0,1,"chrdev_name");
        if(ret < 0){
            printk("alloc_chrdev_region is error\n");
        }
        printk("alloc_chrdev_region is ok\n");
        printk("major is %d\n",MAJOR(dev_num));
        printk("minor is %d\n",MINOR(dev_num));
    }
    return 0;
}

static void module_param_exit(void)      // 驱动出口函数
{
    unregister_chrdev_region(dev_num, 1);
    printk("unregister_chrdev is ok\n");
}

module_init(module_param_init);
module_exit(module_param_exit);

MODULE_AUTHOR("WHY");
MODULE_VERSION("V1.0");