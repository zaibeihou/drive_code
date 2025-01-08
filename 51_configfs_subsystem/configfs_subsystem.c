#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/configfs.h>


static const struct config_item_type myconfigfs_item_type = {
    .ct_owner = THIS_MODULE,
    .ct_item_ops = NULL,
    .ct_attrs = NULL,
    .ct_group_ops = NULL, 
};

static struct configfs_subsystem myconfigfs_subsys = {
    .su_group = {
        .cg_item = {
            .ci_namebuf = "myconfigfs",
            .ci_type = &myconfigfs_item_type,
        }
    }
};


// 模块加载函数
static int __init myconfigfs_init(void)
{
    config_group_init(&myconfigfs_subsys.su_group);
    configfs_register_subsystem(&myconfigfs_subsys); 

    return 0;
}

// 模块卸载函数
static void __exit myconfigfs_exit(void)  
{
    configfs_unregister_subsystem(&myconfigfs_subsys);

}

module_init(myconfigfs_init);
module_exit(myconfigfs_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("why");