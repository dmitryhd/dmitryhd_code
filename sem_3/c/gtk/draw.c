#include <cairo.h>
#include <gtk/gtk.h>

#define SUCCESS 0
#define WID	300.0
#define HEIG	300.0
#define M_PI	3.1415
float xb=10,yb=12,xe=120,ye=120;
unsigned int field_width=10,field_height=10;
int x=1,y=2;
void on_expose_event(GtkWidget *wid,GdkEventExpose *event,gpointer data);
void print_cells(GtkWidget *wid);

int main(int argc,char *argv[]){
	GtkWidget * win, *draw, *hbox, *rbox;
	GtkWidget * button;
	gtk_init(&argc,&argv);
	win=gtk_window_new(GTK_WINDOW_TOPLEVEL);
 	gtk_window_set_default_size(GTK_WINDOW(win), WID+10, HEIG+10); 
	draw=gtk_drawing_area_new();
	gtk_widget_show(draw);
	
	hbox = gtk_hbox_new(FALSE, 0);
	rbox = gtk_vbox_new(FALSE, 10);
	gtk_container_add(GTK_CONTAINER(hbox),draw);
	gtk_container_add(GTK_CONTAINER(win),hbox);
	
  	g_signal_connect(draw, "expose-event",G_CALLBACK(on_expose_event), NULL);
	gtk_widget_show_all(win);

	gtk_main();
	return SUCCESS;		
}
