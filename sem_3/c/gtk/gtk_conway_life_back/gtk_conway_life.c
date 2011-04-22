#include <gtk/gtk.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <unistd.h>
#include "life.h"
#include "life.c"
#define SUCCESS 0
#define M_PI	3.1415

int fsize=80;
int step_mode, step_time=1000, steps=1;
short f_wid,f_heig;
int print_net=1;

unsigned long int turn, live_cells;
unsigned int birth, death;

int ref;//do refresh?
int is_auto_next_running;
int y_pos,x_pos;

void expose_field(GtkWidget *wid,GdkEventExpose *event,gpointer data);
static gboolean delete_event(GtkWidget * widget, GdkEvent * event, gpointer data);
static void destroy(GtkWidget * widget, gpointer data);
static void next_turn_button();
void toogle_step(GtkWidget *widget,GtkLabel **);
GdkPixbuf *create_pixbuf(const gchar * filename);
void change_time (GtkSpinButton *a_spinner);
void change_step (GtkSpinButton *a_spinner);
void change_fsize (GtkSpinButton *a_spinner,GtkWidget *field);
void print_cells(GtkWidget *wid, short f_step, int x, int y);
void open_file();
void show_about(GtkWidget *widget, gpointer data);
void up_act();
void down_act();
void right_act();
void left_act();

void toggle_net(GtkWidget *widget);

GtkWidget *fld, *bar, *t_label,*b_label,*d_label,*l_label;
int main( int argc, char *argv[]){
	y_pos=x_pos=fsize/2-8;
	GtkWidget *main_window, *vbox,*halign,*open;
	GtkWidget *menubar,*filemenu,*file,*quit,*sep;
	GtkWidget *aboutmenu,*about,*about_ent;
	GtkWidget *_settings,*settingsmenu,*tog_net;
	GtkWidget *field, *statusbar, * centerhbox, * rightvbox;
	GtkWidget *falign;
	GtkWidget *next_turn;
   	GtkWidget *step_ch;
	GtkWidget *hsep;

   	GtkWidget *time, *time_box;
   	GtkAdjustment *time_adj;
	
   	GtkWidget *step,*step_box;
   	GtkAdjustment *step_adj;

   	GtkWidget *fsizes,*fsize_box;
   	GtkAdjustment *fsizes_adj;

   	GtkWidget *quit_b;
   	GtkWidget *label;


  	gtk_init(&argc, &argv);
	GtkAccelGroup *accel_group = gtk_accel_group_new();

	//window---------------------------------------------------------
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_add_accel_group(GTK_WINDOW(main_window),accel_group);
	//gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 400);
	gtk_window_set_title(GTK_WINDOW(main_window), "Conway's game of life");
	gtk_container_set_border_width(GTK_CONTAINER(main_window), 2);
	g_signal_connect(G_OBJECT(main_window), "delete_event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(main_window), "destroy", G_CALLBACK(destroy), NULL);

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(main_window), vbox);
	//menu-----------------------------------------------------------
	menubar=gtk_menu_bar_new();
	//----file------
	filemenu=gtk_menu_new();

	file = gtk_menu_item_new_with_mnemonic("_File");
  	//new = gtk_image_menu_item_new_from_stock(GTK_STOCK_NEW, NULL);
  	open = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, accel_group);
  	sep = gtk_separator_menu_item_new();
  	quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
	//gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), sep);
	gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

	//----settings-----  
	settingsmenu=gtk_menu_new();
	_settings = gtk_menu_item_new_with_label("Settings");
  	tog_net = gtk_check_menu_item_new_with_label("View net");
  	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(tog_net), TRUE);
  	gtk_menu_item_set_submenu(GTK_MENU_ITEM(_settings), settingsmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(settingsmenu), tog_net);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), _settings);
	
	//----about------
	aboutmenu=gtk_menu_new();
	about = gtk_menu_item_new_with_mnemonic("_About");
  	about_ent = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(about), aboutmenu);
	gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), about_ent);
	gtk_menu_shell_append(GTK_MENU_SHELL(menubar), about);

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
	rightvbox = gtk_vbox_new(FALSE,2);
  	halign = gtk_alignment_new(1, 0, 0, 0);
  	gtk_container_add(GTK_CONTAINER(halign), rightvbox);
	gtk_container_add(GTK_CONTAINER(centerhbox), halign);
	
	//next turn
	next_turn = gtk_button_new_with_label("next turn");
	gtk_widget_set_size_request(next_turn, 120, 30);
	gtk_container_add(GTK_CONTAINER(rightvbox), next_turn);
	
	//quit button	
	quit_b = gtk_button_new_with_label("Quit"); 
 	g_signal_connect(G_OBJECT(quit_b), "clicked", G_CALLBACK(next_turn_button), NULL);
  	g_signal_connect_swapped(G_OBJECT(quit_b), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(main_window));
	gtk_container_add(GTK_CONTAINER(rightvbox), quit_b);

	//next step	
	step_ch	= gtk_check_button_new_with_mnemonic  ("do step");	
	gtk_container_add(GTK_CONTAINER(rightvbox), step_ch);
	gtk_toggle_button_set_active((GtkToggleButton *)step_ch,1);

	//time	
	time_box=gtk_hbox_new(FALSE,5);

	label=gtk_label_new("Time: (ms)");
	gtk_container_add(GTK_CONTAINER(time_box), label);

   	time_adj = (GtkAdjustment *) gtk_adjustment_new (step_time, 1, 30000, 1.0, 5.0, 0);
   	time = gtk_spin_button_new (time_adj, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(time_box), time);
	gtk_container_add(GTK_CONTAINER(rightvbox), time_box);
	
	//step
	step_box=gtk_hbox_new(FALSE,5);

	label=gtk_label_new("Step:");
	gtk_container_add(GTK_CONTAINER(step_box), label);

   	step_adj = (GtkAdjustment *) gtk_adjustment_new (steps, 0.0, 100.0, 1.0, 5.0, 0);
   	step = gtk_spin_button_new (step_adj, 1.0, 0);
	gtk_container_add(GTK_CONTAINER(step_box), step);
	gtk_container_add(GTK_CONTAINER(rightvbox), step_box);
	
	//field fsize
	fsize_box=gtk_hbox_new(FALSE,5);

	label=gtk_label_new("Field fsize:");
	gtk_container_add(GTK_CONTAINER(fsize_box), label);

   	fsizes_adj = (GtkAdjustment *) gtk_adjustment_new (fsize, 0.0, 100.0, 1.0, 5.0, 0);
   	fsizes = gtk_spin_button_new (fsizes_adj, 1.0, 0);
	gtk_spin_button_set_range ((GtkSpinButton *)fsizes,1,2000);
	gtk_container_add(GTK_CONTAINER(fsize_box), fsizes);
	gtk_container_add(GTK_CONTAINER(rightvbox), fsize_box);
	
	//Statistics
	//GtkWidget *stat_box=gtk_vbox_new(TRUE,5);

	//separator
	hsep=gtk_hseparator_new();
	//gtk_container_add(GTK_CONTAINER(stat_box), hsep);
	gtk_container_add(GTK_CONTAINER(rightvbox), hsep);

	GtkWidget *entry_box=gtk_hbox_new(TRUE,5);
	label=gtk_label_new("Turn:");
	gtk_container_add(GTK_CONTAINER(entry_box), label);
	t_label=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry_box), t_label);
	gtk_container_add(GTK_CONTAINER(rightvbox), entry_box);
	//gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	
	//separator
	hsep=gtk_hseparator_new();
	//gtk_container_add(GTK_CONTAINER(stat_box), hsep);
	gtk_container_add(GTK_CONTAINER(rightvbox), hsep);
	
	GtkWidget *entry1_box=gtk_hbox_new(TRUE,5);
	label=gtk_label_new("Live:");
	gtk_container_add(GTK_CONTAINER(entry1_box), label);
	l_label=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry1_box), l_label);
	//gtk_container_add(GTK_CONTAINER(stat_box), entry1_box);
	gtk_container_add(GTK_CONTAINER(rightvbox), entry1_box);

	//gtk_container_add(GTK_CONTAINER(rightvbox), stat_box);
	//separator
	hsep=gtk_hseparator_new();
	gtk_container_add(GTK_CONTAINER(rightvbox), hsep);
	//gtk_container_add(GTK_CONTAINER(stat_box), hsep);

	GtkWidget *entry2_box=gtk_hbox_new(TRUE,5);
	label=gtk_label_new("Dead:");
	gtk_container_add(GTK_CONTAINER(entry2_box), label);
	d_label=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry2_box), d_label);
	gtk_container_add(GTK_CONTAINER(rightvbox), entry2_box);
	//gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	
	GtkWidget * entry3_box=gtk_hbox_new(TRUE,5);
	label=gtk_label_new("Birth:");
	gtk_container_add(GTK_CONTAINER(entry3_box), label);
	b_label=gtk_label_new("0");
	gtk_container_add(GTK_CONTAINER(entry3_box), b_label);
	gtk_container_add(GTK_CONTAINER(rightvbox), entry3_box);
	//gtk_container_add(GTK_CONTAINER(stat_box), entry_box);
	//gtk_container_add(GTK_CONTAINER(rightvbox), stat_box);
	
	//separator
	hsep=gtk_hseparator_new();
//	gtk_container_add(GTK_CONTAINER(stat_box), hsep);
	//stat end------------------------------------
	
	//Moving--------------------------------------
	GtkWidget *up,*down,*right,*left;

	up = gtk_button_new_with_label("UP");
	gtk_widget_set_size_request(up, 120, 30);
	gtk_container_add(GTK_CONTAINER(rightvbox),up);

	down = gtk_button_new_with_label("DOWN");
	gtk_widget_set_size_request(down, 120, 30);
	gtk_container_add(GTK_CONTAINER(rightvbox),down);

	right = gtk_button_new_with_label("RIGHT");
	gtk_widget_set_size_request(right, 120, 30);
	gtk_container_add(GTK_CONTAINER(rightvbox),right);

	left = gtk_button_new_with_label("LEFT");
	gtk_widget_set_size_request(left, 120, 30);
	gtk_container_add(GTK_CONTAINER(rightvbox),left);

	gtk_widget_show (rightvbox);
	//right --------------------------------------
	
	statusbar = gtk_statusbar_new ();
	gtk_widget_show (statusbar);
	gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);
	
	//signals--------------------------------------
	fld=field;
	bar=statusbar;
  	g_signal_connect(G_OBJECT(step_ch), "clicked",G_CALLBACK(toogle_step), label);
  	g_signal_connect(G_OBJECT(time), "value_changed",G_CALLBACK(change_time), NULL);
  	g_signal_connect(G_OBJECT(step), "value_changed",G_CALLBACK(change_step), NULL);
  	g_signal_connect(G_OBJECT(fsizes), "value_changed",G_CALLBACK(change_fsize), field);
 	g_signal_connect_swapped(G_OBJECT(fsizes), "value_changed", G_CALLBACK(expose_field), field);
  	g_signal_connect(field, "expose-event",G_CALLBACK(expose_field), field);
  	g_signal_connect(open, "activate",G_CALLBACK(open_file), NULL);
	g_signal_connect(about_ent, "activate", G_CALLBACK(show_about), NULL); 
  	g_signal_connect(G_OBJECT(next_turn), "clicked", G_CALLBACK(next_turn_button), NULL);
 	g_signal_connect_swapped(G_OBJECT(next_turn), "clicked", G_CALLBACK(expose_field), field);
    	g_signal_connect(G_OBJECT(tog_net), "activate", G_CALLBACK(toggle_net), NULL);
 	g_signal_connect_swapped(G_OBJECT(tog_net), "activate", G_CALLBACK(expose_field), field);

    	g_signal_connect(G_OBJECT(up), "clicked", 	G_CALLBACK(up_act), NULL);
    	g_signal_connect(G_OBJECT(down), "clicked", 	G_CALLBACK(down_act), NULL);
    	g_signal_connect(G_OBJECT(right), "clicked", 	G_CALLBACK(right_act), NULL);
    	g_signal_connect(G_OBJECT(left), "clicked", 	G_CALLBACK(left_act), NULL);
	//acelerators--------------------------------------
	gtk_widget_add_accelerator(quit,"activate",accel_group,GDK_q,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(open,"activate",accel_group,GDK_q,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(up,	"clicked",accel_group,GDK_k,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(down,"clicked",accel_group,GDK_j,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(right,"clicked",accel_group,GDK_h,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(left, "clicked",accel_group,GDK_l,GDK_CONTROL_MASK,GTK_ACCEL_VISIBLE);
	gtk_widget_show_all(main_window);
	gtk_main();
	return 0;
}
void up_act(){
	g_print("up\n");
	y_pos--;
	expose_field(fld,NULL,NULL);
}
void down_act(){
	g_print("down\n");
	y_pos++;
	expose_field(fld,NULL,NULL);
}
void right_act(){
	g_print("right\n");
	x_pos--;
	expose_field(fld,NULL,NULL);
}
void left_act(){
	g_print("left\n");
	x_pos++;
	expose_field(fld,NULL,NULL);
}

void open_file(){
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
    		char *filename;
		del_all_live();
		turn=live_cells=0;
    		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    		g_print("filename = %s\n",filename);
		gchar *str;
		str = g_strdup_printf("Load file %s",filename);

		gtk_statusbar_push(GTK_STATUSBAR(bar),
		gtk_statusbar_get_context_id(GTK_STATUSBAR(bar), str), str);
		g_free(str);
		

		load_file(filename);
		ref=1;
		g_print("exp\n");
		expose_field(fld,NULL,NULL);
  	}
	gtk_widget_destroy (dialog);
}
void toggle_net(GtkWidget *widget) {
  	if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
    		print_net=1;
  	} else {
    		print_net=0;
  	}
}

gboolean delete_event(GtkWidget * widget, GdkEvent * event, gpointer data){
	g_print("Delete event occurred\n");
	return FALSE;
}

void destroy(GtkWidget * widget, gpointer data){
	g_print("Destroy signal was sent\n");
	
	gtk_main_quit();
}
void next_turn_action(){
	int i;
	for(i=0;i<steps;i++)
		next_turn();
	gchar s[20];
	sprintf(s,"%ld",turn);
	gtk_label_set_text((GtkLabel *)t_label,s);
	sprintf(s,"%ld",live_cells);
	gtk_label_set_text((GtkLabel *)l_label,s);
	sprintf(s,"%d",birth);
	gtk_label_set_text((GtkLabel *)b_label,s);
	sprintf(s,"%d",death);
	gtk_label_set_text((GtkLabel *)d_label,s);
}
void next_turn_button(){
	next_turn_action();
	step_mode=1;
}
void return_to_step(){
	step_mode=1;
}
gint auto_next_step(){
	next_turn_action();
	expose_field(fld,NULL,NULL);
	if(step_mode)
		return FALSE;
	return TRUE;
}
void toogle_step(GtkWidget *widget, GtkLabel **label){
  	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		step_mode=1;
  	} else {
		step_mode=0;
		if(!is_auto_next_running)
			g_timeout_add(step_time,auto_next_step,NULL);
  	}
 	g_print("now step_mode=%d\n",step_mode);
}

void change_time (GtkSpinButton *a_spinner) {
   	step_time=gtk_spin_button_get_value_as_int (a_spinner);
 	g_print("now time=%d\n",step_time);
}
void change_step (GtkSpinButton *a_spinner) {
   	steps=gtk_spin_button_get_value_as_int (a_spinner);
 	g_print("now steps=%d\n",steps);
}
void change_fsize (GtkSpinButton *a_spinner,GtkWidget *field) {
	fsize=gtk_spin_button_get_value_as_int (a_spinner);	
 	g_print("now fsize=%d\n",fsize);
	ref=1;
}
void expose_field(GtkWidget *wid,GdkEventExpose *event,gpointer data){
	int i,j;
	if(!ref)
		return;
	f_wid=wid->allocation.width;
	f_heig=wid->allocation.height;
	int f_step=(f_wid < f_heig ? f_wid/fsize : f_heig/fsize);

	cairo_t *cr;
	cr=gdk_cairo_create(wid->window);
	//gtk_widget_queue_draw(wid);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, 0, 0, f_wid, f_heig);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_fill(cr);

	//draw net
	if(print_net){
		cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
		cairo_set_line_width(cr,0.1);
		
		for(i=0,j=0;i<=fsize;i++){
			cairo_move_to(cr,j,0+2);
			cairo_line_to(cr,j,f_heig-2);
			cairo_move_to(cr,0+2,j);
			cairo_line_to(cr,f_wid-2,j);
			j+=f_step;
		}
		cairo_stroke(cr);
	}
	for(i=-fsize/2+x_pos;i<fsize/2+x_pos;i++)
		for(j=-fsize/2+y_pos;j<fsize/2+y_pos;j++)
			if(search_cell(i,j)==1)
				print_cells(wid,f_step,i+fsize/2-x_pos,j+fsize/2-y_pos);
	cairo_destroy(cr);
}
void print_cells(GtkWidget *wid, short f_step, int x, int y){
	float coordx=f_step*(x);
	float coordy=f_step*(y);
	//g_print("print cell x=%d, y=%d\n",x,y);
	cairo_t *cr;
	cr=gdk_cairo_create(wid->window);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 0.5);

	cairo_rectangle(cr, coordx, coordy, f_step, f_step);
	cairo_stroke_preserve(cr);
	cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
	cairo_fill(cr);
	cairo_destroy(cr);
}
void show_about(GtkWidget *widget, gpointer data){
//  	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("battery.png", NULL);
  	GtkWidget *dialog = gtk_about_dialog_new();
  	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Game of Life");
  	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.6"); 
  	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
      	"(copyleft) Dmitry Khodakov");
  	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
     	"This is simple conway's game of life emulator. Just a student's work.");
  	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
      	"http://dim-lib.110mb.com");
  	//gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  	//g_object_unref(pixbuf), pixbuf = NULL;
  	gtk_dialog_run(GTK_DIALOG (dialog));
  	gtk_widget_destroy(dialog);
}
