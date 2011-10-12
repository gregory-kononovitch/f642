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

/*
 *
 */
struct c642_pict *pict;
int c642_init(void)
{
    _num_ d1, d2, d3, d4, r;
    u32 rnd = random32();
    cpoint p1, p2;
    printk("r = %u / %lu\n", rnd, __fls(rnd));

    //
    d1.plin = 123;  d1.frac = 0x80000000;
    d2.plin = 121;  d2.frac = 0xC0000000;

    // ADD1
    _add_(&d1, &d2, &r);
    printk("123.8 + 121.C = %X.%X\n", r.plin, r.frac);

    // SUB1
    _sub_(&d1, &d2, &r);
    printk("123.8 - 121.C = %X.%X\n", r.plin, r.frac);

    // SUB2
    d1.plin = 1;  d1.frac = 0xAAAAAAAA;
    d2.plin = 0;  d2.frac = 0x55555555;
    _sub_(&d1, &d2, &r);
    printk("1.66 - 0.33 = %X.%X\n", r.plin, r.frac);

    // MUL1
    d1.plin = 1;  d1.frac = 0x7FFFFFFF;
    d2.plin = 2;  d2.frac = 0x55555555;
    _mul_(&d1, &d2, &r);
    printk("1.5 x 2.3 = %X.%X\n", r.plin, r.frac);

    // DIV1
    d1.plin = 1;  d1.frac = 0x55555555;
    d2.plin = 2;  d2.frac = 0x7FFFFFFF;
    _div_(&d1, &d2, &r);
    printk("1.5 / 0.5 = %X.%X\n", r.plin, r.frac);


    // Dist
    //
    p1.x.plin = 1;  p1.x.frac = 0x80000000;
    p1.y.plin = 1;  p1.y.frac = 0xC0000000;
    p2.x.plin = 5;  p2.x.frac = 0x50000000;
    p2.y.plin = 6;  p2.y.frac = 0xF0000000;

    _dist_(&p1, &p2, &r);
    printk("dist = %X.%X\n", r.plin, r.frac);

    return -EAGAIN;

    //
//    pict = NULL;
//    pict = c642_create(50, 50);
//    if (!pict) {
//        printk("Unable to create pict, exiting.\n");
//        return -ENOMEM;
//    }
//
//    //
//    brodge(pict);

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
//    c642_destroy(pict);
    //aquitv("QUIT");
}

module_init(c642_init);
module_exit(c642_exit);

MODULE_AUTHOR("Gregory Kononovitch <gregory.kononovitch@gmail.com>");
MODULE_LICENSE("GPL");
