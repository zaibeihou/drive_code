#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关

struct kobject *my_kobj01;//定义一个内核对象
struct kobject *my_kobj02;
struct kobject *my_kobj03;

struct kobj_type my_ktype;//定义一个内核对象类型


// 模块初始化函数
static int __init myobject_init(void) {
    int ret;//定义一个返回值

    //第一种创建内核对象的方法
    my_kobj01 = kobject_create_and_add("my_kobj01", NULL);//创建一个内核对象
    printk("my kobject 01 kref is %d\n", my_kobj01->kref.refcount.refs.counter);//打印引用计数
    my_kobj02 = kobject_create_and_add("my_kobj02", my_kobj01);//创建一个内核对象 他的父对象是my_kobj01
    printk("my kobject 02 kref is %d\n", my_kobj02->kref.refcount.refs.counter);//打印引用计数
    //第二种创建内核对象的方法
    my_kobj03 = kzalloc(sizeof(struct kobject), GFP_KERNEL);//分配内存
    ret = kobject_init_and_add(my_kobj03, &my_ktype, NULL, "my_kobj03");
    //初始化并添加一个内核对象 他的名字是my_kobj03 他的父对象是NULL 他的类型是my_ktype
    printk("my kobject 03 kref is %d\n", my_kobj03->kref.refcount.refs.counter);//打印引用计数
    return 0;
}

// 模块卸载函数
static void __exit myobject_exit(void) {
    kobject_put(my_kobj01);//释放内核对象
    printk("my kobject 01 kref is %d\n", my_kobj01->kref.refcount.refs.counter);//打印引用计数
    printk("my kobject 02 kref is %d\n", my_kobj02->kref.refcount.refs.counter);//打印引用计数
    printk("my kobject 03 kref is %d\n", my_kobj03->kref.refcount.refs.counter);//打印引用计数
    kobject_put(my_kobj02);
    printk("my kobject 01 kref is %d\n", my_kobj01->kref.refcount.refs.counter);//打印引用计数
    printk("my kobject 02 kref is %d\n", my_kobj02->kref.refcount.refs.counter);//打印引用计数
    printk("my kobject 03 kref is %d\n", my_kobj03->kref.refcount.refs.counter);//打印引用计数
    kobject_put(my_kobj03);
    printk("my kobject 01 kref is %d\n", my_kobj01->kref.refcount.refs.counter);//打印引用计数
    printk("my kobject 02 kref is %d\n", my_kobj02->kref.refcount.refs.counter);//打印引用计数
    printk("my kobject 03 kref is %d\n", my_kobj03->kref.refcount.refs.counter);//打印引用计数
}

// 指定模块的入口和出口函数
module_init(myobject_init);
module_exit(myobject_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");