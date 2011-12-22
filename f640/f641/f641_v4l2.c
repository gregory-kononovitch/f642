/*
 * file    : f641_v4l2.c
 * project : f640
 *
 * Created on: Dec 20, 2011
 * Author and copyright (C) 2011 : Gregory Kononovitch
 * License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
 * There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "f641.h"


/*
 *      V4L2 SNAP THREAD
 */
/*
 *
 */
static int f641_exec_v4l2_snap(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_v4l2_snap_ressource *res = (struct f641_v4l2_snap_ressource*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;

    memset(&line->buf, 0, sizeof(struct v4l2_buffer));
    line->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    line->buf.memory = V4L2_MEMORY_MMAP;
    if(ioctl(app->fd, VIDIOC_DQBUF, &line->buf) == -1) {
        printf("VIDIOC_DQBUF: %s\n", strerror(errno));
        return -1;
    }
    return line->buf.bytesused;
}

/*
 *
 */
static int f641_exec_v4l2_desnap(void *appli, void *ressources, struct f640_stone *stone) {
    struct f641_appli *app = (struct f641_appli*)appli;
    struct f641_v4l2_snap_ressource *res = (struct f641_v4l2_snap_ressource*)ressources;
    struct f640_line *line = (struct f640_line*) stone->private;

    if(ioctl(app->fd, VIDIOC_QBUF, &line->buf) == -1) {
        printf("VIDIOC_QBUF: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
