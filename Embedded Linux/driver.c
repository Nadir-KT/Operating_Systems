#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nadir");
MODULE_DESCRIPTION("DRIVER");
MODULE_VERSION("1.0");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	printk("Helo, Kernel!\n");
	return 0;
}
/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	printk("Goodbye, Kernel!\n");
	return 0;
}

module_init(ModuleInit);
module_exit(ModuleExit);