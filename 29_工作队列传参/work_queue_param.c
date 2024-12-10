#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

int irq;

struct data{
    struct work_struct work;
    int a;
    int b;
};

//static struct tasklet_struct my_tasklet;
static struct workqueue_struct *work_queue;
static struct data data;


void work_func(struct work_struct *work){
    struct data *p = container_of(work,struct data,work);
    msleep(1000);
    printk("this is tasklet_func\n");
    printk("%d\n",p->a);
    printk("%d\n",p->b);
}

irqreturn_t interrupt_func(int irq, void *args){
    printk("this is interrupt_func\n");
    //tasklet_schedule(&my_tasklet);
    //schedule_work(&work);
    queue_work(work_queue,&data.work);
    return IRQ_HANDLED;
}

static int interrupt_init(void)
{
    int ret;
    data.a = 1;
    data.b = 2;
    irq = gpio_to_irq(101);
    ret = request_irq(irq,interrupt_func,IRQF_TRIGGER_RISING,"interrupt_test",NULL);
    if(ret < 0){
        printk("request_irq error");
        return -1;
    }
    
    //tasklet_init(&my_tasklet,my_tasklet_func,0);
    work_queue = create_workqueue("work_queue");
    INIT_WORK(&data.work,work_func);

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