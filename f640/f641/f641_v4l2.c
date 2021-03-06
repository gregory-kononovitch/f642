/*
 * file    : f641_v4l2.c
 * project : f640
 *
 * Created on: Dec 20, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "f641_v4l2.h"



/*
 *
 */
int f641_get_capabilities(struct f641_v4l2_parameters *prm)
{
    if(ioctl(prm->fd, VIDIOC_QUERYCAP, &prm->cap) < 0) {
        printf("%s: Not a V4L2 device?\n", prm->dev);
        return -1;
    }

    if ( prm->show_all || prm->show_caps ) {
        printf("%s information:\n", prm->dev);
        printf("cap.driver: \"%s\"\n", prm->cap.driver);
        printf("cap.card: \"%s\"\n", prm->cap.card);
        printf("cap.bus_info: \"%s\"\n", prm->cap.bus_info);
        printf("cap.capabilities=0x%08X\n", prm->cap.capabilities);
        if(prm->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) printf("- VIDEO_CAPTURE\n");
        if(prm->cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)  printf("- VIDEO_OUTPUT\n");
        if(prm->cap.capabilities & V4L2_CAP_VIDEO_OVERLAY) printf("- VIDEO_OVERLAY\n");
        if(prm->cap.capabilities & V4L2_CAP_VBI_CAPTURE)   printf("- VBI_CAPTURE\n");
        if(prm->cap.capabilities & V4L2_CAP_VBI_OUTPUT)    printf("- VBI_OUTPUT\n");
        if(prm->cap.capabilities & V4L2_CAP_RDS_CAPTURE)   printf("- RDS_CAPTURE\n");
        if(prm->cap.capabilities & V4L2_CAP_TUNER)         printf("- TUNER\n");
        if(prm->cap.capabilities & V4L2_CAP_AUDIO)         printf("- AUDIO\n");
        if(prm->cap.capabilities & V4L2_CAP_RADIO)         printf("- RADIO\n");
        if(prm->cap.capabilities & V4L2_CAP_READWRITE)     printf("- READWRITE\n");
        if(prm->cap.capabilities & V4L2_CAP_ASYNCIO)       printf("- ASYNCIO\n");
        if(prm->cap.capabilities & V4L2_CAP_STREAMING)     printf("- STREAMING\n");
        if(prm->cap.capabilities & V4L2_CAP_TIMEPERFRAME)  printf("- TIMEPERFRAME\n");
    }

    if(!prm->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
        printf("Device does not support capturing.\n");
        return -1;
    }

    struct v4l2_audio audio_cap;
    memset(&audio_cap, 0, sizeof(struct v4l2_audio));
    if(ioctl(prm->fd, VIDIOC_G_AUDIO, &audio_cap) < 0) {
        printf("%s: Not a V4L2 audio device?\n", prm->dev);
        return 0;
    } else if ( prm->show_all || prm->show_inputs ) {
        printf("Audio %u (%s) : cap = %X  ,  mode = %X\n", audio_cap.index, audio_cap.name, audio_cap.capability, audio_cap.mode);
    }

    printf("Device capabilities ok.\n");
    return 0;
}

/*
 *
 */
int f641_set_input(struct f641_v4l2_parameters *prm)
{
    struct v4l2_input inpt;
    int count = 0, i = -1;

    memset(&inpt, 0, sizeof(inpt));

    if ( prm->show_all || prm->show_inputs ) {
        printf("--- Available inputs :\n");
        inpt.index = count;
        while(!ioctl(prm->fd, VIDIOC_ENUMINPUT, &inpt)) {
//            printf("%i: \"%s\" : std-0x%X : type %u : status 0x%X\n", count, inpt.name, inpt.std, inpt.type, inpt.status);
            inpt.index = ++count;
        }
    }

    /* If no input was specified, use input 0. */
    if(!prm->input) {
        if ( prm->verbose ) printf("No input was specified, using the first.\n");
        count = 1;
        i = 0;
    }

    /* Set the input. */
    inpt.index = i;
    if(ioctl(prm->fd, VIDIOC_ENUMINPUT, &inpt) == -1) {
        printf("Unable to query input %i.\n", i);
        printf("VIDIOC_ENUMINPUT: %s\n", strerror(errno));
        return -1;
    }

    if ( prm->show_all || prm->show_inputs ) {
        printf("%s: Input %i information :\n", prm->source, i);
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

    if(ioctl(prm->fd, VIDIOC_S_INPUT, &i) == -1) {
        printf("Error selecting input %i\n", i);
        printf("VIDIOC_S_INPUT: %s\n", strerror(errno));
        return -1;
    } else {
        if ( prm->show_all || prm->show_vidstds ) {
            printf("Video standard :\n");
            struct v4l2_standard std;
            memset(&std, 0, sizeof(std));
            std.index = count = 0;
            while(!ioctl(prm->fd, VIDIOC_ENUMSTD, &std)) {
                printf("\t-standard %d :\n", count);
                memset(&std, 0, sizeof(std));
                std.index = ++count;
            }
        }
    }
    if ( prm->verbose ) printf("Selecting input %i\n", i);

    return 0;
}

/*
 *
 */
static int f640_show_control(struct f641_v4l2_parameters *prm, struct v4l2_queryctrl *queryctrl)
{
    struct v4l2_querymenu querymenu;
    struct v4l2_control control;
    char *t;
    int m;

    if ( !prm->show_all && !prm->show_controls ) return 0;
    if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) {
        printf("Control \"%s\" disabled.\n", queryctrl->name);
        return 0;
    }

    memset(&querymenu, 0, sizeof(querymenu));
    memset(&control, 0, sizeof(control));

    if(queryctrl->type != V4L2_CTRL_TYPE_BUTTON) {
        control.id = queryctrl->id;
        if(ioctl(prm->fd, VIDIOC_G_CTRL, &control)) {
            printf("Error reading value of control '%s'.\n", queryctrl->name);
            printf("VIDIOC_G_CTRL: %s\n", strerror(errno));
        }
        prm->controls_value[prm->nb_controls] = control.value;
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
                if(!ioctl(prm->fd, VIDIOC_QUERYMENU, &querymenu)) {
                    strncat(t, (char *) querymenu.name, 32);
                    if(m < queryctrl->maximum) strncat(t, " | \n", 3);
                }
            }
            querymenu.index = control.value;
            if(ioctl(prm->fd, VIDIOC_QUERYMENU, &querymenu)) {
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
//static int f640_set_control(struct f641_v4l2_parameters *prm, struct v4l2_queryctrl *queryctrl)
//{
//    struct v4l2_control control;
//    struct v4l2_querymenu querymenu;
//    char *sv;
//    int iv;
//
//    if(queryctrl->flags & V4L2_CTRL_FLAG_DISABLED) {
//        if (!prm->quiet) printf("Control \"%s\" disabled, don't update it.\n", queryctrl->name);
//        return 0;
//    }
//
//    memset(&querymenu, 0, sizeof(querymenu));
//    memset(&control, 0, sizeof(control));
//
//    control.id = queryctrl->id;
//
//    switch(queryctrl->type)
//    {
//        case V4L2_CTRL_TYPE_INTEGER:
//
//            /* Convert the value to an integer. */
//            iv = atoi(sv);
//            /* Is the value a precentage? */
//            if(strchr(sv, '%')) {
//                /* Adjust the precentage to fit the controls range. */
//                iv = SCALE(queryctrl->minimum, queryctrl->maximum, 0, 100, iv);
//            }
//            if (!prm->quiet) printf("Setting %s to %i (%i%%).\n"
//                    , queryctrl->name, iv, SCALE(0, 100, queryctrl->minimum, queryctrl->maximum, iv));
//
//            if(iv < queryctrl->minimum || iv > queryctrl->maximum)
//                if (!prm->quiet) printf("Value is out of range. Setting anyway.\n");
//
//            control.value = iv;
//            ioctl(prm->fd, VIDIOC_S_CTRL, &control);
//            break;
//
//        case V4L2_CTRL_TYPE_BOOLEAN:
//            iv = -1;
//            if(!strcasecmp(sv, "1") || !strcasecmp(sv, "true")) iv = 1;
//            if(!strcasecmp(sv, "0") || !strcasecmp(sv, "false")) iv = 0;
//
//            if(iv == -1) {
//                if (!prm->quiet) printf("Unknown boolean value '%s' for %s.\n", sv, queryctrl->name);
//                return -1;
//            }
//            if (!prm->quiet) printf("Setting %s to %s (%i).\n", queryctrl->name, sv, iv);
//            control.value = iv;
//            ioctl(prm->fd, VIDIOC_S_CTRL, &control);
//            break;
//        case V4L2_CTRL_TYPE_MENU:
//            /* Scan for a matching value. */
//            querymenu.id = queryctrl->id;
//            for(iv = queryctrl->minimum; iv <= queryctrl->maximum; iv++)
//            {
//                querymenu.index = iv;
//                if(ioctl(prm->fd, VIDIOC_QUERYMENU, &querymenu)) {
//                    printf("Error querying menu.\n");
//                    continue;
//                }
//                if(!strncasecmp((char *) querymenu.name, sv, 32)) break;
//            }
//            if(iv > queryctrl->maximum) {
//                if (!prm->quiet) printf("Unknown value '%s' for %s.\n", sv, queryctrl->name);
//                return -1;
//            }
//            if (!prm->quiet) printf("Setting %s to %s (%i).\n", queryctrl->name, querymenu.name, iv);
//            control.value = iv;
//            ioctl(prm->fd, VIDIOC_S_CTRL, &control);
//            break;
//        case V4L2_CTRL_TYPE_BUTTON:
//            if (!prm->quiet) printf("Triggering %s control.\n", queryctrl->name);
//            ioctl(prm->fd, VIDIOC_S_CTRL, &control);
//            break;
//        default:
//            if (!prm->quiet) printf("Not setting unknown control type (%s).\n", queryctrl->name);
//            break;
//    }
//    return 0;
//}


/*
 * uvcvideo: Dropping payload (out of sync).
 */
int f641_list_controls(struct f641_v4l2_parameters *prm)
{
    struct v4l2_queryctrl queryctrl;
    int c;

    if (!prm->show_all && !prm->show_controls) return 0;

    // Display normal controls.
    printf(F640_UNDER "User controls :\n" F640_RESET);
    for(c = V4L2_CID_BASE; c < V4L2_CID_LASTP1; c++) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(prm->fd, VIDIOC_QUERYCTRL, &queryctrl)) continue;

        memcpy(&prm->controls[prm->nb_controls], &queryctrl, sizeof(queryctrl));
        f640_show_control(prm, &queryctrl);
        prm->nb_controls++;
    }
    // Display device-specific controls.
    printf(F640_UNDER "Device-specific controls :\n" F640_RESET);
    for(c = V4L2_CID_PRIVATE_BASE; ; c++) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(prm->fd, VIDIOC_QUERYCTRL, &queryctrl)) break;

        memcpy(&prm->controls[prm->nb_controls], &queryctrl, sizeof(queryctrl));
        f640_show_control(prm, &queryctrl);
        prm->nb_controls++;
    }


    // EXPOSURE
//    memset(&queryctrl, 0, sizeof(queryctrl));
//    queryctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
//    if(ioctl(prm->fd, VIDIOC_QUERYCTRL, &queryctrl)) {
//        printf("No exposure control... : %s\n", queryctrl.name);
//    } else {
//        f640_show_control(prm, &queryctrl);
//        int e;
//        for(e = 30000 ; e < 270000 ; e += 10000) {
//            struct v4l2_control control;
//            control.id = queryctrl.id;
//            control.value = e;
//            c = ioctl(prm->fd, VIDIOC_S_CTRL, &control);
//            printf("Set exposure to %d, return %d, %s\n", e, c, c ? strerror(errno) : "OK");
//        }
//    }

    // Camera-class controls
    printf(F640_UNDER "Camera-class controls :\n" F640_RESET);
    for(c = V4L2_CID_EXPOSURE_AUTO; c < V4L2_CID_CAMERA_CLASS_BASE + 19 ; c++ ) {
        memset(&queryctrl, 0, sizeof(queryctrl));
        queryctrl.id = c;
        if(ioctl(prm->fd, VIDIOC_QUERYCTRL, &queryctrl)) {
            continue;
        } else {
            memcpy(&prm->controls[prm->nb_controls], &queryctrl, sizeof(queryctrl));
            f640_show_control(prm, &queryctrl);
            prm->nb_controls++;
        }
    }

    return 0;
}


/*
 *
 */
int f641_set_pix_format(struct f641_v4l2_parameters *prm) {
    struct v4l2_fmtdesc fmt;
    int r, v4l2_pal;

    /* Dump a list of formats the device supports. */
    if ( prm->show_all || prm->show_formats )
        printf("Device offers the following V4L2 pixel formats :\n");

    v4l2_pal = 0;
    memset(&fmt, 0, sizeof(fmt));
    fmt.index = v4l2_pal;
    fmt.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while(ioctl(prm->fd, VIDIOC_ENUM_FMT, &fmt) != -1)
    {
        if ( prm->show_all || prm->show_formats )
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
        if (fmt.pixelformat == prm->palette) memcpy(&prm->format_desc, &fmt, sizeof(fmt));

        memset(&fmt, 0, sizeof(fmt));
        fmt.index = ++v4l2_pal;
        fmt.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    }


    /* Try the palette... */
    memset(&prm->format, 0, sizeof(prm->format));
    prm->format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    prm->format.fmt.pix.width       = prm->width;
    prm->format.fmt.pix.height      = prm->height;
    prm->format.fmt.pix.pixelformat = prm->format_desc.pixelformat;
    prm->format.fmt.pix.field       = V4L2_FIELD_ANY;//V4L2_FIELD_NONE;//V4L2_FIELD_ANY;

    if ( prm->show_all || prm->show_formats ) printf("Try the palette 0x%X (%u, %u):\n", prm->format_desc.pixelformat, prm->format.fmt.pix.width, prm->format.fmt.pix.height);
    if( ( r = ioctl(prm->fd, VIDIOC_TRY_FMT, &prm->format) ) != -1 ) {
        if ( prm->show_all || prm->show_formats )
            printf("Using palette 0x%X : field = %d, colorspace = %d, bytes/line = %u, size=%u\n"
                , prm->format_desc.pixelformat
                , prm->format.fmt.pix.field
                , prm->format.fmt.pix.colorspace
                , prm->format.fmt.pix.bytesperline
                , prm->format.fmt.pix.sizeimage
        );

        if(prm->format.fmt.pix.width != prm->width || prm->format.fmt.pix.height != prm->height) {
            if ( prm->show_all || prm->show_formats )
                printf("Adjusting resolution from %ix%i to %ix%i.\n",
                        prm->width, prm->height, prm->format.fmt.pix.width, prm->format.fmt.pix.height);
            prm->width = prm->format.fmt.pix.width;
            prm->height = prm->format.fmt.pix.height;
        }

        if(ioctl(prm->fd, VIDIOC_S_FMT, &prm->format) == -1) {
            printf("Error setting pixel format.\n");
            printf("VIDIOC_S_FMT: %s\n", strerror(errno));
            return -1;
        }
        if ( prm->verbose ) printf("Setting pixel format ok.\n");

        return 0;
    }
    if ( prm->verbose ) printf("Setting pixel format ko (%d) : %s\n", r, strerror(errno));
    return -1;
}

/*
 *
 */
int f641_set_parm(struct f641_v4l2_parameters *prm)
{
    struct v4l2_streamparm parm;

    memset (&parm, 0, sizeof (parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (prm->fd, VIDIOC_G_PARM, &parm) == -1) {
        perror("VIDIOC_G_INPUT");
        return -1;
    }

    if ( prm->show_all || prm->show_framints )
        printf ("GetParm : cap = 0x%X, mode = 0x%X, emode = 0x%X, buf = %u, fps = %u / %u\n"
            , parm.parm.capture.capability
            , parm.parm.capture.capturemode
            , parm.parm.capture.extendedmode
            , parm.parm.capture.readbuffers
            , parm.parm.capture.timeperframe.numerator, parm.parm.capture.timeperframe.denominator
    );

    if ( parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME ) {
        parm.parm.capture.timeperframe.denominator = prm->frames_pers;
        if (ioctl (prm->fd, VIDIOC_S_PARM, &parm) == -1) {
            perror ("VIDIOC_S_INPUT");
            if ( prm->verbose )
                printf("SetStreamParm to %ufrm / %us : ko.\n"
                    , parm.parm.capture.timeperframe.denominator, parm.parm.capture.timeperframe.numerator);
        } else {
            if ( prm->verbose )
                printf("SetStreamParm to %ufrm / %us : ok (asked = %d/s)\n"
                    , parm.parm.capture.timeperframe.denominator, parm.parm.capture.timeperframe.numerator, prm->frames_pers);
        }
    } else {
        if ( prm->verbose )
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
    sizn.pixel_format = prm->format.fmt.pix.pixelformat;
    while ( ioctl(prm->fd, VIDIOC_ENUM_FRAMESIZES, &sizn) != -1 ) {
        if ( prm->show_all || prm->show_framsize )
            printf("SizeFormat %d : type = %d - %s :\n"
                , sizn.index
                , sizn.type
                , sizn.type == V4L2_FRMSIZE_TYPE_DISCRETE ? "Discrete"
                : sizn.type == V4L2_FRMSIZE_TYPE_STEPWISE ? "StepWise"
                : "Continuous"
        );
        if ( sizn.type == V4L2_FRMSIZE_TYPE_DISCRETE ) {
            if ( prm->show_all || prm->show_framsize )
                printf("\t%ux%u : ", sizn.discrete.width, sizn.discrete.height);
            memset(&fps, 0, sizeof(fps));
            fps.index = indf = 0;
            fps.pixel_format = prm->format.fmt.pix.pixelformat;
            fps.width = sizn.discrete.width;
            fps.height = sizn.discrete.height;
            while ( ioctl(prm->fd, VIDIOC_ENUM_FRAMEINTERVALS, &fps) != -1 ) {
                if( fps.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                    if ( prm->show_all || prm->show_framsize )
                        printf("| %u/%u |", fps.discrete.numerator, fps.discrete.denominator);
                } else {
                    if ( prm->show_all || prm->show_framsize )
                        printf("%u/%u < %u/%u < %u/%u (stepwise)", fps.stepwise.min.numerator, fps.stepwise.min.denominator, fps.stepwise.step.numerator, fps.stepwise.step.denominator, fps.stepwise.max.numerator, fps.stepwise.max.denominator);
                }
                memset(&fps, 0, sizeof(fps));
                fps.index = ++indf;
                fps.pixel_format = prm->format.fmt.pix.pixelformat;
                fps.width = sizn.discrete.width;
                fps.height = sizn.discrete.height;
            }
            if ( prm->show_all || prm->show_framsize ) printf("\n");
        } else {
            if ( prm->show_all || prm->show_framsize )
                printf("\t(%u , %u)x(%u , %u) : (%u , %u)\n"
                    , sizn.stepwise.min_width, sizn.stepwise.max_width
                    , sizn.stepwise.min_height, sizn.stepwise.max_height
                    , sizn.stepwise.step_width, sizn.stepwise.step_height
            );
        }
        memset(&sizn, 0, sizeof(sizn));
        sizn.index = ++inds;
        sizn.pixel_format = prm->format.fmt.pix.pixelformat;
    }

    return 0;
}

/*
 *
 */
int f641_set_mmap(struct f641_v4l2_parameters *prm, int nb_buffers) {
    uint32_t i, b;

    prm->memory = V4L2_MEMORY_MMAP;

    /* Does the device support streaming? */
    if(~prm->cap.capabilities & V4L2_CAP_STREAMING) {
        if (!prm->quiet) printf("No streaming cap, returning.\n");
        return -1;
    }
    printf("SET MMAP : %d\n", nb_buffers);

    prm->buffers = calloc(nb_buffers, sizeof(v4l2_buffer_t));

    memset(&prm->req, 0, sizeof(prm->req));
    prm->req.count  = nb_buffers;
    prm->req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    prm->req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(prm->fd, VIDIOC_REQBUFS, &prm->req) == -1) {
        if (!prm->quiet) printf("Error requesting %d buffers for memory map.\n", nb_buffers);
        if (errno == -EINVAL) {
            if (!prm->quiet)
                printf ("Video capturing or mmap-streaming is not supported: %s\n", strerror(errno));
        } else if (!prm->quiet) {
            printf("VIDIOC_REQBUFS: %s\n", strerror(errno));
        }
        return -1;
    }

    if (!prm->quiet) printf("mmap information :\n");
    if (!prm->quiet) printf("frames = %d ( %u / %u )\n", prm->req.count, prm->req.reserved[0], prm->req.reserved[1]);

    if(prm->req.count < 2) {
        if (!prm->quiet) printf("Insufficient buffer memory.\n");
        return -1;
    }

    for(b = 0 ; b < prm->req.count ; b++)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));

        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = b;

        if(ioctl(prm->fd, VIDIOC_QUERYBUF, &buf) == -1) {
            printf("Error querying buffer %i\n", b);
            printf("VIDIOC_QUERYBUF: %s\n", strerror(errno));
            return -1;
        }
//        if (!prm->quiet) printf("QueryBuf : index = %u, len = %u, input = %u, offset = %u, ptr = %lX", buf.index, buf.length, buf.input, buf.m.offset, buf.m.userptr);
        prm->buffers[b].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, prm->fd, buf.m.offset);
        prm->buffers[b].length = buf.length;
//        if (!prm->quiet) printf(", mmap = %p\n", prm->buffers[b].start);

        if(prm->buffers[b].start == MAP_FAILED) {
            printf("Error mapping buffer %i\n", b);
            printf("mmap: %s\n", strerror(errno));
            prm->req.count = b;
            for(i = 0; i < prm->req.count; i++) munmap(prm->buffers[i].start, prm->buffers[i].length);
            return -1;
        }

        if ( prm->verbose ) printf("%i length=%d\n", b, buf.length);
    }

    return 0;
}

int f641_prepare_buffers(struct f641_v4l2_parameters *prm) {
    int b, i;
    for(b = 0; b < prm->req.count; b++) {
        memset(&prm->buf, 0, sizeof(prm->buf));

        prm->buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        prm->buf.memory = V4L2_MEMORY_MMAP;
        prm->buf.index  = b;

        if(ioctl(prm->fd, VIDIOC_QBUF, &prm->buf) == -1) {
            printf("VIDIOC_QBUF: %s\n", strerror(errno));
            for(i = 0; i < prm->req.count; i++) munmap(prm->buffers[i].start, prm->buffers[i].length);
            return -1;
        }
    }
    return 0;
}

/*
 *
 */
int f641_set_uptr(struct f641_v4l2_parameters *prm, int nb_buffers) {
    uint32_t i, b;

    prm->memory = V4L2_MEMORY_USERPTR;
    for(i = 0 ; i < nb_buffers ; i++) {
        prm->buffers[i].length = 2 * prm->width * prm->height;
        prm->buffers[i].start = malloc(prm->buffers[i].length);
    }

    /* Does the device support streaming? */
    if(~prm->cap.capabilities & V4L2_CAP_STREAMING) {
        if (!prm->quiet) printf("No streaming cap, returning.\n");
        return -1;
    }

    printf("SET MMAP : %d\n", nb_buffers);

    memset(&prm->req, 0, sizeof(prm->req));
    prm->req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    prm->req.memory = prm->memory;

    if(ioctl(prm->fd, VIDIOC_REQBUFS, &prm->req) == -1) {
        if (!prm->quiet) printf("Error requesting %d buffers for memory map.\n", nb_buffers);
        if (errno == -EINVAL)
            if (!prm->quiet)
                printf ("Video capturing or mmap-streaming is not supported: %s\n", strerror(errno));
        printf("VIDIOC_REQBUFS: %s\n", strerror(errno));

        return -1;
    }

    if (!prm->quiet) printf("mmap information :\n");
    if (!prm->quiet) printf("frames = %d ( %u / %u )\n", prm->req.count, prm->req.reserved[0], prm->req.reserved[1]);
    prm->req.count = nb_buffers;

    if(prm->req.count < 2) {
        if (!prm->quiet) printf("Insufficient buffer memory.\n");
        return -1;
    }

    for(b = 0; b < prm->req.count; b++)
    {
        memset(&prm->buf, 0, sizeof(prm->buf));
        prm->buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        prm->buf.memory = prm->memory;
        prm->buf.length = prm->buffers[b % prm->req.count].length;
        prm->buf.m.userptr = (unsigned long)prm->buffers[b % prm->req.count].start;

        if(ioctl(prm->fd, VIDIOC_QBUF, &prm->buf) == -1) {
            printf("VIDIOC_QBUF: %s\n", strerror(errno));
            return -1;
        }
        printf("QBuf : index = %u, len = %u, input = %u, offset = %u, ptr = %lX\n", prm->buf.index, prm->buf.length, prm->buf.input, prm->buf.m.offset, prm->buf.m.userptr);
    }
    return 0;
}

int f641_stream_on(struct f641_v4l2_parameters *prm) {
    int i;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(prm->fd, VIDIOC_STREAMON, &type) == -1) {
        printf("Error starting stream on \"%s\"\n", prm->dev);
        printf("VIDIOC_STREAMON: %s\n", strerror(errno));
        for(i = 0; i < prm->req.count; i++) {
            munmap(prm->buffers[i].start, prm->buffers[i].length);
        }
        return -1;
    }
    if ( prm->verbose ) printf("Stream ON with %u buffers.\n", prm->req.count);
    return 0;
}

// TEST
/*
 *
 */
int f641_test_dq(struct f641_v4l2_parameters *prm, int nb) {
    int i;
//    clock_t times1, times2;
//    times1 = clock();

    for(i = 0 ; i < 50 * nb ; i++) {
        // DeQueue
        struct v4l2_buffer frame;
        memset(&frame, 0, sizeof(struct v4l2_buffer));
        frame.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        frame.memory = prm->memory; // V4L2_MEMORY_MMAP
        if(ioctl(prm->fd, VIDIOC_DQBUF, &frame) == -1) {
            printf("VIDIOC_DQBUF: %s\n", strerror(errno));
            return -1;
        }
//        printf("DQBuf : index = %u, used/len = %u / %u, input = %u, offset = %u, ptr = %p\n", frame.index, frame.bytesused, frame.length, frame.input, frame.m.offset, frame.m.userptr);

        // EnQueue
        if (prm->memory == V4L2_MEMORY_MMAP) {
            if(ioctl(prm->fd, VIDIOC_QBUF, &frame) == -1) {
                printf("VIDIOC_QBUF: %s\n", strerror(errno));
                return -1;
            }
        } else if (prm->memory == V4L2_MEMORY_USERPTR) {
            memset(&frame, 0, sizeof(struct v4l2_buffer));
            frame.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            frame.memory = prm->memory; // V4L2_MEMORY_MMAP
            frame.m.userptr = (unsigned long) prm->buffers[i % prm->req.count].start;
            frame.length = prm->buffers[i % prm->req.count].length;
            if(ioctl(prm->fd, VIDIOC_QBUF, &frame) == -1) {
                printf("VIDIOC_QBUF: %s\n", strerror(errno));
                return -1;
            }
        }
    }
//    times2 = clock();
//    printf("DQ take %ld clock\n", times2 - times1);

    return 0;
}


/*
 *
 */
int f641_stream_off(struct f641_v4l2_parameters *prm)
{
    int i;
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(prm->fd, VIDIOC_STREAMOFF, &type) == -1) {
        printf("Error stoping stream.\n");
        printf("VIDIOC_STREAMOFF: %s\n", strerror(errno));
    } else {
        if (prm->verbose) printf("STREAMOFF succed.\n");
    }

    for(i = 0; i < prm->req.count; i++)
        munmap(prm->buffers[i].start, prm->buffers[i].length);

    return 0;
}

//
int f641_set_defaults(struct f641_v4l2_parameters *prm, int gain, int sharp, int white, int expo) {
    int r;
    struct v4l2_control control;

    // GAIN
    control.id = V4L2_CID_GAIN;
    control.value = gain;
    r = ioctl(prm->fd, VIDIOC_S_CTRL, &control);
    printf("Set gain to %d (%d asked), return %d, %s\n", control.value, gain, r, r ? strerror(errno) : "OK");

    // SHARP
    control.id = V4L2_CID_SHARPNESS;
    control.value = sharp;
    r = ioctl(prm->fd, VIDIOC_S_CTRL, &control);
    printf("Set sharpness to %d (%d asked), return %d, %s\n", control.value, sharp, r, r ? strerror(errno) : "OK");

    // WHITE
    control.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
    control.value = white;
    r = ioctl(prm->fd, VIDIOC_S_CTRL, &control);
    printf("Set exposure to %d (%d asked), return %d, %s\n", control.value, white, r, r ? strerror(errno) : "OK");

    // EXPOSURE
    control.id = V4L2_CID_EXPOSURE_ABSOLUTE;
    control.value = expo;
    r = ioctl(prm->fd, VIDIOC_S_CTRL, &control);
    printf("Set exposure to %d (%d asked), return %d, %s\n", control.value, expo, r, r ? strerror(errno) : "OK");

    return 0;
}

void f641_setup_v4l2(struct f641_v4l2_parameters *prm, const char *dev, int width, int height, int palette, int rate, int buffers) {
    int r;

    if (!prm) {
        printf("No v4l2 parameters, returning\n");
        return;
    }
    prm->debug = 0;
    snprintf(prm->dev, sizeof(prm->dev), dev);
    prm->fd  = open(prm->dev, O_RDWR);
    snprintf(prm->source, sizeof(prm->source), dev);
    prm->width       = width;
    prm->height      = height;
    prm->frames_pers = rate;
    prm->palette     = palette;   //0x47504A4D;   // 0x56595559
    prm->memory      = V4L2_MEMORY_MMAP;
    prm->capture         = 1;
    prm->verbose         = 1;
    prm->quiet           = 0;
    prm->debug           = 1;
    prm->show_all        = 1;
    prm->show_caps       = 1;
    prm->show_inputs     = 1;
    prm->show_controls   = 1;
    prm->show_formats    = 1;
    prm->show_framsize   = 1;
    prm->show_framints   = 1;
    prm->show_vidstds    = 1;

    printf("V4L2 prm calloced\n");

    // Capabilities
    if ( f641_get_capabilities(prm) < 0 ) {
        printf("Capabilities KO\n");
        return;
    }
    printf("Capabilities ok\n");

    // Input
    r = f641_set_input(prm);

    // Controls
    r = f641_list_controls(prm);

    // Format
    r = f641_set_pix_format(prm);

    // Parm
    r = f641_set_parm(prm);

    // Map
    prm->memory = V4L2_MEMORY_MMAP;
    if ( prm->capture ) {
        if (prm->memory == V4L2_MEMORY_MMAP) r = f641_set_mmap(prm, buffers);
        else if (prm->memory == V4L2_MEMORY_USERPTR) r = f641_set_uptr(prm, buffers);
    }
}
