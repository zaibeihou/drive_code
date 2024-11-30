#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/io.h>

#define GPIO_DR 0xFDD60000

struct dev_test
{
    dev_t dev_num;
    struct cdev cdev_test;
    struct class *class_test;
    struct device *device;
    char kbuf[32];
    int minor;
    int major;
    unsigned int *vp_gpio_dr;
};

struct dev_test dev;

static int cdev_test_open(struct inode *inode, struct file *file) {
    //printk("This is cdev open\n");
    file->private_data = container_of(inode->i_cdev, struct dev_test, cdev_test);
    return 0;
}

static ssize_t cdev_test_read(struct file *file, char __user *buf, 
                             size_t size, loff_t *offset) {
    struct dev_test *test_dev =(struct dev_test *)file->private_data;
    copy_to_user(buf,test_dev->kbuf, strlen(test_dev->kbuf));

    printk("This is cdev read\n");
    return 0;
}

static ssize_t cdev_test_write(struct file *file, const char __user *buf, 
                              size_t size, loff_t *offset) {
    struct dev_test *test_dev =(struct dev_test *)file->private_data;
    
    copy_from_user(test_dev->kbuf,buf,size);
   
    if(test_dev->kbuf[0] == 1){
        *(test_dev->vp_gpio_dr) = 0x8000c040;
        printk("%d\n",test_dev->kbuf[0]);
    }else if(test_dev->kbuf[0] == 0){
        *(test_dev->vp_gpio_dr) = 0x80004040;
        printk("%d\n",test_dev->kbuf[0]);
    }
    //printk("This is cdev write\n");
    return 0;
}

static int cdev_test_release(struct inode *inode, struct file *file) {
    //printk("This is cdev release\n");
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
// 1. 申请设备号
    ret = alloc_chrdev_region(&dev.dev_num, 0, 1, "chrdev_name");
    if(ret != 0){
        goto alloc_chrdev_region_error;
    }

    dev.major = MAJOR(dev.dev_num);
    dev.minor = MINOR(dev.dev_num);
    cdev_init(&dev.cdev_test, &cdev_test_ops);
    ret = cdev_add(&dev.cdev_test, dev.dev_num, 1);
    if(ret != 0){
        goto cdev_add_error;
    }

    dev.class_test = class_create(THIS_MODULE, "test");
    if(IS_ERR(dev.class_test)){
        ret = PTR_ERR(dev.class_test);//注意分辨 PTR_ERR 和 ERR_PTR
        goto class_create_error;
    }

    dev.device = device_create(dev.class_test, NULL, dev.dev_num, NULL, "test");
    if(IS_ERR(dev.device)){
        ret = PTR_ERR(dev.device);
        goto device_create1_error;
    }

    dev.vp_gpio_dr = ioremap(GPIO_DR,4);
    if(IS_ERR(dev.vp_gpio_dr)){
        ret = PTR_ERR(dev.vp_gpio_dr);
        goto ioremap_error;
    }

    return 0;

ioremap_error:
    device_destroy(dev.class_test, dev.dev_num);
device_create1_error:
    class_destroy(dev.class_test);
class_create_error:
    cdev_del(&dev.cdev_test);
cdev_add_error:
    unregister_chrdev_region(dev.dev_num, 2);
alloc_chrdev_region_error:
    return ret;
}
    


static void module_cdev_exit(void)      // 驱动出口函数
{
// 删除第一个设备的资源
    iounmap(dev.vp_gpio_dr);
    cdev_del(&dev.cdev_test);
    device_destroy(dev.class_test, dev.dev_num);
    unregister_chrdev_region(dev.dev_num, 1);
    class_destroy(dev.class_test);
}

module_init(module_cdev_init);
module_exit(module_cdev_exit);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("WHY");
MODULE_VERSION("V1.0");