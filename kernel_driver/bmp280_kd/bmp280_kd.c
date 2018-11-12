#include <linux/init.h>    // Macros used to mark up functions e.g., __init __exit
#include <linux/kernel.h>  // Contains types, macros, functions for the kernel
#include <linux/module.h>  // Core header for loading LKMs into the kernel

MODULE_LICENSE("GPL");        ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Khoi Trinh");  ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A bmp280 driver for the raspbery pi");  ///< The description -- see modinfo
MODULE_VERSION("0.1");                                      ///< The version of the module

static char *name = "world";  ///< An example LKM argument -- default value is "world"
module_param(name,
             charp,
             S_IRUGO);  ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name,
                 "The name to display in /var/log/kern.log");  ///< parameter description

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C
 * file. The __init macro means that for a built-in driver (not a LKM) the
 * function is only used at initialization time and that it can be discarded and
 * its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init bmp280_kd_init(void) {
  printk(KERN_DEBUG "Hello %s from the bmp280 LKM!\n", name);
  return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro
 * notifies that if this code is used for a built-in driver (not a LKM) that
 * this function is not required.
 */
static void __exit bmp280_kd_exit(void) {
  printk(KERN_DEBUG "Goodbye %s from the bmp280 LKM!\n", name);
}

/** @brief A module must use the module_init() module_exit() macros from
 * linux/init.h, which identify the initialization function at insertion time
 * and the cleanup function (as listed above)
 */
module_init(bmp280_kd_init);
module_exit(bmp280_kd_exit);
