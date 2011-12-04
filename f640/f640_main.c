/*
 * f640_main.c
 *
 *  Created on: Sep 11, 2011
 *  Author and copyright (C) 2011 : Gregory Kononovitch
 *  License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <f051_profile.h>

#include "f640.h"


/*
 * 
 */
static struct f051_log_env *log_env;
static char device[32];
static char *dev = "/dev/video0";
static int fd;
static struct v4l2_capability cap;
static char *source = "/dev/video0";
static char *input = NULL;
static int cwidth = 320;
static int cheight = 240;
static uint32_t palette = 0x56595559;   // 0x47504A4D
static struct v4l2_format format;
static struct v4l2_fmtdesc format_desc;
static int nb_buffers = 10;
static char map;
static struct v4l2_requestbuffers req;
static struct v4l2_buffer buf;
static v4l2_buffer_t *buffer;
static int loop;

static int capture          = 1;
static uint32_t nb_frames   = 0;
static uint32_t frames_pers = 10;
static int verbose          = 0;
static int quiet            = 0;
static int debug            = 0;
static int show_all         = 0;
static int show_caps        = 0;
static int show_inputs      = 0;
static int show_controls    = 0;
static int show_formats     = 0;
static int show_framsize    = 0;
static int show_framints    = 0;
static int show_vidstds     = 0;


/*
 *
 */
static int f640_get_capabilities()
{
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        printf("%s: Not a V4L2 device?\n", dev);
        return -1;
    }

    if ( show_all || show_caps ) {
        printf("%s information:\n", dev);
        printf("cap.driver: \"%s\"\n", cap.driver);
        printf("cap.card: \"%s\"\n", cap.card);
        printf("cap.bus_info: \"%s\"\n", cap.bus_info);
        printf("cap.capabilities=0x%08X\n", cap.capabilities);
        if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) printf("- VIDEO_CAPTURE\n");
        if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)  printf("- VIDEO_OUTPUT\n");
        if(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY) printf("- VIDEO_OVERLAY\n");
        if(cap.capabilities & V4L2_CAP_VBI_CAPTURE)   printf("- VBI_CAPTURE\n");
        if(cap.capabilities & V4L2_CAP_VBI_OUTPUT)    printf("- VBI_OUTPUT\n");
        if(cap.capabilities & V4L2_CAP_RDS_CAPTURE)   printf("- RDS_CAPTURE\n");
        if(cap.capabilities & V4L2_CAP_TUNER)         printf("- TUNER\n");
        if(cap.capabilities & V4L2_CAP_AUDIO)         printf("- AUDIO\n");
        if(cap.capabilities & V4L2_CAP_RADIO)         printf("- RADIO\n");
        if(cap.capabilities & V4L2_CAP_READWRITE)     printf("- READWRITE\n");
        if(cap.capabilities & V4L2_CAP_ASYNCIO)       printf("- ASYNCIO\n");
        if(cap.capabilities & V4L2_CAP_STREAMING)     printf("- STREAMING\n");
        if(cap.capabilities & V4L2_CAP_TIMEPERFRAME)  printf("- TIMEPERFRAME\n");
    }

    if(!cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        printf("Device does not support capturing.\n");
        return -1;
    }

    struct v4l2_audio audio_cap;
    memset(&audio_cap, 0, sizeof(struct v4l2_audio));
    if(ioctl(fd, VIDIOC_G_AUDIO, &audio_cap) < 0) {
        printf("%s: Not a V4L2 audio device?\n", dev);
        return 0;
    } else if ( show_all || show_inputs ) {
        printf("Audio %u (%s) : cap = %X  ,  mode = %X\n", audio_cap.index, audio_cap.name, audio_cap.capability, audio_cap.mode);
    }

    printf("Device capabilities ok.\n");
    return 0;
}

/*
 *
 */
int f640_set_input()
{
    struct v4l2_input inpt;
    int count = 0, i = -1;

    memset(&inpt, 0, sizeof(inpt));

    if ( show_all || show_inputs ) {
        printf("--- Available inputs :\n");
        inpt.index = count;
        while(!ioctl(fd, VIDIOC_ENUMINPUT, &inpt)) {
            printf("%i: \"%s\" : std-0x%X : type %u : status 0x%X\n", count, inpt.name, inpt.std, inpt.type, inpt.status);
            inpt.index = ++count;
        }
    }

    /* If no input was specified, use input 0. */
    if(!input) {
        if ( verbose ) printf("No input was specified, using the first.\n");
        count = 1;
        i = 0;
    }

    /* Set the input. */
    inpt.index = i;
    if(ioctl(fd, VIDIOC_ENUMINPUT, &inpt) == -1) {
        printf("Unable to query input %i.\n", i);
        printf("VIDIOC_ENUMINPUT: %s\n", strerror(errno));
        return -1;
    }

    if ( show_all || show_inputs ) {
        printf("%s: Input %i information :\n", source, i);
        printf("name = \"%s\"\n", inpt.name);
        printf("type = %08X\n", inpt.type);
        if(inpt.type & V4L2_INPUT_TYPE_TUNER) printf("- TUNER\n");
        if(inpt.type & V4L2_INPUT_TYPE_CAMERA) printf("- CAMERA\n");
        printf("audioset = %08X\n", inpt.audioset);
        printf("tuner = %08X\n", inpt.tuner);
        printf("status = %08X\n", inpt.status);
        if(inpt.status & V4L2_IN_ST_NO_POWER) printf("- NO_POWER\n");
        if(inpt.status & V4L2_IN_ST_NO_SIGNAL) printf("- NO_SIGNAL\n");
        if(inpt.status & V4L2_IN_ST_NO_COLOR) printf("- NO_COLOR\n");
        if(inpt.status & V4L2_IN_ST_NO_H_LOCK) printf("- NO_H_LOCK\n");
        if(inpt.status & V4L2_IN_ST_COLOR_KILL) printf("- COLOR_KILL\n");
        if(inpt.status & V4L2_IN_ST_NO_SYNC) printf("- NO_SYNC\n");
        if(inpt.status & V4L2_IN_ST_NO_EQU) printf("- NO_EQU\n");
        if(inpt.status & V4L2_IN_ST_NO_CARRIER) printf("- NO_CARRIER\n");
        if(inpt.status & V4L2_IN_ST_MACROVISION) printf("- MACROVISION\n");
        if(inpt.status & V4L2_IN_ST_NO_ACCESS) printf("- NO_ACCESS\n");
        if(inpt.status & V4L2_IN_ST_VTR) printf("- VTR\n");
    }

    if(ioctl(fd, VIDIOC_S_INPUT, &i) == -1) {
        printf("Error selecting input %i\n", i);
        printf("VIDIOC_S_INPUT: %s\n", strerror(errno));
        return -1;
    } else {
        if ( show_all || show_vidstds ) {
            printf("Video standard :\n");
            struct v4l2_standard std;
            memset(&std, 0, sizeof(std));
            std.index = count = 0;
            while(!ioctl(fd, VIDIOC_ENUMSTD, &std)) {
                printf("\t-standard %d :\n", count);
                memset(&std, 0, sizeof(std));
                std.index = ++count;
            }
        }
    }
    if ( verbose ) printf("Selecting input %i\n", i);

    return 0;
}

/*
 *
 */
static int f640_show_control(struct v4l2_queryctrl *queryctrl)
{
    struct v4l2_querymenu querymenu;
    struct v4l2_control control;
    char *t;
    int m;

    if ( !show_all && !show_controls ) return 0;
    if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) {
        printf("Control \"%s\" disabled.\n", queryctrl->name);
        return 0;
    }

    memset(&querymenu, 0, sizeof(querymenu));
    memset(&control, 0, sizeof(control));

    if(queryctrl->type != V4L2_CTRL_TYPE_BUTTON) {
        control.id = queryctrl->id;
        if(ioctl(fd, VIDIOC_G_CTRL, &control)) {
            printf("Error reading value of control '%s'.\n", queryctrl->name);
            printf("VIDIOC_G_CTRL: %s\n", strerror(errno));
        }
    }

    switch(queryctrl->type)
    {
        case V4L2_CTRL_TYPE_INTEGER:

            t = malloc(64); /* Ick ... TODO: re-write this. */
            if(!t) {printf("Out of memory."); return -1; }

            if(queryctrl->maximum - queryctrl->minimum <= 10) {
                snprintf(t, 63, "%4i", control.value);
            } else {
                snprintf(t, 63, "%4i (%i%%)", control.value, SCALE(0, 100, queryctrl->minimum, queryctrl->maximum, control.value));
            }
            printf(F640_BOLD "%-32s " F640_RESET F640_FG_RED "%-15s" F640_RESET " ( %4i - %4i )\n"
                    , queryctrl->name, t, queryctrl->minimum, queryctrl->maximum);

            free(t);
            break;
        case V4L2_CTRL_TYPE_BOOLEAN:
            printf(F640_BOLD "%-32s" F640_RESET F640_FG_RED " %-15s" F640_RESET " ( True | False )\n"
                    , queryctrl->name, (control.value ? "True" : "False"));
            break;
        case V4L2_CTRL_TYPE_MENU:
            querymenu.id = queryctrl->id;

            t = calloc((queryctrl->maximum - queryctrl->minimum) + 1, 34);
            m = queryctrl->minimum;
            for(m = queryctrl->minimum; m <= queryctrl->maximum; m++) {
                querymenu.index = m;
                if(!ioctl(fd, VIDIOC_QUERYMENU, &querymenu)) {
                    strncat(t, (char *) querymenu.name, 32);
                    if(m < queryctrl->maximum) strncat(t, " | \n", 3);
                }
            }
            querymenu.index = control.value;
            if(ioctl(fd, VIDIOC_QUERYMENU, &querymenu)) {
                free(t);
                printf("Error reading value of menu item %i for control '%s'\n", control.value, queryctrl->name);
                printf("VIDIOC_QUERYMENU: %s\n", strerror(errno));
                return 0;
            }
            printf(F640_BOLD "%-32s" F640_RESET F640_FG_RED " %-15s" F640_RESET " ( %s)\n"
                    , queryctrl->name, querymenu.name, t);
            free(t);
            break;
        case V4L2_CTRL_TYPE_BUTTON:
            printf(F640_BOLD "%-32s" F640_RESET F640_FG_RED " %-15s" F640_RESET " ( %s )\n"
                    , queryctrl->name, "-\n", "[Button]");
            break;
        default:
            printf(F640_BOLD "%-32s" F640_RESET F640_FG_RED " %-15s " F640_RESET " ( %s)\n"
                    , queryctrl->name, "N/A\n", "[Unknown Control Type]");
            break;
    }

    return 0;
}

/*
 *
 */
static int f640_set_control(struct v4l2_queryctrl *queryctrl)
{
    struct v4l2_control control;
    struct v4l2_querymenu querymenu;
    char *sv;
    int iv;

    if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) {
        if (!quiet) printf("Control \"%s\" disabled, don't update it.\n", queryctrl->name);
        return 0;
    }

    memset(&querymenu, 0, sizeof(querymenu));
    memset(&control, 0, sizeof(control));

    control.id = queryctrl->id;

    switch(queryctrl->type)
    {
        case V4L2_CTRL_TYPE_INTEGER:

            /* Convert the value to an integer. */
            iv = atoi(sv);
            /* Is the value a precentage? */
            if(strchr(sv, '%')) {
                /* Adjust the precentage to fit the controls range. */
                iv = SCALE(queryctrl->minimum, queryctrl->maximum, 0, 100, iv);
            }
            if (!quiet) printf("Setting %s to %i (%i%%).\n"
                    , queryctrl->name, iv, SCALE(0, 100, queryctrl->minimum, queryctrl->maximum, iv));

            if(iv < queryctrl->minimum || iv > queryctrl->maximum)
                if (!quiet) printf("Value is out of range. Setting anyway.\n");

            control.value = iv;
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;

        case V4L2_CTRL_TYPE_BOOLEAN:
            iv = -1;
            if(!strcasecmp(sv, "1") || !strcasecmp(sv, "true")) iv = 1;
            if(!strcasecmp(sv, "0") || !strcasecmp(sv, "false")) iv = 0;

            if(iv == -1) {
                if (!quiet) printf("Unknown boolean value '%s' for %s.\n", sv, queryctrl->name);
                return -1;
            }
            if (!quiet) printf("Setting %s to %s (%i).\n", queryctrl->name, sv, iv);
            control.value = iv;
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;
        case V4L2_CTRL_TYPE_MENU:
            /* Scan for a matching value. */
            querymenu.id = queryctrl->id;
            for(iv = queryctrl->minimum; iv <= queryctrl->maximum; iv++)
            {
                querymenu.index = iv;
                if(ioctl(fd, VIDIOC_QUERYMENU, &querymenu)) {
                    printf("Error querying menu.\n");
                    continue;
                }
                if(!strncasecmp((char *) querymenu.name, sv, 32)) break;
            }
            if(iv > queryctrl->maximum) {
                if (!quiet) printf("Unknown value '%s' for %s.\n", sv, queryctrl->name);
                return -1;
            }
            if (!quiet) printf("Setting %s to %s (%i).\n", queryctrl->name, querymenu.name, iv);
            control.value = iv;
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;
        case V4L2_CTRL_TYPE_BUTTON:
            if (!quiet) printf("Triggering %s control.\n", queryctrl->name);
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;
        default:
            if (!quiet) printf("Not setting unknown control type %i (%s).\n", queryctrl->name);
            break;
    }

    return 0;
}


/*
 * uvcvideo: Dropping payload (out of sync).
 */
static int f640_list_controls()
{
    struct v4l2_queryctrl queryctrl;
    int c;

    if (!show_all && !show_controls) return 0;

    // Display normal controls.
    printf(F640_UNDER "User controls :\n" F640_RESET);
    for(c = V4L2_CID_BASE; c < V4L2_CID_LASTP1; c++) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) continue;
        f640_show_control(&queryctrl);
    }
    // Display device-specific controls.
    printf(F640_UNDER "Device-specific controls :\n" F640_RESET);
    for(c = V4L2_CID_PRIVATE_BASE; ; c++) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) break;
        f640_show_control(&queryctrl);
    }

//    c = V4L2_CID_EXPOSURE;
//    memset(&queryctrl, 0, sizeof(queryctrl));
//    queryctrl.id = c;
//    if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
//        printf("No exposure control... : %s\n", queryctrl.name);
//    } else {
//        f640_show_control(&queryctrl);
//    }

    // Camera-class controls
    printf(F640_UNDER "Camera-class controls :\n" F640_RESET);
    for(c = V4L2_CID_EXPOSURE_AUTO; c < V4L2_CID_PRIVACY ; c++ ) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            continue;
        } else {
            f640_show_control(&queryctrl);
        }
    }

    return 0;
}


/*
 *
 */
static int f640_set_pix_format() {
    struct v4l2_fmtdesc fmt;
    int r, v4l2_pal;

    /* Dump a list of formats the device supports. */
    if ( show_all || show_formats )
        printf("Device offers the following V4L2 pixel formats :\n");

    v4l2_pal = 0;
    memset(&fmt, 0, sizeof(fmt));
    fmt.index = v4l2_pal;
    fmt.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while(ioctl(fd, VIDIOC_ENUM_FMT, &fmt) != -1)
    {
        if ( show_all || show_formats )
            printf("%i: [0x%08X] => '%c%c%c%c' : \"%s\" : %d / 0x%X\n"
                , v4l2_pal
                , fmt.pixelformat
                , fmt.pixelformat >> 0
                , fmt.pixelformat >> 8
                , fmt.pixelformat >> 16
                , fmt.pixelformat >> 24
                , fmt.description
                , fmt.type
                , fmt.flags
        );
        if (fmt.pixelformat == palette) memcpy(&format_desc, &fmt, sizeof(fmt));

        memset(&fmt, 0, sizeof(fmt));
        fmt.index = ++v4l2_pal;
        fmt.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }


    /* Try the palette... */
    memset(&format, 0, sizeof(format));
    format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width       = cwidth;
    format.fmt.pix.height      = cheight;
    format.fmt.pix.pixelformat = format_desc.pixelformat;
    format.fmt.pix.field       = V4L2_FIELD_ANY;//V4L2_FIELD_NONE;//V4L2_FIELD_ANY;

    if ( show_all || show_formats ) printf("Try the palette 0x%X (%u, %u):\n", format_desc.pixelformat, format.fmt.pix.width, format.fmt.pix.height);
    if( ( r = ioctl(fd, VIDIOC_TRY_FMT, &format) ) != -1 ) {
        if ( show_all || show_formats )
            printf("Using palette 0x%X : field = %d, colorspace = %d, bytes/line = %u, size=%u\n"
                , format_desc.pixelformat
                , format.fmt.pix.field
                , format.fmt.pix.colorspace
                , format.fmt.pix.bytesperline
                , format.fmt.pix.sizeimage
        );

        if(format.fmt.pix.width != cwidth || format.fmt.pix.height != cheight) {
            if ( show_all || show_formats )
                printf("Adjusting resolution from %ix%i to %ix%i.\n",
                    cwidth, cheight, format.fmt.pix.width, format.fmt.pix.height);
            cwidth = format.fmt.pix.width;
            cheight = format.fmt.pix.height;
        }

        if(ioctl(fd, VIDIOC_S_FMT, &format) == -1) {
            printf("Error setting pixel format.\n");
            printf("VIDIOC_S_FMT: %s\n", strerror(errno));
            return -1;
        }
        if ( verbose ) printf("Setting pixel format ok.\n");

        return 0;
    }
    if ( verbose ) printf("Setting pixel format ko (%d) : %s\n", r, strerror(errno));
    return -1;
}

/*
 *
 */
int f640_set_parm()
{
    struct v4l2_streamparm parm;

    memset (&parm, 0, sizeof (parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd, VIDIOC_G_PARM, &parm) == -1) {
        perror("VIDIOC_G_INPUT");
        return -1;
    }

    if ( show_all || show_framints )
        printf ("GetParm : cap = 0x%X, mode = 0x%X, emode = 0x%X, buf = %u, fps = %u / %u\n"
            , parm.parm.capture.capability
            , parm.parm.capture.capturemode
            , parm.parm.capture.extendedmode
            , parm.parm.capture.readbuffers
            , parm.parm.capture.timeperframe.numerator, parm.parm.capture.timeperframe.denominator
    );

    if ( parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME ) {
        parm.parm.capture.timeperframe.denominator = frames_pers;
        if (ioctl (fd, VIDIOC_S_PARM, &parm) == -1) {
            perror ("VIDIOC_S_INPUT");
            if ( verbose )
                printf("SetStreamParm to %ufrm / %us : ko.\n"
                    , parm.parm.capture.timeperframe.denominator, parm.parm.capture.timeperframe.numerator);
        } else {
            if ( verbose )
                printf("SetStreamParm to %ufrm / %us : ok.\n"
                    , parm.parm.capture.timeperframe.denominator, parm.parm.capture.timeperframe.numerator);
        }
    } else {
        if ( verbose )
            printf("No StreamParm to set.\n");
    }

    /*
     *
     */
    struct v4l2_frmsizeenum sizn;
    int inds = 0, indf = 0;
    struct v4l2_frmivalenum fps;

    memset(&sizn, 0, sizeof(sizn));
    sizn.index = 0;
    sizn.pixel_format = format.fmt.pix.pixelformat;
    while ( ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &sizn) != -1 ) {
        if ( show_all || show_framsize )
            printf("SizeFormat %d : type = %d - %s :\n"
                , sizn.index
                , sizn.type
                , sizn.type == V4L2_FRMSIZE_TYPE_DISCRETE ? "Discrete"
                : sizn.type == V4L2_FRMSIZE_TYPE_STEPWISE ? "StepWise"
                : "Continuous"
        );
        if ( sizn.type == V4L2_FRMSIZE_TYPE_DISCRETE ) {
            if ( show_all || show_framsize )
                printf("\t%ux%u : ", sizn.discrete.width, sizn.discrete.height);
            memset(&fps, 0, sizeof(fps));
            fps.index = indf = 0;
            fps.pixel_format = format.fmt.pix.pixelformat;
            fps.width = sizn.discrete.width;
            fps.height = sizn.discrete.height;
            while ( ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fps) != -1 ) {
                if( fps.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                    if ( show_all || show_framsize )
                        printf("| %u/%u |", fps.discrete.numerator, fps.discrete.denominator);
                } else {
                    if ( show_all || show_framsize )
                        printf("%u < %u < %u (stepwise)", fps.stepwise.min, fps.stepwise.step, fps.stepwise.max);
                }
                memset(&fps, 0, sizeof(fps));
                fps.index = ++indf;
                fps.pixel_format = format.fmt.pix.pixelformat;
                fps.width = sizn.discrete.width;
                fps.height = sizn.discrete.height;
            }
            if ( show_all || show_framsize ) printf("\n");
        } else {
            if ( show_all || show_framsize )
                printf("\t(%u , %u)x(%u , %u) : (%u , %u)\n"
                    , sizn.stepwise.min_width, sizn.stepwise.max_width
                    , sizn.stepwise.min_height, sizn.stepwise.max_height
                    , sizn.stepwise.step_width, sizn.stepwise.step_height
            );
        }
        memset(&sizn, 0, sizeof(sizn));
        sizn.index = ++inds;
        sizn.pixel_format = format.fmt.pix.pixelformat;
    }

    return 0;
}

/*
 *
 */
int f640_set_mmap()
{
    enum v4l2_buf_type type;
    uint32_t i, b;

    /* Does the device support streaming? */
    if(~cap.capabilities & V4L2_CAP_STREAMING) {
        if (!quiet) printf("No streaming cap, returning.\n");
        return -1;
    }

    memset(&req, 0, sizeof(req));

    req.count  = nb_buffers;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        if (!quiet) printf("Error requesting %d buffers for memory map.\n", nb_buffers);
        if (errno == -EINVAL)
            if (!quiet)
                printf ("Video capturing or mmap-streaming is not supported: %s\n", strerror(errno));
        else if (!quiet)
            printf("VIDIOC_REQBUFS: %s\n", strerror(errno));

        return -1;
    }

    if (!quiet) printf("mmap information :\n");
    if (!quiet) printf("frames = %d\n", req.count);

    if(req.count < 2) {
        if (!quiet) printf("Insufficient buffer memory.\n");
        return -1;
    }

    buffer = calloc(req.count, sizeof(v4l2_buffer_t));
    if(!buffer) {
        if (!quiet) printf("Out of memory.\n");
        return -1;
    }

    for(b = 0; b < req.count; b++)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = b;

        if(ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
            printf("Error querying buffer %i\n", b);
            printf("VIDIOC_QUERYBUF: %s\n", strerror(errno));
            free(buffer);
            return(-1);
        }

        buffer[b].length = buf.length;
        buffer[b].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if(buffer[b].start == MAP_FAILED) {
            printf("Error mapping buffer %i\n", b);
            printf("mmap: %s\n", strerror(errno));
            req.count = b;
            for(i = 0; i < req.count; i++) munmap(buffer[i].start, buffer[i].length);
            free(buffer);
            return -1;
        }

        if ( verbose ) printf("%i length=%d\n", b, buf.length);
    }
    map = -1;

    for(b = 0; b < req.count; b++)
    {
        memset(&buf, 0, sizeof(buf));

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = b;

        if(ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            printf("VIDIOC_QBUF: %s\n", strerror(errno));
            for(i = 0; i < req.count; i++) munmap(buffer[i].start, buffer[i].length);
            free(buffer);
            return -1;
        }
    }
//
//    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    if(ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
//        printf("Error starting stream.\n");
//        printf("VIDIOC_STREAMON: %s\n", strerror(errno));
//        for(i = 0; i < req.count; i++) munmap(buffer[i].start, buffer[i].length);
//        free(buffer);
//        return -1;
//    }
//    if ( verbose ) printf("Stream ON with %u buffers.\n", req.count);
    return 0;
}

int f640_stream_on() {
    int i;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        printf("Error starting stream.\n");
        printf("VIDIOC_STREAMON: %s\n", strerror(errno));
        for(i = 0; i < req.count; i++) munmap(buffer[i].start, buffer[i].length);
        free(buffer);
        return -1;
    }
    if ( verbose ) printf("Stream ON with %u buffers.\n", req.count);
    return 0;
}
/*
 *
 */
int f640_free_mmap()
{
    int i;
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) == -1) {
        printf("Error stoping stream.\n");
        printf("VIDIOC_STREAMOFF: %s\n", strerror(errno));
    } else {
        if ( verbose ) printf("STREAMOFF succed.\n");
    }

    for(i = 0; i < req.count; i++)
        munmap(buffer[i].start, buffer[i].length);

    return 0;
}

/*
 *
 */
void f640_signal_term_handler(int signum)
{
    char *signame;

    /* Catches SIGTERM and SIGINT */
    switch(signum)
    {
        case SIGTERM: signame = "SIGTERM"; break;
        case SIGINT:  signame = "SIGINT"; break;
        default:      signame = "Unknown"; break;
    }

    if ( verbose ) printf(F640_FG_RED "Caught signal %s, processing it\n" F640_RESET, signame);
    loop = 0;
    usleep(200*1000);
    //f640_free_mmap();
    //close(fd);
    //f051_end_env(log_env);
    if ( verbose ) printf(F640_BOLD "Exiting TERM handler.\n" F640_RESET);
}


/*
 *
 */
int f640_processing()
{
    uint32_t frame = 0;
    struct timeval tv1, tv2, tv3;
    double d1, d2, d3, rms, r, nth;
    static uint8_t *im0;
    uint32_t i, j, k, size, moy = 0;
    uint8_t *dif = NULL, *im = NULL, *pix = NULL;

    // init
    size = cwidth * cheight;
    im0 = calloc(size, sizeof(uint8_t));
//    dif = calloc(2*size, sizeof(uint8_t));

    // Grid
    struct f640_grid *grid = f640_make_grid(cwidth, cheight, 32);

    // Broadcast

    // Recording
    struct output_stream *stream = f611_init_output("/work/test/loulou", PIX_FMT_YUYV422, cwidth, cheight, frames_pers);

    // LineUp
    struct f640_line *lineup = f640_make_lineup(buffer, req.count, grid, stream, log_env, 150);

    // Lines
    struct f640_video_lines video_lines;
    f640_init_queue(&video_lines.snaped, lineup, req.count);
    for(i = 0 ; i < video_lines.snaped.size ; i++) video_lines.snaped.lines[i] = i;
    f640_init_queue(&video_lines.watched, lineup, req.count);
    f640_init_queue(&video_lines.converted, lineup, req.count);
    f640_init_queue(&video_lines.recorded, lineup, req.count);

    // Threads
    pthread_t thread_watch1, thread_watch2;
    pthread_t thread_convert1, thread_convert2;
    pthread_t thread_record;
    pthread_t thread_release;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&thread_watch1,   &attr, f640_watch,   (void *)(&video_lines));
    pthread_create(&thread_watch2,   &attr, f640_watch,   (void *)(&video_lines));
    pthread_create(&thread_convert1, &attr, f640_convert, (void *)(&video_lines));
    pthread_create(&thread_convert2, &attr, f640_convert, (void *)(&video_lines));
    pthread_create(&thread_record,   &attr, f640_record,  (void *)(&video_lines));
    pthread_create(&thread_release,  &attr, f640_release, (void *)(&video_lines));


    // pnm
    char fname[32];
    //char *header = "P5?greg?240 320?255?";
    //char header[] = {'P','5',0x0A,'g','r','e','g',0x0A,'2','4','0',' ','3','2','0',0x0A,'2','5','5',0x0A};
    char *header = "P5\012# CREATOR: 12345678 Filter Version 1.1\012240 320\012255\012";
    int num_im = 0;

    // Stream ON
    r = f640_stream_on();
    if (r) {
        printf("Stream on failed, returning\n");
        return -1;
    }

    // Loop
    gettimeofday(&tv1, NULL);
    loop = 1;
    while(loop && frame < nb_frames) {
        // DeQueue
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if(ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
            printf("VIDIOC_DQBUF: %s\n", strerror(errno));
            return -1;
        }
        if (debug) printf("DeQueue ok : index = %u, seq = %u, frames = %u\n"
                , buf.index, buf.sequence, buf.timecode.frames);

        // Processing
        int th = 15;
        nth = 0;
        if ( frame ) {
            i = 0; im = im0; pix = buffer[buf.index].start; rms = 0; moy = 0;

            f640_enqueue_buffer(&video_lines.snaped, buf.index);

//            yuv->data = buffer[buf.index].start;
//            memset(tab, 0, nb * sizeof(double));
//            while( i < size ) {
//                r = (*pix - *im) * (*pix - *im);
//                rms += r;
////                moy += abs(*pix - *im);
//                tab[index[i]] += r;
//                if ( dif ) {
//                    //j = ( i % cwidth) * cheight + cheight -1 - ( i / cwidth );
//                    j = i;
//
//                    if (*pix < *im - th) {
//                        dif[2*j] = 0x00;
//                        nth++;
//                    } else if (*pix > *im + th) {
//                        dif[2*j] = 0x00;
//                        nth++;
//                    } else {
//                        dif[2*j] = *pix;
//                    }
//
//                    //dif[2*j + 0]  = *(pix+0);
//                    dif[2*j + 1]  = *(pix+1);
//                }
//                *im = *pix;
//                //f640_yuv_to_rgb(yuv, i, rgb);
//                i++; im++; pix += 2;
//            }
//
//            // Totals
//            rms /= size;
//            tab_min = 0xFFFFFFFFFFL;
//            tab_max = 0;
//            for(k = 0 ; k < nb ; k++) {
//                tab[k] /= carx * cary;
//                if (tab[k] < tab_min) tab_min = tab[k];
//                if (tab[k] > tab_max) tab_max = tab[k];
//
//                if (tab[k] > tab_th) {
//                    //f640_draw_rect(yuv, (k/nbc) * cary * cwidth + (k % nbc) * carx, carx, cary);
////                    f640_draw_rect(rgb, (k/nbc) * cary * cwidth + (k % nbc) * carx, carx, cary);
//                }
//            }
//
//            // HighLight
////            i = 0; pix = buffer[buf.index].start;
////            while( i < size ) {
////                if ( tab[index[i]] <= tab_th ) {
////                    pix[2 * i] = 0xFF;
////                }
////                i++;
////            }

//            // Alert
////            quiet = 1;
//            if (frame > 35 && tab_max > tab_th) {
////                quiet = 0;
//                gettimeofday(&tv3, NULL);
//                k = f640_draw_number(yuv, cwidth - 5, cheight - 5, frame);
//                f640_draw_number(yuv, k - 20, cheight - 5, tab_max);
//
////                k = f640_draw_number(rgb, cwidth - 5, cheight - 5, frame);
////                f640_draw_number(rgb, k - 20, cheight - 5, tab_max);
//
//                f611_add_frame(stream, buffer[buf.index].start);
//
//                //f051_send_data(log_env, buffer[buf.index].start, size << 1);
//                //f051_send_data(log_env, im0, size);
////                f640_full_yuv_to_rgb(yuv, i, rgb);
////                f051_send_data(log_env, rgb->data, rgb->data_size);
//
////                sprintf(fname, "im%07u.pgm", num_im++);
////                FILE *filp = fopen(fname, "wb");
////                fwrite(header, 1, 54, filp);
////                fwrite(dif, 1, size, filp);
////                fclose(filp);
////
//////                sprintf(fname, "im%07u.yuv", num_im++);
//////                FILE *filp = fopen(fname, "wb");
//////                fwrite(buffer[buf.index].start, 2, size, filp);
//////                fclose(filp);
//            }
            if (!quiet)
                printf("Frame " F640_BOLD "%4d" F640_RESET
                    " | " F640_BOLD "RMS" F640_RESET " = " F640_FG_RED "%5.1f" F640_RESET
                    " | ABS = %5.2f | min %4.1f | MAX %5.0f | N %5.0f |"// %u %u %u %u %u %u %u %u %u %u %u\n"
                    , frame, rms, 1.*moy/size, 0, 0, 0
                    //, y[6], y[7], y[8], y[9], y[10], y[11], y[12], y[13], y[14], y[15], y[16]
            );
        } else {
            i = 0; im = im0; pix = buffer[buf.index].start;
            while( i < size ) {
                *im = *pix;
                i++; im++; pix += 2;
            }
        }

        // Data
        gettimeofday(&tv2, NULL);
        d1 = (tv2.tv_sec + tv2.tv_usec / 1000000.0) - (tv1.tv_sec + tv1.tv_usec / 1000000.0);
        if (!quiet)
            printf(" %3.0fms | " F640_FG_RED "%4.1fHz" F640_RESET " | seq %4u | %3u | %2u | %2u | %6u | %6u\n"
                    , frame, 1000 * d1, 1/d1
                    , buf.sequence, buf.timecode.minutes, buf.timecode.seconds, buf.timecode.frames, buf.bytesused, buf.length);
        gettimeofday(&tv1, NULL);

//        // EnQueue
        if(ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            printf("VIDIOC_QBUF: %s\n", strerror(errno));
            return -1;
        }
        //printf("EnQueue ok : index = %u\n", buf.index);

        // Loop
        frame++;
    }
    if ( verbose ) printf("Exited from processing loop.\n");


    return 0;
}

/*
 *
 */
int f640_getopts(int argc, char *argv[])
{
    int c;
    int index = 0;
    static struct option long_opts[] =
    {
        {"help",            no_argument,       NULL, 'h'},
        {"quiet",           no_argument,       NULL, 'q'},
        {"verbose",         no_argument,       NULL, 'v'},
        {"version",         no_argument,       NULL, 'i'},
        {"no-capture",      no_argument,       NULL, '0'},
        {"buffers",         required_argument, NULL, 'b'},
        {"frames",          required_argument, NULL, 'f'},
        {"fps",             required_argument, NULL, 'z'},
        {"device",          required_argument, NULL, 'd'},
        {"palette",         required_argument, NULL, 'y'},
        {"width",           required_argument, NULL, 'W'},
        {"height",          required_argument, NULL, 'H'},
        {"show-all",        no_argument,       NULL, 'A'},
        {"show-caps",       no_argument,       NULL, 'P'},
        {"show-inputs",     no_argument,       NULL, 'I'},
        {"show-controls",   no_argument,       NULL, 'C'},
        {"show-formats",    no_argument,       NULL, 'F'},
        {"show-frame-size", no_argument,       NULL, 'S'},
        {"show-frame-rate", no_argument,       NULL, 'R'},
        {"show-video-std",  no_argument,       NULL, 'V'},
        {NULL, 0, NULL, 0}
    };
    char *opts = "hqvi0:b:f:z:d:y:W:H:APICFSRV";

    while(1)
    {
        if ( ( c = getopt_long(argc, argv, opts, long_opts, NULL) ) < 0 ) break;
        switch(c)
        {
            case 'h':
                printf("Help\n");
                break;
            case 'q':
                quiet = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'i':
                printf("Version 0.1\n");
                break;
            case '0':
                capture = 0;
                break;
            case 'b':
                nb_buffers = atoi(optarg);
                break;
            case 'f':
                nb_frames = atoi(optarg);
                break;
            case 'z':
                frames_pers = atoi(optarg);
                break;
            case 'd':
                snprintf(device, sizeof(device), "/dev/video%s", optarg);
                dev = device;
                source = device;
                break;
            case 'y':
                switch(atoi(optarg)) {
                    case 0 : palette = 0x56595559; break;
                    case 1 : palette = 0x47504A4D; break;
                }
                break;
            case 'W':
                cwidth = atoi(optarg);
                break;
            case 'H':
                cheight = atoi(optarg);
                break;
            case 'A':
                show_all = 1;
                break;
            case 'P':
                show_caps = 1;
                break;
            case 'I':
                show_inputs = 1;
                break;
            case 'C':
                show_controls = 1;
                break;
            case 'F':
                show_formats = 1;
                break;
            case 'S':
                show_framsize = 1;
                break;
            case 'R':
                show_framints = 1;
                break;
            case 'V':
                show_vidstds = 1;
                break;
        }
    }
    //
    if (!nb_frames) capture = 0;
    return 0;
}
/*
 *
 */
int main(int argc, char *argv[])
{
    int err;

    // Options
    f640_getopts(argc, argv);
    printf("Options ok : frames %u  |  freq %u\n", nb_frames, frames_pers);

    // Signals
    signal(SIGTERM, f640_signal_term_handler);
    signal(SIGINT , f640_signal_term_handler);
    printf("Signals ok\n");

    // Open
    fd = open(dev, O_RDWR );//| O_NONBLOCK);
    if(fd < 0) {
        printf("Error opening device: %s\n", dev);
        printf("open: %s\n", strerror(errno));
        return -1;
    }
    printf("Open video ok\n");

    //
    if ( capture ) {
        log_env = f051_init_data_env("video", "loulou", 3200);
        if (!log_env) {
            printf(F640_FG_RED "No proc environment, exiting.\n" F640_RESET);
            return -1;
        }
    }
    printf("Init env ok\n");

    // Debug
    if ( ioctl(fd, VIDIOC_LOG_STATUS) == -1) {
        printf("No debugging into kern.log : %s\n", strerror(errno));
    } else {
        printf("Debugging into kern.log.\n");
    }

    // Capabilities
    if ( f640_get_capabilities() < 0 ) goto end;
    printf("Capabilities ok\n");

    // Input
    err = f640_set_input();

    // Controls
    err = f640_list_controls();

    // Format
    err = f640_set_pix_format();

    // Parm
    err = f640_set_parm();

    // Map
    if ( capture ) {
        err = f640_set_mmap();
    }

    // Capture
    if ( capture ) {
        err = f640_processing();
    }

    // UnMap
    if ( capture ) {
        err = f640_free_mmap();
    }

    //
end:
    if( fd >= 0 ) close(fd);
    f051_end_env(log_env);
    printf(F640_BOLD "Exiting app.\n" F640_RESET);
    return 0;
}
