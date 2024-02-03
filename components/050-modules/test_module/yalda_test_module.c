/*
  This module is used for a self-test of YALDA. To check an ability to connect
  with debugger, follow breakpoints and set/get variables
  The idea is that when we insert this module we connected with gdb and set
  break on init. By default the module must print "Hello YALDA from gdb 0!"
  But gdb changes the value of gdb_var to 1 and we will see another output
*/

#include <linux/module.h>

static int gdb_var=0;

static int yalda_init_module(void)
{
	printk("Hello YALDA from gdb %d!\n", gdb_var);
	return 0;
}

static void yalda_cleanup_module(void)
{
	printk("Goodbye YALDA!\n");
}

module_init(yalda_init_module);
module_exit(yalda_cleanup_module);

MODULE_LICENSE("GPL");
