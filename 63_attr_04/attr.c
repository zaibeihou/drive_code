#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关
#include <linux/sysfs.h>       // sysfs文件系统相关


struct kobject *kobj01;


ssize_t my_show_value1(struct kobject *kobj, struct kobj_attribute *attr,char *buf){
    ssize_t count = 0;
    count = sprintf(buf,"%s\n",attr->attr.name);
    return count;
};

ssize_t	my_store_value1(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    printk("write value1\n");
    return count;
};

ssize_t	my_show_value2(struct kobject *kobj, struct kobj_attribute *attr, char *buf){
    ssize_t count = 0;
    count = sprintf(buf,"%s\n",attr->attr.name);
    return count;
};

ssize_t	my_store_value2(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count){
    printk("write value2\n");
    return count;
};

struct kobj_attribute value1 = __ATTR(value1, 0664, my_show_value1, my_store_value1);
struct kobj_attribute value2 = __ATTR(value2, 0664, my_show_value2, my_store_value2);

struct attribute *attr[] = {
    &value1.attr,
    &value2.attr,
    NULL,
};

const struct attribute_group attr_group = {
    .attrs = attr,
    .name = "my_group",
};

// 模块初始化函数
static int __init myobject_init(void) {
    int ret;//定义一个返回值
    kobj01 = kobject_create_and_add("my_kobject", NULL);//创建一个内核对象
    ret = sysfs_create_group(kobj01, &attr_group);//创建一个属性组
    return 0;
}

// 模块卸载函数
static void __exit myobject_exit(void) {
    kobject_put(kobj01);//释放内核对象

}

// 指定模块的入口和出口函数
module_init(myobject_init);
module_exit(myobject_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");