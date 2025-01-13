#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关

struct my_kobj{
    struct kobject kobj;
    int value1;
    int value2;
};

struct my_kobj *my_kobj;//定义一个自定义内核对象

void my_release(struct kobject *kobj){
    printk("my_release\n");
    kfree(kobj);
};

struct attribute value1 = {
    .name = "value1",
    .mode = 0666,
};
struct attribute value2 = {
    .name = "value2",
    .mode = 0666,
};

struct attribute *my_attr[] = {
    &value1,
    &value2,
    NULL,
};



ssize_t	my_show(struct kobject *kobj, struct attribute *attr, char *buf){
    struct my_kobj *my_kobj = container_of(kobj,struct my_kobj,kobj);
    ssize_t count = 0;
    if(strcmp(attr->name, "value1")== 0 ){
        count = sprintf(buf, "%d\n", my_kobj->value1);
        return count;
    }else if(strcmp(attr->name, "value2")== 0 ){
        count = sprintf(buf, "%d\n", my_kobj->value2);
        return count;
    }
    return -EINVAL;// 如果属性名称不匹配，返回一个错误码
};

ssize_t	my_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t size){
    struct my_kobj *my_kobj = container_of(kobj,struct my_kobj,kobj);
    if(strcmp(attr->name, "value1")== 0 ){
        sscanf(buf, "%d", &my_kobj->value1);
    }else if(strcmp(attr->name, "value2")== 0 ){
        sscanf(buf, "%d", &my_kobj->value2);
    }
    return size;
};

const struct sysfs_ops my_ops = {
    .show = my_show,
    .store = my_store,
};

struct kobj_type my_ktype = {
    .release = my_release,
    .default_attrs = my_attr,
    .sysfs_ops = &my_ops,
};

// 模块初始化函数
static int __init myobject_init(void) {
    int ret;//定义一个返回值

    my_kobj = kzalloc(sizeof(struct my_kobj), GFP_KERNEL);//分配内存
    my_kobj->value1 = 1;
    my_kobj->value2 = 2;

    ret = kobject_init_and_add(&my_kobj->kobj, &my_ktype, NULL, "my_kobj");

    return 0;
}

// 模块卸载函数
static void __exit myobject_exit(void) {
    kobject_put(&my_kobj->kobj);//释放内核对象

}

// 指定模块的入口和出口函数
module_init(myobject_init);
module_exit(myobject_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");