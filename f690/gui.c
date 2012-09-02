/*
 *  empty.c
 *
 *  Created on: 6 mai 2011
 *  Author: gk
 */
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "gtk/gtk.h"
#include "cairo.h"

/*
 * 		Prototypes
 */
static void hello( GtkWidget *widget, gpointer   data );
static gboolean delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data );
static void destroy( GtkWidget *widget, gpointer   data );


/*
 *
 */
/* This is a callback function. The data arguments are ignored
 * in this example. More on callbacks below. */
static void hello( GtkWidget *widget, gpointer   data )
{
    g_print ("Hello !\n");
}

static gboolean delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data )
{
    /* If you return FALSE in the "delete-event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete-event". */

    return FALSE;
}

/* Another callback */
static void destroy( GtkWidget *widget, gpointer   data )
{
    gtk_main_quit ();
}

/*
 *
 */
static gboolean on_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	gint x = widget->allocation.x;
	gint y = widget->allocation.y;
	gint width = widget->allocation.width;
	gint height = widget->allocation.height;

	cairo_t *cr;

	cr = gdk_cairo_create (widget->window);

	cairo_set_source_rgba(cr, 0, 0, 0, 1);

	static const double dashed1[] = {4.0, 1.0};
	static int len1  = sizeof(dashed1) / sizeof(dashed1[0]);

	static const double dashed2[] = {4.0, 1.0, 4.0};
	static int len2  = sizeof(dashed2) / sizeof(dashed2[0]);

	static const double dashed3[] = {1.0};

	/*
	 *       Geometry
	 */
	cairo_select_font_face(cr, "Purisa", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 13);

	char s[16];
	snprintf(s, 15, "( %i , %i )", x , y);
	cairo_move_to(cr, 10, height-1);
	cairo_show_text(cr, s);

	snprintf(s, 15, "( %i , %i )", width, height );
	cairo_move_to(cr, width/2 , height -1);
	cairo_show_text(cr, s);

	/*
	 *         DASH
	 */
	cairo_set_line_width(cr, 1.5);

	cairo_set_dash(cr, dashed1, len1, 0);
	cairo_move_to(cr, 40, 30);
	cairo_line_to(cr, 200, 30);
	cairo_stroke(cr);

	cairo_set_dash(cr, dashed2, len2, 1);
	cairo_move_to(cr, 40, 50);
	cairo_line_to(cr, 200, 50);
	cairo_stroke(cr);

	cairo_set_dash(cr, dashed3, 1, 0);
	cairo_move_to(cr, 40, 70);
	cairo_line_to(cr, 200, 70);
	cairo_stroke(cr);

	//cairo_set_dash(cr, nodash, 0, 0);
	cairo_set_dash(cr, (double[]){}, 0, 0);

      /*
       *        Rectangles
       */
	cairo_set_source_rgb(cr, 0.69, 0.19, 0);
	cairo_rectangle(cr, 30, 30, 100, 100);
	cairo_set_line_width(cr, 14);
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.3, 0.4, 0.6);
	cairo_rectangle(cr, 160, 30, 100, 100);
	cairo_set_line_width(cr, 14);
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
	cairo_stroke(cr);

	cairo_set_source_rgb(cr, 0.3, 0.8, 0.2);
	cairo_rectangle(cr, 100, 160, 100, 100);
	cairo_set_line_width(cr, 14);
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
	cairo_stroke(cr);

	/*
	 *      Time
	 */
	char buffer[256];
	time_t curtime;
	struct tm *loctime;

	curtime = time(NULL);
	loctime = localtime(&curtime);
	//strftime(buffer, 256, "%T", loctime);

	//
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	suseconds_t deb = 0;
	static suseconds_t wait = 0;

	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
        deb = tv.tv_usec;

	snprintf(buffer, 255, "%d:%02d:%02d %d %d", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec, deb>wait ? deb-wait : deb+1000000-wait);

	tm = localtime(&tv.tv_sec);
        wait = tv.tv_usec;

	cairo_move_to(cr, 10 , height - 30);
	cairo_show_text(cr, buffer);


	/*
	 *
	 */
        /*
         *
         */

        /*
         *
         */
        static FILE *fp;
        char info[256];
        if (!fp)
        {
          fp = fopen ("/proc/stat", "r");
          setvbuf (fp, NULL, _IONBF, 0);
        }
        //rewind(fp);
        //fflush(fp);
        //fgets(info, 33, fp);

                //
        static int cp = 0;
        int cpt = 0;
        static long l0[9] = {0,0,0,0,0,0,0,0,0};
        static long l[9] = {0,0,0,0,0,0,0,0,0};
        char *tmp;
        const int us = 20000;

        // Time
        gettimeofday(&tv, &tz);
        tm = localtime(&tv.tv_sec);
        deb = tv.tv_usec;

        //move(9, 2);
        //printw("%d:%02d:%02d %03d %07d", tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec/1000, deb > wait ? deb - wait : 1000000 - wait + deb);

        // Proc
        rewind(fp);
        fflush(fp);
        fgets(info, 255, fp);
        tmp = info;
        tmp +=5;
        cairo_move_to(cr, 10 , height - 45);
        cairo_show_text(cr, tmp);

        //move(10, 2);
        //printw(info);

        // Proc
        //tmp += 4;

      for(cpt = 0 ; cpt < 9 ; cpt++)
      {
          char *tail;
          int next;

          /* Skip whitespace by hand, to detect the end.  */
          while (isspace (*tmp)) tmp++;
          if (*tmp == 0)
          break;


          if ( cp % 10 == 1 )
          {
              l0[cpt] = l[cpt];
              //printf("%i = %i\n", cpt, l0[cpt] );
          }
          /* There is more nonwhitespace,  */
          /* so it ought to be another number.  */
          errno = 0;
          /* Parse it.  */
          l[cpt] = strtol (tmp, &tail, 0);
          /* Add it in, if not overflow.  */
          if (errno)
          {
            printf ("Overflow\n");
            l[cpt] = -1;
          }

          /* Advance past it.  */
          tmp = tail;
        }

        double tproc = 100.0 * ( l[0]-l0[0]+l[1]-l0[1]+l[2]-l0[2]+l[4]-l0[4]+l[5]-l0[5]+l[6]-l0[6]+l[7]-l0[7]+l[8]-l0[8] ) / ( l[0]-l0[0]+ l[1]-l0[1] + l[2]-l0[2] + l[3]-l0[3]+l[4]-l0[4]+l[5]-l0[5]+l[6]-l0[6]+l[7]-l0[7]+l[8]-l0[8] );

        snprintf(info, 255, "%i = %i - %i - %i - %i - %i - %i - %i - %i - %i", (int)tproc, l[0]-l0[0], l[1]-l0[1], l[2]-l0[2], l[3]-l0[3], l[4]-l0[4], l[5]-l0[5], l[6]-l0[6], l[7]-l0[7], l[8]-l0[8]);
        cairo_move_to(cr, 10 , height - 15);
        cairo_show_text(cr, info);

        snprintf(info, 255, "%i = %i - %i - %i - %i - %i - %i - %i - %i - %i", (int)tproc, l0[0],l0[1],l0[2],l0[3],l0[4],l0[5],l0[6],l0[7],l0[8]);
        cairo_move_to(cr, 10 , height - 60);
        cairo_show_text(cr, info);

        snprintf(info, 255, "proc = %i", (int)tproc);
        cairo_move_to(cr, 500 , height - 60);
        cairo_show_text(cr, info);

        gettimeofday(&tv, &tz);
        tm = localtime(&tv.tv_sec);
        wait = tv.tv_usec;

        cp++;

	cairo_destroy(cr);

	return FALSE;
}


static gboolean time_handler(GtkWidget *widget)
{
  gtk_widget_queue_draw(widget);
  return TRUE;
}


/*
 *
 */
int main( int argc, char *argv[] )
{
    /* GtkWidget is the storage type for widgets */
    GtkWidget *window;
    GtkWidget *fixed;
    GtkWidget *button;
    GtkWidget *darea;

    /* This is called in all GTK applications. Arguments are parsed
     * from the command line and are returned to the application. */
    gtk_init (&argc, &argv);

    /* create a new window */
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* When the window is given the "delete-event" signal (this is given
     * by the window manager, usually by the "close" option, or on the
     * titlebar), we ask it to call the delete_event () function
     * as defined above. The data passed to the callback
     * function is NULL and is ignored in the callback function. */
    g_signal_connect (window, "delete-event", G_CALLBACK (delete_event), NULL);

    /* Here we connect the "destroy" event to a signal handler.
     * This event occurs when we call gtk_widget_destroy() on the window,
     * or if we return FALSE in the "delete-event" callback. */
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    /*
     *
     */

    /* Creates a new button with the label "Hello World". */
    button = gtk_button_new_with_label ("Hello !");

    /* When the button receives the "clicked" signal, it will call the
     * function hello() passing it NULL as its argument.  The hello()
     * function is defined above. */
    g_signal_connect (button, "clicked", G_CALLBACK (hello), NULL);

    /* This will cause the window to be destroyed by calling
     * gtk_widget_destroy(window) when "clicked".  Again, the destroy
     * signal could come from here, or the window manager. */
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);

    /* This packs the button into the window (a gtk container). */
    gtk_fixed_put(GTK_FIXED(fixed), button, 580 , 10);
    //gtk_container_add (GTK_CONTAINER (window), button);

    /*
     *
     */
    darea = gtk_drawing_area_new();

    //gtk_container_add(GTK_CONTAINER(window), darea);
    gtk_fixed_put(GTK_FIXED(fixed), darea, 10, 20);
    gtk_drawing_area_size (GTK_DRAWING_AREA(darea), 660, 320);

    g_signal_connect(darea, "expose-event", G_CALLBACK (on_expose_event), NULL);
    //g_signal_connect(window, "destroy", G_CALLBACK (gtk_main_quit), NULL);


    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    gtk_widget_show(button);
    //gtk_widget_show(darea);
    gtk_widget_show(fixed);
    gtk_widget_show_all(window);

    g_timeout_add(40, (GSourceFunc) time_handler, (gpointer) darea);

    gtk_main();

    return 0;
    /*
     *
     */

    /* This packs the button into the window (a gtk container). */
    gtk_container_add (GTK_CONTAINER (window), button);

    /* The final step is to display this newly created widget. */
    gtk_widget_show (button);

    /* and the window */
    gtk_widget_show (window);

    /* All GTK applications must have a gtk_main(). Control ends here
     * and waits for an event to occur (like a key press or
     * mouse event). */
    gtk_main ();

    return 0;
}

