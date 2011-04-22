#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define SUCCESS 0
#define M_PI	3.1415

unsigned int coord=20;
int x=1,y=2;
int step_mode, step_time, steps;
short f_wid,f_heig;

void expose_field(GtkWidget *wid,GdkEventExpose *event,gpointer data);
void print_cells(GtkWidget *wid,short f_step);
static gboolean delete_event(GtkWidget * widget, GdkEvent * event, gpointer data);
static void destroy(GtkWidget * widget, gpointer data);
static void button_clicked(GtkWidget * widget, gpointer data);
void toogle_step(GtkWidget *widget);
GdkPixbuf *create_pixbuf(const gchar * filename);
void change_time (GtkSpinButton *a_spinner);
void change_step (GtkSpinButton *a_spinner);
void change_coord (GtkSpinButton *a_spinner,GtkWidget *field);
void open_file(GtkWindow *parent_window);

int main( int argc, char *argv[]){
	GtkWidget *main_window, *vbox,*halign,*open;
	GtkWidget *menubar,*filemenu,*file,*new,*quit,*sep;
	GtkWidget *field, *statusbar, * centerhbox, * rightvbox;
	GtkWidget *falign;
	GtkWidget *next_turn;
   	GtkWidget *step_ch;
	GtkWidget *hsep;

   	GtkWidget *time, *time_box;
   	GtkAdjustment *time_adj;
	
   	GtkWidget *step,*step_box;
   	GtkAdjustment *step_adj;

   	GtkWidget *coords,*coord_box;
   	GtkAdjustment *coords_adj;

   	GtkWidget *quit_b;
   	GtkWidget *label[10];


  	gtk_init(&argc, &argv);
	GtkAccelGroup *accel_group = NULL;
	//window---------------------------------------------------------
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 400);
	gtk_window_set_title(GTK_WINDOW(main_window), "Conway's game of life");
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 2);
	g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(destroy), NULL);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), vbox);
	//menu-----------------------------------------------------------
	menubar=gtk_menu_bar_new();
	filemenu=gtk_menu_new();

	file = gtk_menu_item_new_with_mnemonic("_File");
  	new = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
  	open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
  	sep = gtk_separator_menu_item_new();
  	quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), sep);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 3);

  	g_signal_connect_swapped(G_OBJECT(main_window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
  	g_signal_connect(G_OBJECT(quit), "activate",G_CALLBACK(gtk_main_quit), NULL);

	//menu-----------------------------------------------------------
	centerhbox = gtk_hbox_new(FALSE,0);
	gtk_box_pack_start (GTK_BOX (vbox), centerhbox, TRUE, TRUE, 0);
	//gtk_container_add(GTK_CONTAINER(vbox), centerhbox);
	gtk_widget_show (centerhbox);
	
	//field----------------------------------------------------------
	falign = gtk_alignment_new(0.5, 0.5, 1, 1);
	field = gtk_drawing_area_new ();
	gtk_widget_show (field);
	gtk_container_add (GTK_CONTAINER (falign), field);
	gtk_widget_set_size_request(field,500,500);
	//gtk_box_pack_start (GTK_BOX (falign), field, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (centerhbox), falign, TRUE, TRUE, 0);
	//gtk_container_add (GTK_CONTAINER (centerhbox), falign);

	//right --------------------------------------
	hsep=gtk_hseparator_new();

	rightvbox = gtk_vbox_new(FALSE,0);
  	halign = gtk_alignment_new(1, 0, 0, 0);
  	gtk_container_add(GTK_CONTAINER(halign), rightvbox);
	gtk_container_add(GTK_CONTAINER(centerhbox), halign);
	
	//next turn
	next_turn = gtk_button_new_with_label("next turn");
	gtk_widget_set_size_request(next_turn, 120, 30);
  	g_signal_connect(G_OBJECT(next_turn), "clicked", G_CALLBACK(button_clicked), NULL);
	gtk_container_add(GTK_CONTAINER(rightvbox), next_turn);
	gtk_container_add(GTK_CONTAINER(rightvbox), hsep);
	
	//quit button	
	quit_b = gtk_button_new_with_label("Quit"); 
 	g_signal_connect(G_OBJECT(quit_b), "clicked", G_CALLBACK(button_clicked), NULL);
  	g_signal_connect_swapped(G_OBJECT(quit_b), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(main_window));
	gtk_container_add(GTK_CONTAINER(rightvbox), quit_b);

	//next step	
	step_ch	= gtk_check_button_new_with_mnemonic  ("do step");	
	gtk_container_add(GTK_CONTAINER(rightvbox), step_ch);

	//time	
	time_box=gtk_hbox_new(FALSE,5);

	label[0]=gtk_label_new("Time:");
	gtk_container_add(GTK_CONTAINER(time_box), label[0]);

   	time_adj = (GtkAdjustment *) gtk_adjustment_new (2.0, 0.0, 100.0, 1.0, 5.0, 0);
   	time = gtk_spin_button_new (time_adj, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(time_box), time);

	gtk_container_add(GTK_CONTAINER(rightvbox), time_box);
	
	//step
	step_box=gtk_hbox_new(FALSE,5);

	label[0]=gtk_label_new("Step:");
	gtk_container_add(GTK_CONTAINER(step_box), label[0]);

   	step_adj = (GtkAdjustment *) gtk_adjustment_new (1.0, 0.0, 100.0, 1.0, 5.0, 0);
   	step = gtk_spin_button_new (step_adj, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(step_box), step);

	gtk_container_add(GTK_CONTAINER(rightvbox), step_box);
	
	//field coord
	coord_box=gtk_hbox_new(FALSE,5);

	label[0]=gtk_label_new("Field coord:");
	gtk_container_add(GTK_CONTAINER(coord_box), label[0]);

   	coords_adj = (GtkAdjustment *) gtk_adjustment_new (coord, 0.0, 100.0, 1.0, 5.0, 0);
   	coords = gtk_spin_button_new (coords_adj, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(coord_box), coords);

	gtk_container_add(GTK_CONTAINER(rightvbox), coord_box);
	
	//Statistics
	GtkWidget *stat_box=gtk_vbox_new(TRUE,5);

	//separator
	hsep=gtk_hseparator_new();
	gtk_container_add(GTK_CONTAINER(stat_box), hsep);

	GtkWidget *entry_box=gtk_hbox_new(TRUE,5);
	label[0]=gtk_label_new("Turn:");
	gtk_container_add(GTK_CONTAINER(entry_box), label[0]);
	label[1]=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry_box), label[1]);
	gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	
	//separator
	hsep=gtk_hseparator_new();
	gtk_container_add(GTK_CONTAINER(stat_box), hsep);

	entry_box=gtk_hbox_new(TRUE,5);
	label[0]=gtk_label_new("Live:");
	gtk_container_add(GTK_CONTAINER(entry_box), label[0]);
	label[2]=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry_box), label[2]);
	gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	gtk_container_add(GTK_CONTAINER(rightvbox), stat_box);
	//separator
	hsep=gtk_hseparator_new();
	gtk_container_add(GTK_CONTAINER(stat_box), hsep);

	entry_box=gtk_hbox_new(TRUE,5);
	label[0]=gtk_label_new("Dead:");
	gtk_container_add(GTK_CONTAINER(entry_box), label[0]);
	label[3]=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry_box), label[3]);
	gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	
	entry_box=gtk_hbox_new(TRUE,5);
	label[0]=gtk_label_new("Birth:");
	gtk_container_add(GTK_CONTAINER(entry_box), label[0]);
	label[4]=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry_box), label[4]);
	gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	gtk_container_add(GTK_CONTAINER(rightvbox), stat_box);
	
	//separator
	hsep=gtk_hseparator_new();
	gtk_container_add(GTK_CONTAINER(stat_box), hsep);
	//stat end------------------------------------

	//right --------------------------------------

	gtk_widget_show (rightvbox);
	
	//signals--------------------------------------
  	g_signal_connect(G_OBJECT(step_ch), "clicked",G_CALLBACK(toogle_step), NULL);
  	g_signal_connect(G_OBJECT(time), "value_changed",G_CALLBACK(change_time), NULL);
  	g_signal_connect(G_OBJECT(step), "value_changed",G_CALLBACK(change_step), NULL);
  	g_signal_connect(G_OBJECT(coords), "value_changed",G_CALLBACK(change_coord), field);
  	g_signal_connect(field, "expose-event",G_CALLBACK(expose_field), field);
  	g_signal_connect(open, "activate",G_CALLBACK(open_file), main_window);

	statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar);
	gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);

	gtk_widget_show_all(main_window);

	gtk_main();

	return 0;
}
void open_file(GtkWindow *parent_window){
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",parent_window,
					GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
    		char *filename;
    		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    		g_print("filename = %s\n",filename);
    		g_free (filename);
  	}
	gtk_widget_destroy (dialog);
}
 
static gboolean delete_event(GtkWidget * widget, GdkEvent * event, gpointer data){
	g_print("Delete event occurred\n");
	return FALSE;
}

static void destroy(GtkWidget * widget, gpointer data){
	g_print("Destroy signal was sent\n");
	gtk_main_quit();
}

static void button_clicked(GtkWidget * widget, gpointer data){
 	g_print("Button was clicked!\n");
}
void toogle_step(GtkWidget *widget){
  	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		step_mode=1;
  	} else {
		step_mode=0;
  	}
 	g_print("now step_mode=%d\n",step_mode);
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

void change_time (GtkSpinButton *a_spinner) {
   	step_time=gtk_spin_button_get_value_as_int (a_spinner);
 	g_print("now time=%d\n",step_time);
}
void change_step (GtkSpinButton *a_spinner) {
   	steps=gtk_spin_button_get_value_as_int (a_spinner);
 	g_print("now steps=%d\n",steps);
}
void change_coord (GtkSpinButton *a_spinner,GtkWidget *field) {
   	coord=gtk_spin_button_get_value_as_int (a_spinner);
 	g_print("now coord=%d\n",coord);
	expose_field(field,NULL,NULL);
}

void expose_field(GtkWidget *wid,GdkEventExpose *event,gpointer data){
	short i,j;
	g_print("width=%d heigh=%d\n", wid->allocation.width,wid->allocation.height);
	f_wid=wid->allocation.width;
	f_heig=wid->allocation.height;
	short f_step=(f_wid < f_heig ? f_wid/coord : f_heig/coord);

	cairo_t *cr;
	cr=gdk_cairo_create(wid->window);
	//gtk_widget_queue_draw(wid);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, 0, 0, f_wid, f_heig);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr,0.5);
	//draw net
	for(i=0,j=0;i<=coord;i++){
		cairo_move_to(cr,j,0+2);
		cairo_line_to(cr,j,f_heig-2);
		cairo_move_to(cr,0+2,j);
		cairo_line_to(cr,f_wid-2,j);
		j+=f_step;
	}
	cairo_stroke(cr);
	print_cells(wid,f_step);
	cairo_destroy(cr);
}
void print_cells(GtkWidget *wid, short f_step){
	float coordx=f_step*(x+0.5+coord/2);
	float coordy=f_step*(y+0.5+coord/2);
	float rad=(f_step)/2 - 2;
	cairo_t *cr;
	cr=gdk_cairo_create(wid->window);
	//g_print("wid=%f,center=%f,%f, radius=%f",WID/field_width,coordx,coordy,rad);
	cairo_set_source_rgb(cr, 0, 0, 0);
 	cairo_arc(cr, coordx, coordy, rad, 0, 2 * M_PI);
 	cairo_stroke_preserve(cr);

 	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3); 
 	cairo_fill(cr);
	cairo_destroy(cr);
}
