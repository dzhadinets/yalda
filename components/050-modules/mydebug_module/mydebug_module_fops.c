#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

extern u64 param;

static struct class *mydebug_class;
static struct cdev mydebug_cdev;
dev_t mydebug_dev = 0;

#define WR_VALUE _IOW('a','a',u_int64_t*)
#define RD_VALUE _IOR('a','b',u_int64_t*)

static int mydebug_module_open(struct inode *inode, struct file *file)
{
	pr_debug("mydebug_module_open\n");

	return 0;
}

static ssize_t mydebug_module_read(struct file *file, char __user *user_buffer,
				   size_t size, loff_t *offset)
{
	pr_debug("mydebug_module_read size=%ld\n", size);

	if ((size < sizeof(param))) {
		pr_err("size=%ld error\n", size);
		return -EFAULT;
	}

	if ( copy_to_user((u_int64_t*) user_buffer, &param, sizeof(param)) ) {
		pr_err("Data Read : Err!\n");
		return -EFAULT;
	}

	return size;
}

static ssize_t mydebug_module_write(struct file *file, const char __user *user_buffer,
					size_t size, loff_t * offset)
{
	pr_debug("mydebug_module_write size=%ld\n", size);

	if ((size != sizeof(param))) {
		pr_err("size=%ld error\n", size);
		return -EFAULT;
	}

	if ( copy_from_user(&param ,(u_int64_t*) user_buffer, sizeof(param)) ) {
		pr_err("data write error!\n");
		return -EFAULT;
	}

	pr_debug("new val = %lld\n", param);

	return size;
}

static long mydebug_module_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	pr_debug("mydebug_module_ioctl cmd=%d\n", cmd);

	switch(cmd) {
		case WR_VALUE:
		   if( copy_from_user(&param ,(u_int64_t*) arg, sizeof(param)) ) {
				pr_err("data write error!\n");
		   }
		   pr_debug("new val = %lld\n", param);
		   break;
	   case RD_VALUE:
		   if( copy_to_user((u_int64_t*) arg, &param, sizeof(param)) ) {
				pr_err("EFAULT read error!\n");
		   }
		   break;
	   default:
		   pr_warn("default\n");
		   break;
	}
	return 0;
}

static int mydebug_module_release(struct inode *inode, struct file *file)
{
	pr_debug("mydebug_module_release\n");

	return 0;
}

const struct file_operations mydebug_module_fops = {
	.owner = THIS_MODULE,
	.open = mydebug_module_open,
	.read = mydebug_module_read,
	.write = mydebug_module_write,
	.release = mydebug_module_release,
	.unlocked_ioctl = mydebug_module_ioctl
};

int mydebug_module_cdev_init(void)
{
	/*Allocating Major number*/
	if((alloc_chrdev_region(&mydebug_dev, 0, 1, "mydebug_Dev")) <0){
			pr_err("Cannot allocate major number\n");
			return -1;
	}
	pr_info("MyDebug Major = %d Minor = %d \n",MAJOR(mydebug_dev), MINOR(mydebug_dev));

	/*Creating cdev structure*/
	cdev_init(&mydebug_cdev,&mydebug_module_fops);

	/*Adding character device to the system*/
	if((cdev_add(&mydebug_cdev,mydebug_dev,1)) < 0){
		pr_err("Cannot add the device to the system\n");
		goto err_class;
	}

	/*Creating struct class*/
	if(IS_ERR(mydebug_class = class_create(THIS_MODULE,"mydebug_class"))){
		pr_err("Cannot create the struct class\n");
		goto err_class;
	}

	/*Creating device*/
	if(IS_ERR(device_create(mydebug_class,NULL,mydebug_dev,NULL,"mydebug_device"))){
		pr_err("Cannot create the Device 1\n");
		goto err_device;
	}
	pr_info("Device Driver Insert...Done!!!\n");

	return 0;
err_device:
	class_destroy(mydebug_class);
err_class:
	unregister_chrdev_region(mydebug_dev,1);
	return -1;
}

int mydebug_module_cdev_release(void)
{
	device_destroy(mydebug_class,mydebug_dev);
	class_destroy(mydebug_class);
	cdev_del(&mydebug_cdev);
	unregister_chrdev_region(mydebug_dev, 1);
	pr_info("Device Driver Remove...Done!!!\n");
	return 0;
}
