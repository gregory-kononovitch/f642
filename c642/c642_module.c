/*
 * c642_module.c
 *
 * Date Oct 11, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "c642_internal.h"

char a051_code[17];
struct a051_proc_env *a051_env = NULL;
struct a051_log_env *a051_log = NULL;





/*
 *
 */
struct c642_pict *pict;
int c642_init(void)
{
    //ainit("bird", "c642", 32, A051_LOG_MEM, "C642", 4, 25);

    //
    pict = NULL;
    pict = c642_create(1024, 600);
    if (!pict) {
        printk("Unable to create pict, exiting.\n");
        return -ENOMEM;
    }

    //
    brodge(pict);

    //
    //areturn(-EBUSY, "ok");
    return 0;
}

/*
 *
 */
void c642_exit(void)
{
    //astart(1, " ");
    c642_destroy(pict);
    //aquitv("QUIT");
}

module_init(c642_init);
module_exit(c642_exit);

MODULE_AUTHOR("Gregory Kononovitch <gregory.kononovitch@gmail.com>");
MODULE_LICENSE("GPL");
