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
int c642_init(void)
{
    int r;
    ainit("bird", "pict", 32, A051_LOG_MEM, "C642", 4, 25);

    areturn(0, "ok");
}

/*
 *
 */
void c642_exit(void)
{
    astart(1, " ");

    aquitv("QUIT");
}

module_init(c642_init);
module_exit(c642_exit);

MODULE_AUTHOR("Gregory Kononovitch <gregory.kononovitch@gmail.com>");
MODULE_LICENSE("GPL");
