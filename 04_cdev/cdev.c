#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>

static dev_t dev_num;
struct cdev cdev_test;

struct class *class_test;

static int cdev_test_release(struct inode *inode, struct file *filp) {
    printk("This is cdev release\n");
    return 0;
}

static ssize_t cdev_test_write(struct file *filp, const char __user *buf, 
                              size_t count, loff_t *offset) {
    printk("This is cdev write\n");
    return 0;
}

static int cdev_test_open(struct inode *inode, struct file *filp) {
    printk("This is cdev open\n");
    return 0;
}

static ssize_t cdev_test_read(struct file *filp, char __user *buf, 
                             size_t count, loff_t *offset) {
    printk("This is cdev read\n");
    return 0;
}

struct file_operations cdev_test_ops = {
    .owner = THIS_MODULE, //将owner字段指向本模块，可以避免在模块的操作正在被使用时卸载该模块
    .read = cdev_test_read,
    .write = cdev_test_write,
    .release = cdev_test_release,
    .open = cdev_test_open,
};


static int module_cdev_init(void)
{
    int ret;
    ret = alloc_chrdev_region(&dev_num, 0,1,"chrdev_name");
    if(ret < 0){
        printk("alloc_chrdev_region is error\n");
    }
    printk("alloc_chrdev_region is ok\n");
    printk("major is %d\n",MAJOR(dev_num));
    printk("minor is %d\n",MINOR(dev_num));
    
    cdev_test.owner = THIS_MODULE;
    cdev_init(&cdev_test, &cdev_test_ops);
    ret = cdev_add(&cdev_test,dev_num,1);
    if(ret < 0){
        printk("cdev_addiserror\n");
    }
    printk("cdev_add is ok\n");

    class_test = class_create(THIS_MODULE, "test");
    device_create(class_test, NULL, dev_num, NULL, "test");
    printk("class_create  device_create is ok\n");
    return 0;
}
    


static void module_cdev_exit(void)      // 驱动出口函数
{
    cdev_del(&cdev_test);
    printk("cdev_del is ok\n");
    unregister_chrdev_region(dev_num, 1);
    printk("unregister_chrdev is ok\n");

    device_destroy(class_test, dev_num);
    class_destroy(class_test);
    printk("device  class destroy is ok\n");
}

module_init(module_cdev_init);
module_exit(module_cdev_exit);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("WHY");
MODULE_VERSION("V1.0");