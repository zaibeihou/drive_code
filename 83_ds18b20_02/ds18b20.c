#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define SET_RESOLUTION _IOW('A', 1, int)  // 参数1：命令的类型，参数2：命令的序号，参数3：命令的参数类型
#define GET_RESOLUTION _IOR('A', 2, int)  // 建议使用不同的命令序号

#define SET_RESOLUTION_9 9
#define SET_RESOLUTION_10 10
#define SET_RESOLUTION_11 11
#define SET_RESOLUTION_12 12


struct ds18b20_data {
    struct cdev ds18b20_cdev;
    dev_t dev_num;
    struct class *ds18b20_class;
    struct device *ds18b20_device;
    struct gpio_desc *ds18b20_gpio;
};

struct ds18b20_data *ds18b20;

void ds18b20_reset(void){
    gpiod_direction_output(ds18b20->ds18b20_gpio, 1);//设置为输出模式
    //printk("ds18b20_reset\n");

    gpiod_set_value(ds18b20->ds18b20_gpio, 0);
    udelay(500);
    gpiod_set_value(ds18b20->ds18b20_gpio, 1);

    gpiod_direction_input(ds18b20->ds18b20_gpio);//设置为输入模式
    while(gpiod_get_value(ds18b20->ds18b20_gpio));
    while(!gpiod_get_value(ds18b20->ds18b20_gpio));
    udelay(480);
}

void ds18b20_write_bit(unsigned char bit){
    
    gpiod_direction_output(ds18b20->ds18b20_gpio, 1);//设置为输出模式
    //printk("ds18b20_write_bit\n");
    gpiod_set_value(ds18b20->ds18b20_gpio, 0);
    if(bit){
        udelay(5);
        gpiod_set_value(ds18b20->ds18b20_gpio, 1);
    }
    udelay(65);
    gpiod_set_value(ds18b20->ds18b20_gpio, 1);
    udelay(2);
}

void ds18b20_write_byte(unsigned char byte){
    int i;
    for(i = 0; i < 8; i++){
        ds18b20_write_bit(byte & 0x01);
        byte >>= 1;
    }
}

unsigned char ds18b20_read_bit(void){
    unsigned char bit;
    gpiod_direction_output(ds18b20->ds18b20_gpio, 1);//设置为输出模式
    //printk("ds18b20_read_bit\n");
    gpiod_set_value(ds18b20->ds18b20_gpio, 0);
    udelay(2);

    gpiod_direction_input(ds18b20->ds18b20_gpio);//设置为输入模式
    udelay(10);
    
    bit = gpiod_get_value(ds18b20->ds18b20_gpio);
    udelay(65);
    return bit;
}

unsigned char ds18b20_read_byte(void){
    int i;
    unsigned char byte = 0;
    for(i = 0; i < 8; i++){ 
        
        byte |= (ds18b20_read_bit() << i);
    }
    return byte;
}

int ds18b20_read_temperature(void){
    unsigned char temp_l, temp_h;
    int temp;
    ds18b20_reset();
    ds18b20_write_byte(0xcc);
    ds18b20_write_byte(0x44); 
    mdelay(750);
    ds18b20_reset();
    ds18b20_write_byte(0xcc);
    ds18b20_write_byte(0xbe);
    temp_l = ds18b20_read_byte();
    temp_h = ds18b20_read_byte();
    temp = temp_h;
    temp <<= 8;
    temp |= temp_l;
    return temp;
}

int ds18b20_open(struct inode *inode, struct file *file){
    //printk("ds18b20_open\n");
    return 0;
}

int ds18b20_release(struct inode *inode, struct file *file){
    //printk("ds18b20_release\n");
    return 0;
}

ssize_t ds18b20_read(struct file *file, char __user *buf, size_t count, loff_t *ppos){
    int temp;
    int ret;
   //printk("ds18b20_read\n");
    temp = ds18b20_read_temperature();
    ret = copy_to_user(buf, &temp, sizeof(temp));
    if(ret < 0){
        printk("ds18b20_read copy_to_user failed\n");
        return -EFAULT;
    }
    return 0;
}

void set_ds18b20_resolution(int args){
    ds18b20_reset();
    ds18b20_write_byte(0xcc);
    ds18b20_write_byte(0x4e); 
    
    ds18b20_write_byte(60);//设置报警上限 
    ds18b20_write_byte(10);//设置报警下限

    switch(args){
        case SET_RESOLUTION_9:
            ds18b20_write_byte(0x1f);
            break;
        case SET_RESOLUTION_10:
            ds18b20_write_byte(0x3f);
            break;
        case SET_RESOLUTION_11:
            ds18b20_write_byte(0x5f);
            break;
        case SET_RESOLUTION_12:
            ds18b20_write_byte(0x7f);
            break;
    }
    return;
}

int get_ds18b20_resolution(void){
    int resolution;
    ds18b20_reset();
    ds18b20_write_byte(0xcc);
    ds18b20_write_byte(0xbe);
    ds18b20_read_byte();
    ds18b20_read_byte();
    ds18b20_read_byte();
    ds18b20_read_byte();//跳过四个字节 分别是温度低字节，温度高字节，报警上限，报警下限

    resolution = ds18b20_read_byte();
    switch(resolution){
        case 0x1f:
            resolution = SET_RESOLUTION_9;
            break;
        case 0x3f:
            resolution = SET_RESOLUTION_10;
            break;
        case 0x5f:
            resolution = SET_RESOLUTION_11;
            break;
        case 0x7f:
            resolution = SET_RESOLUTION_12;
            break;
    }
    return resolution;
}

long ds18b20_ioctl (struct file *file, unsigned int cmd, unsigned long args){
    int resolution;
    if(cmd == SET_RESOLUTION){
        if(args >= SET_RESOLUTION_9 && args <= SET_RESOLUTION_12){
            set_ds18b20_resolution(args);
        }
    }else if(cmd == GET_RESOLUTION){
        resolution = get_ds18b20_resolution();
        printk("resolution = %d\n", resolution);
        copy_to_user((int *)args, &resolution, sizeof(resolution));
    }
    return 0;
}

struct file_operations ds18b20_ops = {
    .owner = THIS_MODULE,
    .open = ds18b20_open,
    .release = ds18b20_release,
    .read = ds18b20_read,
    .unlocked_ioctl = ds18b20_ioctl,
};

int ds18b20_probe(struct platform_device *dev){
    int ret;
    printk("ds18b20_probe\n");
    
    ds18b20 = kzalloc(sizeof(struct ds18b20_data), GFP_KERNEL);
    if(!ds18b20){
        printk("ds18b20 kzalloc failed\n");
        ret =  -ENOMEM;
        goto error_0;
    }

    ret = alloc_chrdev_region(&ds18b20->dev_num, 0, 1, "my_ds18b20");
    if(ret < 0){
        printk("ds18b20 alloc_chrdev_region failed\n");
        ret = -EAGAIN;
        goto error_1;
    }
    cdev_init(&ds18b20->ds18b20_cdev, &ds18b20_ops);
    ds18b20->ds18b20_cdev.owner = THIS_MODULE;//为什么要设置这个 owner ？这个是为了在模块卸载的时候，可以自动释放设备号   

    cdev_add(&ds18b20->ds18b20_cdev, ds18b20->dev_num, 1);//参数1：cdev结构体指针，参数2：设备号，参数3：设备号个数

    ds18b20->ds18b20_class = class_create(THIS_MODULE, "sensors");
    if(IS_ERR(ds18b20->ds18b20_class)){
        printk("ds18b20 class_create failed\n");
        ret = PTR_ERR(ds18b20->ds18b20_class);
        goto error_2;
    }

    ds18b20->ds18b20_device =  device_create(ds18b20->ds18b20_class, NULL, ds18b20->dev_num, NULL, "ds18b20");
    if(IS_ERR(ds18b20->ds18b20_device)){
        printk("ds18b20 device_create failed\n");
        ret = PTR_ERR(ds18b20->ds18b20_device);
        goto error_3;
    }

    ds18b20->ds18b20_gpio = gpiod_get_optional(&dev->dev, "ds18b20", 0);
    if(IS_ERR(ds18b20->ds18b20_gpio)){
        printk("ds18b20 gpiod_get_optional failed\n");
        ret = PTR_ERR(ds18b20->ds18b20_gpio);
        goto error_4;
    }

    gpiod_direction_output(ds18b20->ds18b20_gpio, 1);

    return 0;

error_4:
    device_destroy(ds18b20->ds18b20_class, ds18b20->dev_num);
error_3:
    class_destroy(ds18b20->ds18b20_class);
error_2:
    cdev_del(&ds18b20->ds18b20_cdev);
    unregister_chrdev_region(ds18b20->dev_num, 1);
error_1:
    kfree(ds18b20);
error_0:
    return ret;
}

struct of_device_id ds18b20__match_table[] = {
    { .compatible = "ds18b20" },
    { },
};

struct platform_driver ds18b20_driver = {
    .driver = {
        .name = "ds18b20",
        .owner = THIS_MODULE,
        .of_match_table = ds18b20__match_table,
    },
    .probe = ds18b20_probe,
};

static int __init ds18b20_init(void)
{
    int ret;
    ret = platform_driver_register(&ds18b20_driver);
    if(ret < 0){
        printk("ds18b20 driver register failed\n");
        return ret;
    }


    return 0;
}

static void __exit ds18b20_exit(void)
{
    gpiod_put(ds18b20->ds18b20_gpio);
    device_destroy(ds18b20->ds18b20_class, ds18b20->dev_num);
    class_destroy(ds18b20->ds18b20_class);
    cdev_del(&ds18b20->ds18b20_cdev);
    unregister_chrdev_region(ds18b20->dev_num, 1);
    kfree(ds18b20);
    platform_driver_unregister(&ds18b20_driver);
}

module_init(ds18b20_init);
module_exit(ds18b20_exit);   
MODULE_LICENSE("GPL");