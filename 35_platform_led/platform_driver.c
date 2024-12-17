#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/io.h>

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

struct dev_test dev_led;

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

struct resource *my_platform_resource;

static int my_probe(struct platform_device * dev){
    int ret;

    printk("this is my_probe\n");
    printk("IRQ is %lld\n", dev->resource[1].start);
    
    my_platform_resource = platform_get_resource(dev, IORESOURCE_MEM, 0);
    printk("MEM is %0llx\n", my_platform_resource->start);
    
   
// 1. 申请设备号
    ret = alloc_chrdev_region(&dev_led.dev_num, 0, 1, "platform_led");
    if(ret != 0){
        goto alloc_chrdev_region_error;
    }

    dev_led.major = MAJOR(dev_led.dev_num);
    dev_led.minor = MINOR(dev_led.dev_num);
    cdev_init(&dev_led.cdev_test, &cdev_test_ops);
    ret = cdev_add(&dev_led.cdev_test, dev_led.dev_num, 1);
    if(ret != 0){
        goto cdev_add_error;
    }

    dev_led.class_test = class_create(THIS_MODULE, "platform_led_test");
    if(IS_ERR(dev_led.class_test)){
        ret = PTR_ERR(dev_led.class_test);//注意分辨 PTR_ERR 和 ERR_PTR
        goto class_create_error;
    }

    dev_led.device = device_create(dev_led.class_test, NULL, dev_led.dev_num, NULL, "platform_led_test");
    if(IS_ERR(dev_led.device)){
        ret = PTR_ERR(dev_led.device);
        goto device_create1_error;
    }

    dev_led.vp_gpio_dr = ioremap(my_platform_resource->start,4);
    if(IS_ERR(dev_led.vp_gpio_dr)){
        ret = PTR_ERR(dev_led.vp_gpio_dr);
        goto ioremap_error;
    }

    return 0;

ioremap_error:
    device_destroy(dev_led.class_test, dev_led.dev_num);
device_create1_error:
    class_destroy(dev_led.class_test);
class_create_error:
    cdev_del(&dev_led.cdev_test);
cdev_add_error:
    unregister_chrdev_region(dev_led.dev_num, 2);
alloc_chrdev_region_error:
    return ret;
}

static int my_remove(struct platform_device *dev){
    printk("my_remove\n");
    return 0;
}

const struct platform_device_id my_id_table = {
    .name = "platform_device_test",
};

struct platform_driver my_driver = {
    .driver = {
        .name = "platform_device_test",
        .owner = THIS_MODULE,
    },
    .probe = my_probe,
    .remove = my_remove,
    .id_table = &my_id_table,
};



static int __init platform_driver_init(void)
{
    printk("platform_driver_init\n");
    platform_driver_register(&my_driver);
    return 0;
}

static void __exit platform_driver_exit(void)
{
    printk("platform_driver_exit\n");
    platform_driver_unregister(&my_driver);

    // 删除第一个设备的资源
    iounmap(dev_led.vp_gpio_dr);
    cdev_del(&dev_led.cdev_test);
    device_destroy(dev_led.class_test, dev_led.dev_num);
    unregister_chrdev_region(dev_led.dev_num, 1);
    class_destroy(dev_led.class_test);
}



module_init(platform_driver_init);
module_exit(platform_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("why");