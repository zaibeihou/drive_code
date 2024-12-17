#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

int irq;
//static struct tasklet_struct my_tasklet;
static struct work_struct work;

void work_func(struct work_struct *work){
    printk("this is tasklet_func\n");
}

irqreturn_t interrupt_func(int irq, void *args){
    printk("this is interrupt_func\n");
    //tasklet_schedule(&my_tasklet);
    schedule_work(&work);
    return IRQ_HANDLED;
}

static int interrupt_init(void)
{
    int ret;

    irq = gpio_to_irq(101);
    ret = request_irq(irq,interrupt_func,IRQF_TRIGGER_RISING,"interrupt_test",NULL);
    if(ret < 0){
        printk("request_irq error");
        return -1;
    }
    
    //tasklet_init(&my_tasklet,my_tasklet_func,0);
    INIT_WORK(&work,work_func);

    return 0;
}

static void interrupt_exit(void)      // 驱动出口函数
{
    printk("helloworld_exit\n");
    //tasklet_enable(&my_tasklet);
    //tasklet_kill(&my_tasklet);
    free_irq(irq,NULL);

}

module_init(interrupt_init);
module_exit(interrupt_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");