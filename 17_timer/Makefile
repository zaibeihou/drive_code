# 定义模块名称
MODULE_NAME := timer

# 编译的目标文件
obj-m := $(MODULE_NAME).o

# 使用当前系统的内核源码路径
KDIR := /lib/modules/$(shell uname -r)/build

# 当前目录路径
PWD ?= $(shell pwd)  # 这里使用 ?= 是可以的

# 编译目标
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# 清理目标
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
