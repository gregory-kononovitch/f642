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
int DEBUG = 0;

static struct f051_log_env *log_env;

static int v4l2 = 1;
static struct input_stream *file_stream;;

static char device[32];
static char *dev = "/dev/video0";
static int fd;
static struct v4l2_capability cap;
static char *source = "/dev/video0";
static char *input = NULL;
static int cwidth = 1024;
static int cheight = 576;
static uint32_t palette = 0x47504A4D;   // 0x56595559
static struct v4l2_format format;
static struct v4l2_fmtdesc format_desc;
static int nb_buffers = 10;
static char map;
static struct v4l2_requestbuffers req;
static struct v4l2_buffer buf;
static v4l2_buffer_t *buffer;
static int loop;

static int stream_no = 1;
static int grid_no = 2;
static int edge_no = -1;
static int get_no  = -1;
static int grid10_no  = -1;
static int grid20_no  = -1;
static int grid30_no  = -1;

static int trigger = 1000;
static int recording = 1;

static int capture          = 1;
static uint32_t nb_frames   = 0;
static uint32_t frames_pers = 1;
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

    printf("SET MMAP : %d\n", nb_buffers);

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
        printf("Error starting stream on \"%s\"\n", dev);
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
int f640_get_v4l2_frame(int fd, struct v4l2_buffer *frame) {
    // DeQueue
    memset(frame, 0, sizeof(struct v4l2_buffer));
    frame->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frame->memory = V4L2_MEMORY_MMAP;
    if(ioctl(fd, VIDIOC_DQBUF, frame) == -1) {
        printf("VIDIOC_DQBUF: %s\n", strerror(errno));
        return -1;
    }
    return frame->bytesused;
}

/*
 *
 */
int f640_rel_v4l2_frame(int fd, struct v4l2_buffer *frame) {
    // EnQueue
    if(ioctl(fd, VIDIOC_QBUF, frame) == -1) {
        printf("VIDIOC_QBUF: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/*
 *
 */
int f640_get_file_frame(struct v4l2_buffer *frame) {
    int r = 1;
    long delta = 1000 * 1000 / frames_pers;
    static long index = 0;
    static struct timeval tv1, tv2, tv3;

    while (1) {
        av_free_packet(&file_stream->pkt);
        r = av_read_frame(file_stream->inputFile, &file_stream->pkt);
        if (r < 0) {
            printf("End of file : %d\n", r);
            return -1;
        }
        if (file_stream->pkt.stream_index != file_stream->video_stream) continue;
        break;
    }
    frame->index = index % nb_buffers;
    frame->length = file_stream->pkt.size;
    frame->bytesused = file_stream->pkt.size;
    frame->sequence++;

    memcpy(buffer[frame->index].start, file_stream->pkt.data, file_stream->pkt.size);

    //
    gettimeofday(&tv2, NULL);
    if (index) {
        timersub(&tv2, &tv1, &tv3);
        if (1000000 * tv3.tv_sec + tv3.tv_usec < delta) {
            usleep(delta - (1000000 * tv3.tv_sec + tv3.tv_usec));
        }
    }
    index++;
    gettimeofday(&tv1, NULL);
    return 0;
}

/*
 *
 */
int f640_rel_file_frame(struct v4l2_buffer *frame) {
    return 0;
}

/*
 *
 */
int f640_processing()
{
    uint32_t frame = 0;
    struct timeval tv1, tv2, tv3;
    double d;
    int r, i, io, bs = 0;
    long size_in = 0;

    // Grid
    struct f640_grid  *grid  = f640_make_grid(cwidth, cheight, 32);
    struct f640_grid2 *grid2 = f640_make_grid2(grid);

    // Broadcast

    // Recording
    struct output_stream *stream = NULL;
    //struct output_stream *stream = f611_init_output("/work/test/loulou", PIX_FMT_YUYV422, cwidth, cheight, frames_pers);
    //struct output_stream *stream = f611_init_output("/work/test/loulouPPP", PIX_FMT_BGR24, cwidth, cheight, frames_pers);
    if (recording) stream = f611_init_output("/work/test/loulouPPP", PIX_FMT_BGR24, cwidth, cheight, frames_pers);


    // LineUp
    struct f640_line *lineup = f640_make_lineup(buffer, req.count, grid, PIX_FMT_BGR24, stream, log_env, trigger);  //220

    // Lines
    struct f640_video_lines video_lines;
    memset(&video_lines, 0, sizeof(struct f640_video_lines));
    video_lines.grid    = grid;
    video_lines.grid2   = grid2;
    video_lines.fps     = frames_pers;
    video_lines.recording = recording;

    char fname[64];
    sprintf(video_lines.stream_path, "/dev/t030/t030-%d", stream_no);
    video_lines.fd_stream = open(video_lines.stream_path, O_WRONLY);
    sprintf(video_lines.grid_path, "/dev/t030/t030-%d", grid_no);
    video_lines.fd_grid = open(video_lines.grid_path, O_WRONLY);
    if (edge_no > 0) {
        sprintf(video_lines.edge_path, "/dev/t030/t030-%d", edge_no);
        video_lines.fd_edge = open(video_lines.edge_path, O_WRONLY);
    } else {
        video_lines.fd_edge = -1;
    }
    if (get_no > 0) {
        sprintf(video_lines.get_path, "/dev/t030/t030-%d", get_no);
        video_lines.fd_get = open(video_lines.get_path, O_RDONLY);
    } else {
        video_lines.fd_get = -1;
    }
    if (grid10_no > 0) {
        sprintf(video_lines.grid10_path, "/dev/t030/t030-%d", grid10_no);
        video_lines.fd_grid10 = open(video_lines.grid10_path, O_WRONLY);
    } else {
        video_lines.fd_grid10 = -1;
    }
    if (grid20_no > 0) {
        sprintf(video_lines.grid20_path, "/dev/t030/t030-%d", grid20_no);
        video_lines.fd_grid20 = open(video_lines.grid20_path, O_WRONLY);
    } else {
        video_lines.fd_grid20 = -1;
    }
    if (grid30_no > 0) {
        sprintf(video_lines.grid30_path, "/dev/t030/t030-%d", grid30_no);
        video_lines.fd_grid30 = open(video_lines.grid30_path, O_WRONLY);
    } else {
        video_lines.fd_grid30 = -1;
    }
    printf("Streams : stream %d, edge %d, grid %d, write %d, grid10 %d, grid20 %d, grid30%d\n"
            , video_lines.fd_stream, video_lines.fd_edge, video_lines.fd_grid, video_lines.fd_get, video_lines.fd_grid10, video_lines.fd_grid20, video_lines.fd_grid30
    );

    f640_init_queue(&video_lines.snaped,    lineup, req.count);
    for(i = 0 ; i < video_lines.snaped.size ; i++) video_lines.snaped.lines[i] = i;
    f640_init_queue(&video_lines.buffered,  lineup, req.count);
    f640_init_queue(&video_lines.decoded,   lineup, req.count);
    f640_init_queue(&video_lines.watched,   lineup, req.count);
    f640_init_queue(&video_lines.edged,     lineup, req.count);
    f640_init_queue(&video_lines.converted, lineup, req.count);
    f640_init_queue(&video_lines.recorded,  lineup, req.count);
    f640_init_queue(&video_lines.released,  lineup, req.count);
    video_lines.fd = fd;
    pthread_mutex_init(&video_lines.ioc, NULL);

    if (DEBUG) printf("Queues initialized\n");

    // Threads
    pthread_t thread_decode1, thread_decode2, thread_decode3, thread_decode4;
    pthread_t thread_watch1, thread_watch2, thread_watch3, thread_watch4;
    pthread_t thread_gray1, thread_gray2, thread_gray3, thread_gray4;
    pthread_t thread_convert1, thread_convert2, thread_convert3, thread_convert4;
    pthread_t thread_record;
    pthread_t thread_release;
    pthread_t thread_read;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Decode
    pthread_create(&thread_decode1,   &attr, f640_decode,   (void *)(&video_lines));
//    usleep(150*1000);
//    pthread_create(&thread_decode2,   &attr, f640_decode,   (void *)(&video_lines));
//    usleep(150*1000);
//    pthread_create(&thread_decode3,   &attr, f640_decode,   (void *)(&video_lines));

    if (DEBUG) {
        printf("Thread 1 launched\n");
        usleep(100*1000);
    }

    // Watch
    pthread_create(&thread_watch1,   &attr, f640_watch_mj,   (void *)(&video_lines));
//    pthread_create(&thread_watch2,   &attr, f640_watch_mj,   (void *)(&video_lines));
//    pthread_create(&thread_watch3,   &attr, f640_watch,   (void *)(&video_lines));
//    pthread_create(&thread_watch4,   &attr, f640_watch,   (void *)(&video_lines));

    if (DEBUG) {
        printf("Thread 2 launched\n");
        usleep(100*1000);
    }

    // Grayed
    pthread_create(&thread_gray1,   &attr, f640_gray_mj,   (void *)(&video_lines));
//    pthread_create(&thread_gray2,   &attr, f640_gray_mj,   (void *)(&video_lines));

    if (DEBUG) {
        printf("Thread 3 launched\n");
        usleep(100*1000);
    }

    // Convert
    pthread_create(&thread_convert1, &attr, f640_convert, (void *)(&video_lines));
//    pthread_create(&thread_convert2, &attr, f640_convert, (void *)(&video_lines));
//    pthread_create(&thread_convert3, &attr, f640_convert, (void *)(&video_lines));
//    pthread_create(&thread_convert4, &attr, f640_convert, (void *)(&video_lines));

    if (DEBUG) {
        printf("Thread 4 launched\n");
        usleep(100*1000);
    }

    // Record
    pthread_create(&thread_record,   &attr, f640_record_mj,  (void *)(&video_lines));

    if (DEBUG) {
        printf("Thread 5 launched\n");
        usleep(100*1000);
    }

    // Release
    pthread_create(&thread_release,  &attr, f640_release, (void *)(&video_lines));

    if (DEBUG) {
        printf("Thread 6 launched\n");
        usleep(100*1000);
    }

    // Read
    if (video_lines.fd_get > 0) {
        pthread_create(&thread_read,  &attr, f640_read, (void *)(&video_lines));
    }

    if (DEBUG) {
        printf("Thread 7 launched\n");
        usleep(100*1000);
    }

    if (DEBUG) printf("Thread launched\n");

    // pnm
    //char *header = "P5?greg?240 320?255?";
    //char header[] = {'P','5',0x0A,'g','r','e','g',0x0A,'2','4','0',' ','3','2','0',0x0A,'2','5','5',0x0A};
    char *header = "P5\012# CREATOR: 12345678 Filter Version 1.1\012240 320\012255\012";
    int num_im = 0;

    // Stream ON
    if (v4l2) {
        r = f640_stream_on();
        if (r) {
            printf("Stream on failed, returning\n");
            return -1;
        }
    }

    // Loop
    int show = 0, show_freq = 2 * frames_pers;
    struct v4l2_buffer v4l_buf;
    uint64_t HZ[100];
    memset(&HZ, 0, sizeof(HZ));

    gettimeofday(&tv1, NULL);
    loop = 1;
    while(loop && (frame < nb_frames || nb_frames == 1)) {
        // DeQueue
        if (v4l2) size_in += f640_get_v4l2_frame(fd, &buf);
        else f640_get_file_frame(&buf);


        bs++;
        if (0) {
            if (frame % show_freq == 0) {
                printf("GET: %.1f\n", 1. * size_in / (1024 * show_freq));
                size_in = 0;
                gettimeofday(&tv2, NULL);
                d = ( (tv2.tv_sec + tv2.tv_usec / 1000000.0) - (tv1.tv_sec + tv1.tv_usec / 1000000.0) ) / show_freq;
                if ( 1/d < 100) HZ[(int)(1/d)]++;
                if (show) printf("DeQueue ok : index = %u, seq = %u, frames = %u, Hz = %3.1f\n", buf.index, buf.sequence, buf.timecode.frames, 1/d);
                if (frame % show_freq == 0) printf("HZ %3.1f (bs = %d) | %lu | %lu - %lu - %lu - %lu | %lu | %lu - %lu - %lu - %lu | "
                        "%lu | %lu - %lu - %lu | %lu | %lu - %lu - %lu - %lu - %lu | "
                        "%lu | %lu\n", 1/d, bs
                        , HZ[10], HZ[11], HZ[12], HZ[13], HZ[14], HZ[15], HZ[16], HZ[17], HZ[18], HZ[19]
                        , HZ[20], HZ[21], HZ[22], HZ[23], HZ[24], HZ[25], HZ[26], HZ[27], HZ[28], HZ[29]
                        , HZ[30], HZ[31]
                );
                gettimeofday(&tv1, NULL);
            }
        }

        // Processing
        f640_enqueue_buffer(&video_lines.snaped, &buf);

        // Loop
        frame++;

        // EnQueue
        io = 0;
        while( io != -1) {
            io = f640_dequeue_unblock(&video_lines.released);
            if (show) printf("MAIN    : dequeue %d / %d : (sn %d - wa %d - cv %d - rc %d - rl %d)\n", io, bs
                    , f640_uns_queue_size(&video_lines.snaped)
                    , f640_uns_queue_size(&video_lines.watched)
                    , f640_uns_queue_size(&video_lines.converted)
                    , f640_uns_queue_size(&video_lines.recorded)
                    , f640_uns_queue_size(&video_lines.released)
            );
            if (io < 0 && bs < req.count - 2) break;
            if (io < 0 && bs > req.count - 3) {
                io = f640_dequeue_line(&video_lines.released);
            }
            if (v4l2) f640_rel_v4l2_frame(fd, &lineup[io].buf);
            else f640_rel_file_frame(&lineup[io].buf);
            bs--;
            f640_free_line(&video_lines.snaped, io);
            //printf("MAIN    : released %d (%u) / %d\n", io, lineup[io].buf.index, bs);
        }
    }
    if ( verbose ) printf("Exited from processing loop.\n");


    return 0;
}

/*
 *
 */
static int f640_getopts(int argc, char *argv[])
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
        {"debug",           no_argument,       NULL, 'D'},
        {"file",            no_argument,       NULL, 'a'},
        {"norecord",        no_argument,       NULL, 'r'},
        {"stream",          required_argument, NULL, 's'},
        {"grid",            required_argument, NULL, 'g'},
        {"edge",            required_argument, NULL, 'e'},
        {"grid10",          required_argument, NULL, '1'},
        {"grid20",          required_argument, NULL, '2'},
        {"grid30",          required_argument, NULL, '3'},
        {"get",             required_argument, NULL, 'w'},
        {"buffers",         required_argument, NULL, 'b'},
        {"frames",          required_argument, NULL, 'f'},
        {"fps",             required_argument, NULL, 'z'},
        {"device",          required_argument, NULL, 'd'},
        {"trigger",         required_argument, NULL, 't'},
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
    char *opts = "hqvi0Dar:s:g:e:1:2:3:b:f:z:d:t:y:W:H:APICFSRV";

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
            case 'D':
                DEBUG = 1;
                break;
            case 'a':
                v4l2 = 0;
                recording = 0;
                break;
            case 'r':
                recording = 0;
                break;
            case 's':
                stream_no = atoi(optarg);
                printf("Stream : %d\n", stream_no);
                break;
            case 'g':
                grid_no = atoi(optarg);
                printf("Grid : %d\n", grid_no);
                break;
            case 'e':
                edge_no = atoi(optarg);
                printf("Edge : %d\n", edge_no);
                break;
            case 'w':
                get_no = atoi(optarg);
                printf("Write : %d\n", get_no);
                break;
            case '1':
                grid10_no = atoi(optarg);
                printf("Grid 10 : %d\n", grid10_no);
                break;
            case '2':
                grid20_no = atoi(optarg);
                printf("Grid 20 : %d\n", grid20_no);
                break;
            case '3':
                grid30_no = atoi(optarg);
                printf("Grid 30 : %d\n", grid30_no);
                break;
            case 'b':
                nb_buffers = atoi(optarg);
                break;
            case 'f':
                nb_frames = atoi(optarg);
                break;
            case 'z':
                frames_pers = atoi(optarg);
                printf("Frame per second : \n", frames_pers);
                break;
            case 'd':
                snprintf(device, sizeof(device), "/dev/video%s", optarg);
                dev = device;
                source = device;
                break;
            case 't':
                trigger = atoi(optarg);
                printf("Threshold : %d\n", trigger);
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

    avcodec_register_all();
    av_register_all();

    // Options
    f640_getopts(argc, argv);
    printf("Options ok : frames %u  |  freq %u\n", nb_frames, frames_pers);

    // Signals
    signal(SIGTERM, f640_signal_term_handler);
    signal(SIGINT , f640_signal_term_handler);
    printf("Signals ok\n");

    if ( !v4l2 ) {
        int i;

        v4l2 = 0;

        if (argc > 0) {
            printf("Opening file %s\n", argv[argc-1]);
            file_stream = f611_init_input(argv[argc-1]);
        } else {
            file_stream = f611_init_input("/music/tst.avi");
        }
        if (!file_stream || file_stream->video_stream < 0) return -1;

        cwidth  = file_stream->decoderCtxt[file_stream->video_stream]->width;
        cheight = file_stream->decoderCtxt[file_stream->video_stream]->height;
        if (frames_pers == 1) frames_pers = file_stream->inputFile->streams[file_stream->video_stream]->r_frame_rate.num;

        // Buffers
        nb_buffers = 10;
        req.count  = nb_buffers;
        req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_USERPTR;

        buffer = calloc(nb_buffers, sizeof(v4l2_buffer_t));
        for(i = 0 ; i < nb_buffers ; i++) {
            buffer[i].start  = calloc(1, 2 * cwidth * cheight);
            buffer[i].length = 2 * cwidth * cheight;
        }

        err = f640_processing();

        return 0;
    } else {
        v4l2 = 1;
        if (frames_pers == 1) frames_pers = 10;

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
            //log_env = f051_init_data_env("bird", "brodge", 3200);
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
}
