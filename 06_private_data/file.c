#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

struct dev_test
{
    dev_t dev_num;
    struct cdev cdev_test;
    struct class *class_test;
    struct device *device;
    char kbuf[32];
    int minor;
    int major;
};

struct dev_test dev1;
struct dev_test dev2;

static int cdev_test_open(struct inode *inode, struct file *file) {
    //printk("This is cdev open\n");
    dev1.minor = 0;
    dev2.minor = 1;

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

    if(test_dev->minor == 0){
        printk("test0: %s\n",test_dev->kbuf);
    }else if(test_dev->minor == 1){
        printk("test1: %s\n",test_dev->kbuf);
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
    ret = alloc_chrdev_region(&dev1.dev_num, 0, 2, "chrdev_name");
    if(ret != 0){
        goto alloc_chrdev_region_error;
    }
// 2. 初始化第一个设备
    dev1.major = MAJOR(dev1.dev_num);
    dev1.minor = MINOR(dev1.dev_num);
    cdev_init(&dev1.cdev_test, &cdev_test_ops);
    ret = cdev_add(&dev1.cdev_test, dev1.dev_num, 1);
    if(ret != 0){
        goto cdev1_add_error;
    }
// 3. 初始化第二个设备
    dev2.dev_num = MKDEV(MAJOR(dev1.dev_num), MINOR(dev1.dev_num) + 1);  // 正确计算第二个设备号
    dev2.major = MAJOR(dev2.dev_num);
    dev2.minor = MINOR(dev2.dev_num);
    cdev_init(&dev2.cdev_test, &cdev_test_ops);
    ret = cdev_add(&dev2.cdev_test, dev2.dev_num, 1);
    if(ret != 0){
        goto cdev2_add_error;
    }
// 4. 创建一个class，用于两个设备
    dev1.class_test = class_create(THIS_MODULE, "test");
    if(IS_ERR(dev1.class_test)){
        ret = PTR_ERR(dev1.class_test);//注意分辨 PTR_ERR 和 ERR_PTR
        goto class_create_error;
    }
// 5. 创建两个不同名字的设备节点
    dev1.device = device_create(dev1.class_test, NULL, dev1.dev_num, NULL, "test0");
    if(IS_ERR(dev1.device)){
        ret = PTR_ERR(dev1.device);
        goto device_create1_error;
    }
    dev2.device = device_create(dev1.class_test, NULL, dev2.dev_num, NULL, "test1");
    if(IS_ERR(dev2.device)){
        ret = PTR_ERR(dev2.device);
        goto device_create2_error;
    }
    return 0;

device_create2_error: //遵循先进后出原则，将之前创建的一一删除
    device_destroy(dev1.class_test, dev1.dev_num);
device_create1_error:
    class_destroy(dev1.class_test);
class_create_error:
    cdev_del(&dev2.cdev_test);
cdev2_add_error:
    cdev_del(&dev1.cdev_test);
cdev1_add_error:
    unregister_chrdev_region(dev1.dev_num, 2);
alloc_chrdev_region_error:
    return ret;
}
    


static void module_cdev_exit(void)      // 驱动出口函数
{
// 删除第一个设备的资源
    cdev_del(&dev1.cdev_test);
    device_destroy(dev1.class_test, dev1.dev_num);
    
// 删除第二个设备的资源
    cdev_del(&dev2.cdev_test);
    device_destroy(dev1.class_test, dev2.dev_num);  // 注意这里用的是同一个class
    
// 释放设备号（因为是一起申请的2个，这里释放时也要指定2）
    unregister_chrdev_region(dev1.dev_num, 2);
    
// 最后才销毁class（因为两个设备共用一个class）
    class_destroy(dev1.class_test);
    
    printk("All devices removed successfully\n");
}

module_init(module_cdev_init);
module_exit(module_cdev_exit);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("WHY");
MODULE_VERSION("V1.0");