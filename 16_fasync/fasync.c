#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/poll.h>

struct dev_test
{
    dev_t dev_num;
    struct cdev cdev_test;
    struct class *class_test;
    struct device *device;
    char kbuf[32];
    int minor;
    int major;
    int flag;
    struct fasync_struct *fasync;
};

struct dev_test dev1;


DECLARE_WAIT_QUEUE_HEAD(head);

static int cdev_test_open(struct inode *inode, struct file *file) {
    file->private_data = container_of(inode->i_cdev, struct dev_test, cdev_test);
    return 0;
}

static ssize_t cdev_test_read(struct file *file, char __user *buf, 
                             size_t size, loff_t *offset) {
    struct dev_test *test_dev =(struct dev_test *)file->private_data;
    int ret;
    wait_event_interruptible(head,test_dev->flag);

    ret = copy_to_user(buf, test_dev->kbuf, strlen(test_dev->kbuf));
    if (ret)
        return -EFAULT;
    test_dev->flag = 0;
    //printk("This is cdev read\n");
    return 0;
}

static ssize_t cdev_test_write(struct file *file, const char __user *buf, 
                              size_t size, loff_t *offset) {
    struct dev_test *test_dev =(struct dev_test *)file->private_data;
    int ret;

    ret = copy_from_user(test_dev->kbuf, buf, size);
    if (ret)
        return -EFAULT;
    
    test_dev->flag=1;
    wake_up_interruptible(&head);
    kill_fasync(&test_dev->fasync,SIGIO,POLLIN);
    //printk("This is cdev write\n");
    return 0;
}

static int cdev_test_release(struct inode *inode, struct file *file) {

    //printk("This is cdev release\n");
    return 0;
}

static __poll_t cdev_test_poll (struct file *file, struct poll_table_struct *p){
    struct dev_test *test_dev =(struct dev_test *)file->private_data;
    __poll_t mask=0;
    poll_wait(file,&head,p);
    if (test_dev->flag == 1)
    {
        mask |= POLLIN;
    }
    return mask;
}

static int cdev_test_fasync(int fd, struct file *file, int on){
    struct dev_test *test_dev=(struct dev_test *)file->private_data;
    return fasync_helper(fd,file,on,&test_dev->fasync);
}

struct file_operations cdev_test_ops = {
    .owner = THIS_MODULE, //将owner字段指向本模块，可以避免在模块的操作正在被使用时卸载该模块
    .read = cdev_test_read,
    .write = cdev_test_write,
    .release = cdev_test_release,
    .open = cdev_test_open,
    .poll = cdev_test_poll,
    .fasync = cdev_test_fasync,
};


static int module_cdev_init(void)
{
    int ret;
// 1. 申请设备号
    ret = alloc_chrdev_region(&dev1.dev_num, 0, 1, "file_test");
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
   
    dev1.class_test = class_create(THIS_MODULE, "file_test");
    if(IS_ERR(dev1.class_test)){
        ret = PTR_ERR(dev1.class_test);//注意分辨 PTR_ERR 和 ERR_PTR
        goto class_create_error;
    }

    dev1.device = device_create(dev1.class_test, NULL, dev1.dev_num, NULL, "test0");
    if(IS_ERR(dev1.device)){
        ret = PTR_ERR(dev1.device);
        goto device_create1_error;
    }

    printk("主设备号：%d 次设备号:test0 %d \n",dev1.major,dev1.minor);
    dev1.flag = 0;

    return 0;


device_create1_error:
    class_destroy(dev1.class_test);
class_create_error:
    cdev_del(&dev1.cdev_test);
cdev1_add_error:
    unregister_chrdev_region(dev1.dev_num, 1);
alloc_chrdev_region_error:
    return ret;
}
    


static void module_cdev_exit(void)      // 驱动出口函数
{
// 删除第一个设备的资源
    cdev_del(&dev1.cdev_test);
    device_destroy(dev1.class_test, dev1.dev_num);
    
// 释放设备号（因为是一起申请的2个，这里释放时也要指定2）
    unregister_chrdev_region(dev1.dev_num, 1);
    
// 最后才销毁class（因为两个设备共用一个class）
    class_destroy(dev1.class_test);
    
    printk("All devices removed successfully\n");
}

module_init(module_cdev_init);
module_exit(module_cdev_exit);

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("WHY");
MODULE_VERSION("V1.0");