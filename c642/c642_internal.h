/*
 * c642_internal.h
 *
 * Date Oct 11, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef C642_INTERNAL_H_
#define C642_INTERNAL_H_

#include <linux/module.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include <linux/err.h>
#include <linux/list.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>

#include <linux/firmware.h>
//
#define A051_PROFILE
#define A051_PROFILE_MEM
//#define A051_PROFILE_TXT
extern char   a051_code[17];
extern struct a051_proc_env *a051_env;
extern struct a051_log_env  *a051_log;
#include "../../a051/src/a051_profile.h"

//
struct _numb_tmp_ {
    u32 frac;
    u32 plin;
};
typedef struct _numb_tmp_ _num_;
extern void _add_(_num_ *d1, _num_ *d2, _num_ *r);
extern void _sub_(_num_ *d1, _num_ *d2, _num_ *r);
extern void _mul_(_num_ *d1, _num_ *d2, _num_ *r);
extern void _div_(_num_ *d1, _num_ *d2, _num_ *r);

struct tmp1 {
    _num_ x;
    _num_ y;
};
typedef struct tmp1 cpoint;
extern void _dist_(cpoint *p1, cpoint *p2, _num_ *r);

// Brodge
struct c642_pict;
extern struct c642_pict *c642_create(int width, int height);
extern void brodge(struct c642_pict *pict);
extern void c642_destroy(struct c642_pict *pict);


#endif /* C642_INTERNAL_H_ */
