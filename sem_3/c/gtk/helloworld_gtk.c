#include <gtk/gtk.h>

static void button_clicked(GtkWidget * widget, gpointer data){
  g_print("Button was clicked!\n");
}

static gboolean delete_event(GtkWidget * widget, GdkEvent * event, gpointer data){
  g_print("Delete event occurred\n");
  return FALSE;
}
 
static void destroy(GtkWidget * widget, gpointer data){
  g_print("Destroy signal was sent\n");
  gtk_main_quit();
}

int main(int argc, char ** argv){
	GtkWidget * window;
  GtkWidget * button;
  const gchar * title = "Hello World!";
  gtk_init(&argc, &argv);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
  button = gtk_button_new_with_label("Quit");
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_clicked), NULL);
  g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(window));
  gtk_container_add(GTK_CONTAINER(window), button);
  gtk_widget_show(button);
  gtk_widget_show(window);
  gtk_main();
  return 0;
}
 
