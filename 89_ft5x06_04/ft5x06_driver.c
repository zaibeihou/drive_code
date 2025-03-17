#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/input.h>
#include <linux/workqueue.h>

void work_handler(struct work_struct *work);



DECLARE_WORK(work,work_handler);

static struct gpio_desc *reset_gpio;
static struct gpio_desc *irq_gpio;

static struct i2c_client *ft5x06_client;
static struct input_dev *ft5x06_input_dev;

u8 ft5x06_read_meg(u8 reg){
    u8 data;
    struct i2c_msg msg[] = {
        [0] = {
            .addr = ft5x06_client->addr,
            .flags = 0,
            .len = sizeof(reg),
            .buf = &reg,//ft5x06_client->addr是从机地址 reg是从机中的寄存器地址  这里的从机指的是I2C控制器还是触摸屏控制器 ？从机指的是触摸屏控制器
        },
        [1] = {
            .addr = ft5x06_client->addr,
            .flags = I2C_M_RD,
            .len = sizeof(data),
            .buf = &data,
        },
    };
    i2c_transfer(ft5x06_client->adapter,msg,2);
    return data;
}

void ft5x06_write_msg(u8 reg,u8 data,u16 len){
    u8 buff[256];
    struct i2c_msg msg[] = {
        [0] = {
        .addr = ft5x06_client->addr,
        .flags = 0,
        .len = len + 1,
        .buf = buff,
        },
    };
    buff[0] = reg;
    memcpy(&buff[1],&data,len);
    i2c_transfer(ft5x06_client->adapter,msg,1);
    return;
}

irqreturn_t ft5x06_handler(int irq, void *dev_id){
    schedule_work(&work);
    return IRQ_RETVAL(IRQ_HANDLED);
}

void work_handler(struct work_struct *work){
    //触摸屏的数据处理
    u8 TOUCH1_XH,TOUCH1_XL,TOUCH1_YH,TOUCH1_YL;

    int x,y;

    u8 TD_STATUS;

    TOUCH1_XH = ft5x06_read_meg(0x03);
    TOUCH1_XL = ft5x06_read_meg(0x04);

    x = ((TOUCH1_XH << 8) | TOUCH1_XL) & 0x0fff;

    TOUCH1_YH = ft5x06_read_meg(0x05);
    TOUCH1_YL = ft5x06_read_meg(0x06);

    y = ((TOUCH1_YH << 8) | TOUCH1_YL) & 0x0fff;
    
    //上报给input子系统
    TD_STATUS = ft5x06_read_meg(0x02);
    TD_STATUS = TD_STATUS & 0x0f;

    if(TD_STATUS == 0){
        input_report_key(ft5x06_input_dev,BTN_TOUCH,0);
        input_sync(ft5x06_input_dev);
    }else{
        input_report_key(ft5x06_input_dev,BTN_TOUCH,1);
        input_report_abs(ft5x06_input_dev,ABS_X,x);
        input_report_abs(ft5x06_input_dev,ABS_Y,y);
        input_sync(ft5x06_input_dev);
    }
}


int ft5x06_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    ft5x06_client = client;

    reset_gpio = gpiod_get_optional(&client->dev, "reset", 0);
    if (IS_ERR(reset_gpio)) {
        printk("Failed to get reset gpio\n");
        ret = PTR_ERR(reset_gpio);
        goto error0;
    }

    irq_gpio = gpiod_get_optional(&client->dev, "interrupts", 0);
    if (IS_ERR(irq_gpio)) {
        printk("Failed to get irq gpio\n");
        ret = PTR_ERR(irq_gpio);
        goto error1;
    }

    gpiod_direction_output(reset_gpio, 0);
    msleep(5);
    gpiod_set_value(reset_gpio, 1); //进行复位

    //初始化中断
    ret = request_irq(client->irq, ft5x06_handler, IRQF_TRIGGER_FALLING, "ft5x06_irq", NULL);
    if (ret < 0) {
        printk("Failed to request irq\n");
        goto error2;
    }

    ft5x06_input_dev = input_allocate_device();
    if (ft5x06_input_dev == NULL) {
        printk("Failed to allocate input device\n");
        ret = -ENOMEM;
        goto error3;
    }

    ft5x06_input_dev->name = "ft5x06_dev";

    set_bit(EV_KEY, ft5x06_input_dev->evbit);
    set_bit(BTN_TOUCH, ft5x06_input_dev->keybit);

    set_bit(EV_ABS, ft5x06_input_dev->evbit);
    set_bit(ABS_X, ft5x06_input_dev->absbit);
    set_bit(ABS_Y, ft5x06_input_dev->absbit);

    input_set_abs_params(ft5x06_input_dev, ABS_X, 0, 1024, 0, 0);
    input_set_abs_params(ft5x06_input_dev, ABS_Y, 0, 600, 0, 0);

    ret = input_register_device(ft5x06_input_dev);
    if (ret < 0) {
        printk("Failed to register input device\n");
        goto error4;
    }

    return 0;

error4:
    input_free_device(ft5x06_input_dev);
error3:
    free_irq(client->irq, NULL);
error2:
    gpiod_put(irq_gpio);
error1:
    gpiod_put(reset_gpio);
error0:
    return ret;
}

int ft5x06_remove(struct i2c_client *client)
{
    
    return 0;
}

struct of_device_id ft5x06__match_table[] = {
    { .compatible = "my-ft5x06" },
    { },
};

struct i2c_driver ft5x06_driver = {
    .driver = {
        .name = "my-ft5x06",
        .owner = THIS_MODULE,
        .of_match_table = ft5x06__match_table,
    },
    .probe = ft5x06_probe,
    .remove = ft5x06_remove,
};

static int __init ft5x06_driver_init(void)
{
    int ret;
    ret = i2c_add_driver(&ft5x06_driver);
    if(ret < 0){
        printk("ft5x06 driver register failed\n");
        return ret;
    } 
    return 0;
}

static void __exit ft5x06_driver_exit(void)
{
    input_free_device(ft5x06_input_dev);
    free_irq(ft5x06_client->irq, NULL);
    gpiod_put(irq_gpio);
    gpiod_put(reset_gpio);
    i2c_del_driver(&ft5x06_driver);
}

module_init(ft5x06_driver_init);
module_exit(ft5x06_driver_exit);

MODULE_LICENSE("GPL"); 