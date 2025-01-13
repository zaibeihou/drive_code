#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关
#include <linux/sysfs.h>       // sysfs文件系统相关

struct my_kobj{
    struct kobject kobj;
    int value1;
    int value2;
};

struct my_kobj *my_kobj;//定义一个自定义内核对象


ssize_t my_show_value1(struct kobject *kobj, struct kobj_attribute *attr,char *buf){
    ssize_t count = 0;
    count = sprintf(buf,"%d\n",container_of(kobj, struct my_kobj, kobj)->value1);
    return count;
};

ssize_t	my_store_value1(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    sscanf(buf,"%d", &container_of(kobj, struct my_kobj, kobj)->value1);
    return count;
};

ssize_t	my_show_value2(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    ssize_t count = 0;
    count = sprintf(buf,"%d\n",container_of(kobj, struct my_kobj, kobj)->value2);
    return count;
};

ssize_t	my_store_value2(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    sscanf(buf,"%d", &container_of(kobj, struct my_kobj, kobj)->value2);
    return count;
};

struct kobj_attribute value1 = __ATTR(value1, 0664, my_show_value1, my_store_value1);
struct kobj_attribute value2 = __ATTR(value2, 0664, my_show_value2, my_store_value2);


void my_release(struct kobject *kobj){
    printk("my_release\n");
    kfree(kobj);
};

struct attribute *my_attr[] = {
    &value1.attr,
    &value2.attr,
    NULL,
};



ssize_t	my_show(struct kobject *kobj, struct attribute *attr, char *buf){
    ssize_t count = 0;
    struct kobj_attribute *kobj_attr = container_of(attr, struct kobj_attribute, attr);
    count = kobj_attr->show(kobj, kobj_attr, buf);
    
    return count;
};

ssize_t	my_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t size){
    struct kobj_attribute *kobj_attr = container_of(attr, struct kobj_attribute, attr);
    return kobj_attr->store(kobj, kobj_attr, buf, size);
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