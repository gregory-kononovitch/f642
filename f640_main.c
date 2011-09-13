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
static char *dev = "/dev/video0";
static int fd;
static struct v4l2_capability cap;
static char *source = "/dev/video0";
static char *input = NULL;
static int cwidth = 320;
static int cheight = 240;
static struct v4l2_format format;
static struct v4l2_fmtdesc format_desc;
static char map;
static struct v4l2_requestbuffers req;
static struct v4l2_buffer buf;
static v4l2_buffer_t *buffer;
static int loop;

static uint32_t nb_frames = 10;
static int show_config = 0;
/*
 *
 */
static int f640_get_capabilities()
{
    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        printf("%s: Not a V4L2 device?\n", dev);
        return -1;
    }

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

    if(!cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        printf("Device does not support capturing.\n");
        return -1;
    }
}

/*
 *
 */
int f640_set_input()
{
    struct v4l2_input inpt;
    int count = 0, i = -1;

    memset(&inpt, 0, sizeof(inpt));

    printf("--- Available inputs :\n");

    inpt.index = count;
    while(!ioctl(fd, VIDIOC_ENUMINPUT, &inpt)) {
        printf("%i: \"%s\" : std-0x%X : type %u : status 0x%X\n", count, inpt.name, inpt.std, inpt.type, inpt.status);
        inpt.index = ++count;
    }


    /* If no input was specified, use input 0. */
    if(!input) {
        printf("No input was specified, using the first.\n");
        count = 1;
        i = 0;
    }

    /* Check if the input is specified by name. */
    if(i == -1) {
        inpt.index = count;
        while(!ioctl(fd, VIDIOC_ENUMINPUT, &inpt))
        {
            if(!strncasecmp((char *) inpt.name, input, 32)) i = count;
            inpt.index = ++count;
        }
    }

    if(i == -1) {
        char *endptr;
        /* Is the input specified by number? */
        i = strtol(input, &endptr, 10);
        if(endptr == input) i = -1;
    }

    if(i == -1 || i >= count) {
        /* The specified input wasn't found! */
        printf("Unrecognised input \"%s\"\n", input);
        return -1;
    }

    /* Set the input. */
    inpt.index = i;
    if(ioctl(fd, VIDIOC_ENUMINPUT, &inpt) == -1) {
        printf("Unable to query input %i.\n", i);
        printf("VIDIOC_ENUMINPUT: %s\n", strerror(errno));
        return -1;
    }

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

    if(ioctl(fd, VIDIOC_S_INPUT, &i) == -1) {
        printf("Error selecting input %i\n", i);
        printf("VIDIOC_S_INPUT: %s\n", strerror(errno));
        return -1;
    } else {
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
    printf("Selecting input %i\n", i);

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

    if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) return 0;

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
                snprintf(t, 63, "%i : ", control.value);
            } else {
                snprintf(t, 63, "%i (%i%%) : ", control.value, SCALE(0, 100, queryctrl->minimum, queryctrl->maximum, control.value));
            }
            printf("%-25s %-15s %i - %i\n", queryctrl->name, t, queryctrl->minimum, queryctrl->maximum);

            free(t);
            break;
        case V4L2_CTRL_TYPE_BOOLEAN:
            printf("%-25s %-15s True | False\n", queryctrl->name, (control.value ? "True" : "False"));
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
            printf("%-25s %-15s %s\n", queryctrl->name, querymenu.name, t);
            free(t);
            break;
        case V4L2_CTRL_TYPE_BUTTON:
            printf("%-25s %-15s %s\n", queryctrl->name, "-\n", "[Button]");
            break;
        default:
            printf("%-25s %-15s %s\n", queryctrl->name, "N/A\n", "[Unknown Control Type]");
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

    if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) {printf("Ctrl disabled;\n"); return 0; }

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
            printf("Setting %s to %i (%i%%).\n", queryctrl->name, iv, SCALE(0, 100, queryctrl->minimum, queryctrl->maximum, iv));

            if(iv < queryctrl->minimum || iv > queryctrl->maximum) printf("Value is out of range. Setting anyway.\n");

            control.value = iv;
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;

        case V4L2_CTRL_TYPE_BOOLEAN:
            iv = -1;
            if(!strcasecmp(sv, "1") || !strcasecmp(sv, "true")) iv = 1;
            if(!strcasecmp(sv, "0") || !strcasecmp(sv, "false")) iv = 0;

            if(iv == -1) {
                printf("Unknown boolean value '%s' for %s.\n", sv, queryctrl->name);
                return -1;
            }
            printf("Setting %s to %s (%i).\n", queryctrl->name, sv, iv);
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
                printf("Unknown value '%s' for %s.\n", sv, queryctrl->name);
                return -1;
            }
            printf("Setting %s to %s (%i).\n", queryctrl->name, querymenu.name, iv);
            control.value = iv;
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;
        case V4L2_CTRL_TYPE_BUTTON:
            printf("Triggering %s control.\n", queryctrl->name);
            ioctl(fd, VIDIOC_S_CTRL, &control);
            break;
        default:
            printf("Not setting unknown control type %i (%s).\n", queryctrl->name);
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
    // Display normal controls.
    printf("User controls :\n");
    for(c = V4L2_CID_BASE; c < V4L2_CID_LASTP1; c++) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(fd, VIDIOC_QUERYCTRL, &queryctrl)) continue;
        f640_show_control(&queryctrl);
    }
    // Display device-specific controls.
    printf("Device-specific controls :\n");
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
    printf("Camera-class controls :\n");
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
static int f640_set_pix_format()
{
    struct v4l2_fmtdesc fmt;
    int v4l2_pal;

    /* Dump a list of formats the device supports. */
    printf("Device offers the following V4L2 pixel formats :\n");

    v4l2_pal = 0;
    memset(&fmt, 0, sizeof(fmt));
    fmt.index = v4l2_pal;
    fmt.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while(ioctl(fd, VIDIOC_ENUM_FMT, &fmt) != -1)
    {
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
        memcpy(&format_desc, &fmt, sizeof(fmt));

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

    printf("Try the palette :\n");
    if(ioctl(fd, VIDIOC_TRY_FMT, &format) != -1 ) {
        printf("Using palette 0x%X : field = %d, colorspace = %d, bytes/line = %u, size=%u\n"
                , format_desc.pixelformat
                , format.fmt.pix.field
                , format.fmt.pix.colorspace
                , format.fmt.pix.bytesperline
                , format.fmt.pix.sizeimage
        );

        if(format.fmt.pix.width != cwidth || format.fmt.pix.height != cheight) {
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
        printf("Setting pixel format ok.\n");

        return 0;
    }
    printf("Setting pixel format ko : %s\n", strerror(errno));
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
        perror ("VIDIOC_G_INPUT");
        return -1;
    }
    printf ("GetParm : cap = 0x%X, mode = 0x%X, emode = 0x%X, buf = %u, fps = %u / %u\n"
            , parm.parm.capture.capability
            , parm.parm.capture.capturemode
            , parm.parm.capture.extendedmode
            , parm.parm.capture.readbuffers
            , parm.parm.capture.timeperframe.numerator, parm.parm.capture.timeperframe.denominator
    );

    if ( parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME ) {
        parm.parm.capture.timeperframe.denominator = 15;
        if (ioctl (fd, VIDIOC_S_PARM, &parm) == -1) {
            perror ("VIDIOC_S_INPUT");
            printf("SetStreamParm ko.\n");
        } else {
            printf("SetStreamParm ok.\n");
        }
    } else {
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
        printf("SizeFormat %d : type = %d - %s :\n"
                , sizn.index
                , sizn.type
                , sizn.type == V4L2_FRMSIZE_TYPE_DISCRETE ? "Discrete"
                : sizn.type == V4L2_FRMSIZE_TYPE_STEPWISE ? "StepWise"
                : "Continuous"
        );
        if ( sizn.type == V4L2_FRMSIZE_TYPE_DISCRETE ) {
            printf("\t%ux%u : ", sizn.discrete.width, sizn.discrete.height);
            memset(&fps, 0, sizeof(fps));
            fps.index = indf = 0;
            fps.pixel_format = format.fmt.pix.pixelformat;
            fps.width = sizn.discrete.width;
            fps.height = sizn.discrete.height;
            while ( ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fps) != -1 ) {
                if( fps.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                    printf("| %u/%u |", fps.discrete.numerator, fps.discrete.denominator);
                } else {
                    printf("%u < %u < %u (stepwise)", fps.stepwise.min, fps.stepwise.step, fps.stepwise.max);
                }
                memset(&fps, 0, sizeof(fps));
                fps.index = ++indf;
                fps.pixel_format = format.fmt.pix.pixelformat;
                fps.width = sizn.discrete.width;
                fps.height = sizn.discrete.height;
            }
            printf("\n");
        } else {
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
    if(~cap.capabilities & V4L2_CAP_STREAMING) {printf("No streaming cap, returning.\n"); return -1;}

    memset(&req, 0, sizeof(req));

    req.count  = 5;
    req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        printf("Error requesting buffers for memory map.\n");
        if (errno == EINVAL) printf ("Video capturing or mmap-streaming is not supported: %s\n", strerror(errno));
        else printf("VIDIOC_REQBUFS: %s\n", strerror(errno));

        return -1;
    }

    printf("mmap information :\n");
    printf("frames = %d\n", req.count);

    if(req.count < 2) {
        printf("Insufficient buffer memory.\n");
        return -1;
    }

    buffer = calloc(req.count, sizeof(v4l2_buffer_t));
    if(!buffer) {
        printf("Out of memory.\n");
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

        printf("%i length=%d\n", b, buf.length);
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

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        printf("Error starting stream.\n");
        printf("VIDIOC_STREAMON: %s\n", strerror(errno));
        for(i = 0; i < req.count; i++) munmap(buffer[i].start, buffer[i].length);
        free(buffer);
        return -1;
    }
    printf("Stream ON with %u buffers.\n", req.count);
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
        printf("STREAMOFF succed.\n");
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

    printf(F640_FG_RED "Caught signal %s, processing it\n" F640_RESET, signame);
    loop = 0;
    f640_free_mmap();
    close(fd);
    f051_end_env(log_env);
    printf(F640_BOLD "Exiting TERM handler.\n" F640_RESET);
}

/*
 *
 */
int f640_processing()
{
    uint32_t frame = 0;
    struct timeval tv1, tv2, tv3;
    double d1, d2, d3;
    static uint8_t *im0;
    uint32_t i, size, rms = 0, moy = 0, y[256];
    uint8_t *dif, *im, *pix;

    // init
    size = cwidth * cheight;
    im0 = calloc(size, sizeof(uint8_t));
    dif = NULL;//calloc(size, sizeof(uint8_t));

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
        //printf("DeQueue ok : index = %u\n", buf.index);

        // Processing
        if ( frame ) {
            i = 0; im = im0; pix = buffer[buf.index].start; rms = 0; moy = 0;
            memset(y, 0, sizeof(y));
            while( i < size ) {
                rms += (*pix - *im) * (*pix - *im);
                moy += abs(*pix - *im);
                y[*pix / 10]++;
                if ( dif ) dif[i] = 127 + (*pix -*im) / 2;
                *im = *pix;
                i++; im++; pix += 2;
            }
            rms = sqrt(rms);
            printf("Frame " F640_BOLD "%4d" F640_RESET
                    "  |  " F640_BOLD "RMS" F640_RESET " = " F640_FG_RED "%5u" F640_RESET
                    "  |  ABS = %5.2f  | "// %u %u %u %u %u %u %u %u %u %u %u\n"
                    , frame, rms, 1.*moy/size
                    //, y[6], y[7], y[8], y[9], y[10], y[11], y[12], y[13], y[14], y[15], y[16]
            );
        } else {
            i = 0; im = im0; pix = buffer[buf.index].start;;
            while( i < size ) {
                *im = *pix;
                i++; im++; pix += 2;
            }
        }
        f051_send_data(log_env, dif ? dif : im0, size);

        // EnQueue
        if(ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
            printf("VIDIOC_QBUF: %s\n", strerror(errno));
            return -1;
        }
        //printf("EnQueue ok : index = %u\n", buf.index);

        // Loop
        gettimeofday(&tv2, NULL);
        d1 = (tv2.tv_sec + tv2.tv_usec / 1000000.0) - (tv1.tv_sec + tv1.tv_usec / 1000000.0);
        printf(" d = %5.1fms  | freq = " F640_FG_RED "%5.2fHz" F640_RESET "  |\n", frame, 1000 * d1, 1/d1);
        gettimeofday(&tv1, NULL);
        frame++;
    }
    printf("Exited from processing loop.\n");

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
        {"help",            no_argument,       0, 'h'},
        {"config",          required_argument, 0, 'c'},
        {"quiet",           no_argument,       0, 'q'},
        {"verbose",         no_argument,       0, 'v'},
        {"version",         no_argument,       0, 'V'},
        {"frames",          required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };
    char *opts = "-h:c:q:v:V:f";

    while( (c = getopt_long(argc, argv, opts, long_opts, &index) ) != -1)
    {
        switch(c)
        {
            case 'h':
                printf("Help\n");
                break;
            case 'c':
                show_config = atoi(optarg);
                printf("Show config selected.\n");
                break;
            case 'q':

                break;
            case 'v':

                break;
            case 'V':
                printf("Version 0.1\n");
                break;
            case 'l':

                break;
            case 'f':
                nb_frames = atoi(optarg);
                printf("Will get %u frames\n", nb_frames);
                break;
        }
    }
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

    // Signals
    signal(SIGTERM, f640_signal_term_handler);
    signal(SIGINT , f640_signal_term_handler);

    // Open
    fd = open(dev, O_RDWR );//| O_NONBLOCK);
    if(fd < 0) {
        printf("Error opening device: %s\n", dev);
        printf("open: %s\n", strerror(errno));
        return -1;
    }

    //
    if ( !show_config ) {
        log_env = f051_init_data_env("bird", "stream", 900);
        if (!log_env) {
            printf(F640_FG_RED "No proc environment, exiting.\n" F640_RESET);
            return -1;
        }
    }

    // Debug
    if ( ioctl(fd, VIDIOC_LOG_STATUS) == -1) {
        printf("No debugging into kern.log : %s\n", strerror(errno));
    } else {
        printf("Debugging into kern.log.\n");
    }

    // Capabilities
    if ( f640_get_capabilities() < 0 ) goto end;

    // Input
    if ( !show_config ) {
        err = f640_set_input();
    }

    // Controls
    err = f640_list_controls();

    // Format
    if ( !show_config ) {
        err = f640_set_pix_format();
    }

    // Parm
    if ( !show_config ) {
        err = f640_set_parm();
    }

    // Map
    if ( !show_config ) {
        err = f640_set_mmap();
    }

    // Grab
    if ( !show_config ) {
        err = f640_processing();
    }

    // UnMap
    if ( !show_config ) {
        err = f640_free_mmap();
    }

    //
end:
    if( fd >= 0 ) close(fd);
    return 0;
}
