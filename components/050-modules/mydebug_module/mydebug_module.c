#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry Zhadinets" "Vladimir Yakovlev");
MODULE_DESCRIPTION("A simple Linux module to debug.");
MODULE_VERSION("0.1");

u64 param;
#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
static int mydebug_module_parse_param(const char *str, u64 *result);

int mydebug_module_cdev_init(void);
int mydebug_module_cdev_release(void);

#if IS_ENABLED(CONFIG_MYDEBUG_MODULE_PARAM)
static char str_param[128];
module_param_string(param, str_param, sizeof(str_param), S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(param, "Formatted param value regex:[0-9]+[KMGT]?");
#endif

#if IS_ENABLED(CONFIG_MYDEBUG_MODULE_CONFIGURATION)

struct kobject *kobj_ref;

/*************** Sysfs functions **********************/
static ssize_t mydebug_module_sysfs_show(struct kobject *kobj,
					 struct kobj_attribute *attr,
					 char *buf);
static ssize_t mydebug_module_sysfs_store(struct kobject *kobj,
					  struct kobj_attribute *attr,
					  const char *buf, size_t count);
struct kobj_attribute attr_param = __ATTR(
	param, 0660, mydebug_module_sysfs_show, mydebug_module_sysfs_store);

/*
** This function will be called when we read the sysfs file
*/
static ssize_t mydebug_module_sysfs_show(struct kobject *kobj,
					 struct kobj_attribute *attr, char *buf)
{
	pr_debug("Sysfs - Read\n");
	return sprintf(buf, "%llu\n", param);
}
/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t mydebug_module_sysfs_store(struct kobject *kobj,
					  struct kobj_attribute *attr,
					  const char *buf, size_t count)
{
	pr_debug("Sysfs - Write\n");
	if (mydebug_module_parse_param(buf, &param) != 0) {
		return -1;
	}
	return count;
}
#endif

static int mydebug_module_parse_param(const char *str, u64 *result)
{
	size_t i;
	size_t len = strlen(str);
	*result = 0;

#if 1
	while (len > 0 && str[len - 1] == '\n') {
		len--;
	}
#endif

	for (i = 0; i < len; i++) {
		if (!ISDIGIT(str[i])) {
			if (i == 0 || i != len - 1) {
				pr_err("Wrong param format\n");
				return -1;
			}
			switch (str[i]) {
			case 'K':
				*result <<= 10;
				break;
			case 'M':
				*result <<= 20;
				break;
			case 'G':
				*result <<= 30;
				break;
			case 'T':
				*result <<= 40;
				break;
			default:
				pr_err("Unknown identifier\n");
				return -1;
			}
		} else {
			*result = *result * 10 + (u8)(str[i] - '0');
		}
	}
	return 0;
}

/*
** This function will be called during module initialization
*/
static int
#ifndef DEBUG
	__init
#endif
	mydebug_module_init(void)
{
	pr_info("Hello, Linux!\n");

#if IS_ENABLED(CONFIG_MYDEBUG_MODULE_CONFIGURATION)

	/*Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add("mydebug_module", kernel_kobj);

	/*Creating sysfs file for param*/
	if (sysfs_create_file(kobj_ref, &attr_param.attr)) {
		pr_err("Cannot create sysfs file.\n");
		kobject_put(kobj_ref);
		sysfs_remove_file(kernel_kobj, &attr_param.attr);
		return -1;
	}
#endif

#if IS_ENABLED(CONFIG_MYDEBUG_MODULE_PARAM)
	if (mydebug_module_parse_param(str_param, &param) != 0) {
		return -1;
	}
#else
	param = CONFIG_MYDEBUG_MODULE_PARAM_VALUE;
#endif

	mydebug_module_cdev_init();

	pr_info("Module parameter is %llu\n", param);
	return 0;
}

/*
** This function will be called during module deinitialization
*/
static void
#ifndef DEBUG
	__exit
#endif
	mydebug_module_exit(void)
{
	mydebug_module_cdev_release();

#if IS_ENABLED(CONFIG_MYDEBUG_MODULE_CONFIGURATION)
	kobject_put(kobj_ref);
	sysfs_remove_file(kernel_kobj, &attr_param.attr);
#endif
	pr_info("Goodbye, Linux!\n");
}

module_init(mydebug_module_init);
module_exit(mydebug_module_exit);
