#include <linux/module.h>      // 模块相关头文件
#include <linux/kernel.h>      // 内核相关
#include <linux/init.h>        // 初始化相关
#include <linux/slab.h>        // 内存分配相关
#include <linux/configfs.h>    // configfs文件系统相关

// 声明一个全局的配置组,用于后续注册
static struct config_group my_group;

// 定义我们自己的item结构体
// 必须包含一个config_item作为第一个成员
struct myitem {
    struct config_item item;   // 继承自config_item
    // 这里可以添加自己的其他成员
};

void myitem_release(struct config_item *item){
    // 释放item的处理函数
    struct myitem *myitem = container_of(item, struct myitem, item);
    printk("myitem_release\n");
    
    kfree(myitem);

};

struct configfs_item_operations my_item_ops = {
    .release = myitem_release,  // 释放item的处理函数
};

static const struct config_item_type my_item_type = {
    .ct_owner = THIS_MODULE,
    .ct_item_ops = &my_item_ops,
};



// 当用户在configfs中创建新item时会调用此函数
struct config_item *mygroup_make_item(struct config_group *group, const char *name) {
    struct myitem *myitem;
    
    // 分配一个新的myitem结构体
    myitem = kzalloc(sizeof(*myitem), GFP_KERNEL);
    
    // 初始化这个item,设置其名字和类型
    config_item_init_type_name(&myitem->item, name, &my_item_type);
    return &myitem->item;
}

// 定义group的操作函数集
struct configfs_group_operations mygroup_ops = {
    .make_item = mygroup_make_item,  // 指定创建item的处理函数
};

// 定义mygroup的类型信息
static const struct config_item_type mygroup_item_type = {
    .ct_owner = THIS_MODULE,         // 指定所属模块
    .ct_item_ops = NULL,            // item的操作函数(这里未定义)
    .ct_attrs = NULL,               // 属性(这里未定义)
    .ct_group_ops = &mygroup_ops,   // 组操作函数
};

// 定义顶层item的类型信息
static const struct config_item_type myconfigfs_item_type = {
    .ct_owner = THIS_MODULE,
    .ct_group_ops = NULL,  // 顶层不需要group操作
};

// 定义configfs子系统
// 这是整个层次结构的根
static struct configfs_subsystem myconfigfs_subsys = {
    .su_group = {
        .cg_item = {
            .ci_namebuf = "myconfigfs",  // 子系统名称
            .ci_type = &myconfigfs_item_type,  // 指定类型
        }
    }
};

// 模块初始化函数
static int __init myconfigfs_init(void) {
    // 初始化子系统的group
    config_group_init(&myconfigfs_subsys.su_group);
    // 注册子系统
    configfs_register_subsystem(&myconfigfs_subsys);
    
    // 初始化my_group,设置其名称和类型
    config_group_init_type_name(&my_group, "mygroup", &mygroup_item_type);
    // 将my_group注册到子系统下
    configfs_register_group(&myconfigfs_subsys.su_group, &my_group);
    return 0;
}

// 模块卸载函数
static void __exit myconfigfs_exit(void) {
    // 注销子系统
    configfs_unregister_subsystem(&myconfigfs_subsys);
}

// 指定模块的入口和出口函数
module_init(myconfigfs_init);
module_exit(myconfigfs_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");