#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/wait.h>
#include <linux/ioctl.h>
#include <linux/time.h>

#define BUFSIZE 1024
char mem [BUFSIZE];

#define CMD_TIME_CLOSE _IO('A',1)
#define CMD_TIME_OPEN _IO('A',2)
#define CMD_TIME_SET _IOW('A',2,int)


struct dev_test
{
    dev_t dev_num;
    struct cdev cdev_test;
    struct class *class_test;
    struct device *device;
    char kbuf[32];
    int minor;
    int major;
    int counter;
};

struct dev_test dev1;

void func(struct timer_list *timer);

DEFINE_TIMER(test_timer,func);

void func(struct timer_list *timer)  // 修改参数类型
{
    printk("timer function running!\n");
    mod_timer(&test_timer,jiffies_64 + msecs_to_jiffies(dev1.counter));
}


static int cdev_test_open(struct inode *inode, struct file *file) {
    file->private_data = container_of(inode->i_cdev, struct dev_test, cdev_test);
    
    //struct dev_test *test_dev =(struct dev_test *)file->private_data;
    
    return 0;
}

static ssize_t cdev_test_read(struct file *file, char __user *buf, size_t size, loff_t *offset) {
    //struct dev_test *test_dev =(struct dev_test *)file->private_data;
    int ret;
    loff_t p = *offset;//偏移量
    size_t count = size;//要读取的大小
    if(p > BUFSIZE){
        return-1;
    }
    if(count > BUFSIZE - p){
        count = BUFSIZE - p;
    }

    ret = copy_to_user(buf, mem + p, count);
    if (ret)
        return -EFAULT;
    *offset += count;
    return count;
}

static ssize_t cdev_test_write(struct file *file, const char __user *buf, size_t size, loff_t *offset) {
    //struct dev_test *test_dev =(struct dev_test *)file->private_data;
    int ret;

    loff_t p = *offset;//将写入数据的偏移量赋值给loff_t类型变量p
    size_t count = size;

    if(p > BUFSIZE){
        return-1;
    }
    if(count > BUFSIZE - p){
        count = BUFSIZE - p;//如果要写入的偏移值超出剩余的空间，则写入到最后位置
    }

    ret = copy_from_user(mem + p, buf, count);
    if (ret)
        return -EFAULT;
    

    *offset += count;

    return count;
}

static int cdev_test_release(struct inode *inode, struct file *file) {
    //struct dev_test *test_dev = (struct dev_test *)file->private_data;
    del_timer(&test_timer);
    return 0;

}

static loff_t cdev_test_llseek(struct file *file, loff_t offset, int whence){
    loff_t new_offset;//定义loff_t类型的新的偏移值
    switch(whence){//对lseek函数传递的whence参数进行判断
        case SEEK_SET:
            if(offset < 0){
                return -EINVAL;
                break;
            }
            if(offset > BUFSIZE){
                return -EINVAL;
                break;
            }
            new_offset = offset;//如果whence参数为SEEK_SET，则新偏移值为offset
            break;
        case SEEK_CUR:
            if(file->f_pos + offset > BUFSIZE){
                return-EINVAL;
                break;
            }
            if(file->f_pos + offset < 0){
                return-EINVAL;
                break;
            }
            new_offset = file->f_pos + offset;//如果whence参数为SEEK_CUR，则新偏移值为file->f_pos+offset，file->f_pos为当前的偏移值
        break;
        case SEEK_END:
            if(file->f_pos+offset < 0){
                return-EINVAL;
                break;
            }
            new_offset = BUFSIZE + offset;
            break;
        default:
            break;
    }
    file->f_pos = new_offset;
    return new_offset;
}

static long cdev_test_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    struct dev_test *test_dev =(struct dev_test *)file->private_data;
    switch (cmd)
    {
    case CMD_TIME_OPEN:
        add_timer(&test_timer);
        break;
    case CMD_TIME_CLOSE:
        del_timer(&test_timer);
        break;
    case CMD_TIME_SET:
        test_dev->counter = arg;
        test_timer.expires = jiffies_64 + msecs_to_jiffies(test_dev->counter);
    break;
    default:
        break;
    }
    return 0;
}

struct file_operations cdev_test_ops = {
    .owner = THIS_MODULE, //将owner字段指向本模块，可以避免在模块的操作正在被使用时卸载该模块
    .read = cdev_test_read,
    .write = cdev_test_write,
    .release = cdev_test_release,
    .open = cdev_test_open,
    .llseek = cdev_test_llseek,
    .unlocked_ioctl = cdev_test_ioctl,
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
    del_timer(&test_timer);
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