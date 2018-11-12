#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Khoi Trinh");
MODULE_DESCRIPTION("A bmp280 driver for the raspbery pi");
MODULE_VERSION("0.1");

static char *name = "world";
module_param(name, charp, S_IRUGO);
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");

static int __init bmp280_kd_init(void) {
  printk(KERN_DEBUG "Hello %s from the bmp280 LKM!\n", name);
  return 0;
}

static void __exit bmp280_kd_exit(void) {
  printk(KERN_DEBUG "Goodbye %s from the bmp280 LKM!\n", name);
}

module_init(bmp280_kd_init);
module_exit(bmp280_kd_exit);
