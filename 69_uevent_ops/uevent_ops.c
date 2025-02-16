#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/kobject.h>     // 内核对象相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // 配置文件系统相关

struct kobject *my_kobj01;//定义一个内核对象
struct kobject *my_kobj02;//定义一个内核对象
struct kobj_type my_ktype;//定义一个内核对象类型
struct kset *my_kset;//定义一个内核对象集合

int filter(struct kset *kset, struct kobject *kobj){
    if (strcmp(kobj->name, "my_kobj01") == 0){
        return 0;
    }
    else{
        return 1;
    }
};

const char *name(struct kset *kset, struct kobject *kobj){
    return "my_kobj02_why";
};

int uevent(struct kset *kset, struct kobject *kobj,struct kobj_uevent_env *env){
    add_uevent_var(env, "MYDEVPATH=%s", "my_kobj02");
    return 0;
};

struct kset_uevent_ops my_uevent_ops = {
    .filter = filter,
    .uevent = uevent,
    .name = name,
};

// 模块初始化函数
static int __init myobject_init(void) {
    int ret;//定义一个返回值

    my_kset = kset_create_and_add("my_kset", &my_uevent_ops,NULL);//创建一个内核对象集合
    
    my_kobj01 = kzalloc(sizeof(struct kobject), GFP_KERNEL);//分配内存
    my_kobj01->kset = my_kset;//设置内核对象的集合
    ret = kobject_init_and_add(my_kobj01, &my_ktype, NULL, "my_kobj01");
    //初始化并添加一个内核对象 他的名字是my_kobj03 他的父对象是NULL 他的类型是my_ktype

    my_kobj02 = kzalloc(sizeof(struct kobject), GFP_KERNEL);//分配内存
    my_kobj02->kset = my_kset;//设置内核对象的集合
    ret = kobject_init_and_add(my_kobj02, &my_ktype, NULL, "my_kobj02");
    //初始化并添加一个内核对象 他的名字是my_kobj03 他的父对象是NULL 他的类型是my_ktype
    kobject_uevent(my_kobj01, KOBJ_CHANGE);//发送一个内核对象事件
    kobject_uevent(my_kobj02, KOBJ_ADD);//发送一个内核对象事件
    
    return 0;

}

// 模块卸载函数
static void __exit myobject_exit(void) {
    kobject_put(my_kobj01);//释放内核对象
    kobject_put(my_kobj02);//释放内核对象
    kset_unregister(my_kset);//注销内核对象集合
}

// 指定模块的入口和出口函数
module_init(myobject_init);
module_exit(myobject_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");