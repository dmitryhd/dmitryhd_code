#include <gtk/gtk.h>
#define SUCCESS 0
static gboolean delete_event(GtkWidget * widget, GdkEvent * event, gpointer data){
	g_print("Delete event occurred\n");
	return FALSE;
}
static void destroy(GtkWidget * widget, gpointer data){
	g_print("Destroy signal was sent\n");
	gtk_main_quit();
}
static void button_clicked(GtkWidget * widget, gpointer data){
	int i = * (gint *) data;
 	g_print("Button %d was clicked!\n",i);
}
GdkPixbuf *create_pixbuf(const gchar * filename){
	GdkPixbuf *pixbuf;
 	GError *error = NULL;
 	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
   	if(!pixbuf) {
      		fprintf(stderr, "%s\n", error->message);
      		g_error_free(error);
   	}

  	return pixbuf;
}
/*void increase(GtkWidget *widget, gpointer label)
{
  count++;

  sprintf(buf, "%d", count);
  gtk_label_set_text(label, buf);

}*/

int main (int argc, char **argv){
	GtkWidget * window, * button[2], * box;
	gint i1,i2;
	gtk_init(&argc,&argv);

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
//	window=gtk_window_new(GTK_WINDOW_POPUP);
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
	gtk_window_set_title(GTK_WINDOW(window), "zomg, teh gui!");
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window),230,60);
	gtk_window_set_icon(GTK_WINDOW(window),create_pixbuf("icon.png"));
  	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	
	box=gtk_hbox_new(TRUE,2);
	gtk_container_add(GTK_CONTAINER(window),box);

	i1=1;
	button[0] = gtk_button_new_with_label("Quit"); 
 	g_signal_connect(G_OBJECT(button[0]), "clicked", G_CALLBACK(button_clicked), &i1);
  	g_signal_connect_swapped(G_OBJECT(button[0]), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(window));
  	gtk_box_pack_start(GTK_BOX(box), button[0], TRUE, TRUE, 0);
	
	i2=2;
	button[1] = gtk_button_new_with_label("Enter");
  	g_signal_connect(G_OBJECT(button[1]), "clicked", G_CALLBACK(button_clicked), &i2);
  	gtk_box_pack_start(GTK_BOX(box), button[1], TRUE, TRUE, 0);

  	/*
  	frame = gtk_fixed_new();
  	gtk_container_add(GTK_CONTAINER(window), frame);

  	plus = gtk_button_new_with_label("+");
  	gtk_widget_set_size_request(plus, 80, 35);
  	gtk_fixed_put(GTK_FIXED(frame), plus, 50, 20);

	minus = gtk_button_new_with_label("-");
	gtk_widget_set_size_request(minus, 80, 35);
	gtk_fixed_put(GTK_FIXED(frame), minus, 50, 80);

	label = gtk_label_new("0");
	gtk_fixed_put(GTK_FIXED(frame), label, 190, 58);
  g_signal_connect(plus, "clicked", 
      G_CALLBACK(increase), label);
	*/

	gtk_widget_show(button[0]);
	gtk_widget_show(button[1]);
	gtk_widget_show(box);
	gtk_widget_show(window);

	gtk_main();
	return SUCCESS;
}
