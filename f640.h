/*
 * f640.h
 *
 *  Created on: Sep 11, 2011
 *  Author and copyright (C) 2011 : Gregory Kononovitch
 *  License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 */

#ifndef F640_H_
#define F640_H_

#define F640_RESET       "\033[0m"//(0)
#define F640_UNDER       "\033[4m"//(0)
#define F640_ERASE_LINE  "\033[2J"
#define F640_BOLD        "\033[1m"//(1)
#define F640_FG_BLACK    "\033[30m"//(30)
#define F640_FG_RED      "\033[31m"//(31)
#define F640_FG_GREEN    "\033[32m"//(32)
#define F640_FG_BROWN    "\033[33m"//(33)
#define F640_FG_BLUE     "\033[34m"//(34)
#define F640_FG_PURPLE   "\033[35m"//(35)
#define F640_FG_CYAN     "\033[36m"//(36)
#define F640_FG_GREY     "\033[37m"//(37)


// The SCALE macro converts a value (sv) from one range (sf -> sr)
//   to another (df -> dr).
#define SCALE(df, dr, sf, sr, sv) (((sv - sf) * (dr - df) / (sr - sf)) + df)


/*
 *
 */
typedef struct {
    void *start;
    size_t length;
} v4l2_buffer_t;


#endif /* F640_H_ */
