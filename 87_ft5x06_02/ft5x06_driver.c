#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

static struct gpio_desc *reset_gpio;
static struct gpio_desc *irq_gpio;

irqreturn_t ft5x06_handler(int irq, void *dev_id){
    printk("ft5x06 irq triggered\n");
    return IRQ_RETVAL(IRQ_HANDLED);
}


int ft5x06_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret;
    reset_gpio = gpiod_get_optional(&client->dev, "reset", 0);
    if(reset_gpio == NULL){
        printk("Failed to get reset gpio\n");
        return -1;
    }

    irq_gpio = gpiod_get_optional(&client->dev, "interrupts", 0);
    if(irq_gpio == NULL){
        printk("Failed to get irq gpio\n");
        return -1;
    }
    gpiod_direction_output(reset_gpio, 0);
    msleep(5);
    gpiod_set_value(reset_gpio, 1);//进行复位

    //初始化中断
    ret = request_irq(client->irq,ft5x06_handler,IRQF_TRIGGER_FALLING,"ft5x06_irq",NULL);
    if(ret < 0){
        printk("Failed to request irq\n");
        return ret;
    }

    return 0;
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
    i2c_del_driver(&ft5x06_driver);
}

module_init(ft5x06_driver_init);
module_exit(ft5x06_driver_exit);

MODULE_LICENSE("GPL"); 