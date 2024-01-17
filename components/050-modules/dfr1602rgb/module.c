#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include "display.h"

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define mem_size 1024 //Memory Size
static char *kernel_buffer;
static size_t cdev_pos;
static size_t cdev_currow;

static dev_t dev = 0;
static struct class *dev_class;
struct cdev cdev;

struct kobject *kobj_ref;

static int dfr1602rgb_open(struct inode *inode, struct file *file);
static int dfr1602rgb_release(struct inode *inode, struct file *file);
static ssize_t dfr1602rgb_read(struct file *filp, char __user *buf, size_t len,
			       loff_t *off);
static ssize_t dfr1602rgb_write(struct file *filp, const char *buf, size_t len,
				loff_t *off);

static ssize_t dfr1602rgb_sysfs_show(struct kobject *kobj,
				     struct kobj_attribute *attr, char *buf);
static ssize_t dfr1602rgb_sysfs_store(struct kobject *kobj,
				      struct kobj_attribute *attr,
				      const char *buf, size_t count);

struct kobj_attribute attr_color =
	__ATTR(color, 0660, dfr1602rgb_sysfs_show, dfr1602rgb_sysfs_store);

static int i2c_bus = 0;
module_param(i2c_bus, int, 0644);
MODULE_PARM_DESC(i2c_bus, "I2C bus number (default=0)");

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = dfr1602rgb_read,
	.write = dfr1602rgb_write,
	.open = dfr1602rgb_open,
	.release = dfr1602rgb_release,
};

static ssize_t dfr1602rgb_sysfs_show(struct kobject *kobj,
				     struct kobj_attribute *attr, char *buf)
{
	pr_err("Not supported\n");
	return 0;
}

static ssize_t dfr1602rgb_sysfs_store(struct kobject *kobj,
				      struct kobj_attribute *attr,
				      const char *buf, size_t count)
{
	union {
		u32 rgba;
		struct {
			u8 r;
			u8 g;
			u8 b;
			u8 a;
		};
	} color;

	if (sscanf(buf, "#%2hhx%2hhx%2hhx\n", &color.r, &color.g, &color.b) ==
	    3) {
		display_set_rgb(color.r, color.g, color.b);
	} else {
		pr_err("Wrong format\n");
		return 0;
	}
	return count;
}

static int dfr1602rgb_open(struct inode *inode, struct file *file)
{
	if ((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0) {
		pr_err("Cannot allocate memory in kernel\n");
		return -1;
	}
	cdev_pos = 0;
	cdev_currow = 0;
	display_clear();

	return 0;
}

/*
** This function will be called when we close the Device file
*/
static int dfr1602rgb_release(struct inode *inode, struct file *file)
{
	kfree(kernel_buffer);
	return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t dfr1602rgb_read(struct file *filp, char __user *buf, size_t len,
			       loff_t *off)
{
	static const char *help =
		"This is the help of usage\n"
		"use insmod dfr1602rgb.ko busno=N to specify I2C bus number\n"
		"use /dev/dfr1602rgb to write new message to the diplay\n"
		"use /sys/kernel/dfr1602rgb/color to change a color (hexadecimal colour string i.e. 0xFFFFFF)\n";
	size_t help_length = strlen(help);
	size_t to_read = min(len, help_length - cdev_pos);

	if (copy_to_user(buf, help + cdev_pos, to_read)) {
		pr_err("Data Read : Err!\n");
		return 0;
	}
	cdev_pos += to_read;

	return to_read;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t dfr1602rgb_write(struct file *filp, const char __user *buf,
				size_t len, loff_t *off)
{
	size_t to_write = min(mem_size - cdev_pos, len);
	char *this_line, *next_line;

	// Copy the data to kernel space from the user-space
	if (copy_from_user(kernel_buffer + cdev_pos, buf, len)) {
		pr_err("Data Write : Err!\n");
		return 0;
	}
	kernel_buffer[cdev_pos + to_write] = '\0';
	this_line = kernel_buffer + cdev_pos;

	while (*(next_line = strchrnul(this_line, '\n')) != '\0') {
		*next_line = '\0';
		display_set_cursor(0, cdev_currow++);
		display_print(this_line);
		this_line = next_line + 1;
	}
	cdev_pos += to_write;
	return to_write;
}

/*
** Module Init function
*/
static int
#ifndef DEBUG
	__init
#endif
	dfr1602rgb_driver_init(void)
{
	/*Allocating Major number*/
	if ((alloc_chrdev_region(&dev, 0, 1, "dfr1602rgb")) < 0) {
		pr_info("Cannot allocate major number\n");
		return -1;
	}

	/*Creating cdev structure*/
	cdev_init(&cdev, &fops);

	/*Adding character device to the system*/
	if ((cdev_add(&cdev, dev, 1)) < 0) {
		pr_info("Cannot add the device to the system\n");
		goto r_class;
	}

	/*Creating struct class*/
	if (
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
		(dev_class = class_create(THIS_MODULE, "dfr1602rgb_class")
#else
		(dev_class = class_create("dfr1602rgb_class")
#endif
			 ) == NULL) {
		pr_info("Cannot create the struct class\n");
		goto r_class;
	}

	/*Creating device*/
	if ((device_create(dev_class, NULL, dev, NULL, "dfr1602rgb")) == NULL) {
		pr_info("Cannot create the Device\n");
		goto r_device;
	}

	/*Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add("dfr1602rgb", kernel_kobj);

	/*Creating sysfs file for etx_value*/
	if (sysfs_create_file(kobj_ref, &attr_color.attr)) {
		pr_err("Cannot create sysfs file.\n");
		goto r_sysfs;
	}

	display_init(i2c_bus);

	pr_info("dfr1602rgb initialization done\n");
	return 0;

r_sysfs:
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &attr_color.attr);

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	cdev_del(&cdev);
	return -1;
}

/*
** Module exit function
*/
static void __exit dfr1602rgb_driver_exit(void)
{
	display_deinit();

	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &attr_color.attr);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("dfr1602rgb deinitialization done\n");
}

module_init(dfr1602rgb_driver_init);
module_exit(dfr1602rgb_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Zhadinets <dzhadinets@gmail.com>");
MODULE_DESCRIPTION("Char device to work with DfRobot 16x2 LCD1602 with RGB");
MODULE_VERSION("0.1");
