#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>



static int cdev_test_open(struct inode *inode, struct file *file) {
    printk("cdev_test_open\n");
    return 0;
}

static ssize_t cdev_test_read(struct file *file, char __user *buf, 
                             size_t size, loff_t *offset) {

    return 0;
}

static ssize_t cdev_test_write(struct file *file, const char __user *buf, 
                              size_t size, loff_t *offset) {
    
    return 0;
}

static int cdev_test_release(struct inode *inode, struct file *file) {
    
    return 0;
}

struct file_operations misc_cdev_ops = {
    .owner = THIS_MODULE, //将owner字段指向本模块，可以避免在模块的操作正在被使用时卸载该模块
    .read = cdev_test_read,
    .write = cdev_test_write,
    .release = cdev_test_release,
    .open = cdev_test_open,
};

struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_test",
    .fops = &misc_cdev_ops
};

static int module_cdev_init(void)
{
    int ret;
    ret = misc_register(&misc);
    if(ret != 0){
        printk("misc_register error\n");
        return -1;
    }
    return 0;
}
    


static void module_cdev_exit(void){      // 驱动出口函数
    misc_deregister(&misc);
}

module_init(module_cdev_init);
module_exit(module_cdev_exit);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("WHY");
MODULE_VERSION("V1.0");