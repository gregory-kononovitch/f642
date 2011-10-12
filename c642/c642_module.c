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



#include <asm/bitops.h>
#include <linux/random.h>
#include <linux/firmware.h>
/*
 *
 */
const struct firmware *fm;
signed short *cosinus = NULL;
struct c642_pict *pict;



/*
 * 512 for a loop
 */
int c642_cos(_frac_ *res, long num, u64 den)
{
    int i;

    if (cosinus == NULL || den == 0) {
        return -EFAULT;
    }
    //
    num = num < 0 ? -num % den : num % den;
    i = (int) ( 512L * num / den );
    res->num = cosinus[i];
    res->den = 0x7FFF;

    return 0;
}

int c642_sin(_frac_ *res, long num, u64 den)
{
    int i;

    if (cosinus == NULL || den == 0) {
        return -EFAULT;
    }
    //
    num = num < 0 ? -num % den : num % den;
    i = (int) ( 512L * num / den );
    i = ( i + 128 ) % 512;
    res->num = cosinus[i];
    res->den = 0x7FFF;

    return 0;
}








int c642_init(void)
{
    int r;

    // Trigo
    r = request_firmware(&fm, "c642-trig.bin", NULL);
    if (r) {
        cosinus = NULL;
        printk("Loading trigonom numbers failed, miss something.\n");
    } else {
        cosinus = kmalloc(fm->size, GFP_KERNEL);
        if (!cosinus) {
            printk("Unabled to kmalloc.\n");
            release_firmware(fm);
        }
        memcpy(cosinus, fm->data, fm->size);
        printk("Trigonom numbers loaded.\n");

        release_firmware(fm);
    }

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
