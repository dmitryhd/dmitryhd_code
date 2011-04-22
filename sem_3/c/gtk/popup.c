#include <gtk/gtk.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFF 	1024
#define TIMEOUT	3*1000 //in ms
#define PROG_RC "/var/popup.rc"
#define FILEERR 1
#define SUCC 0

#define HSIZE 300
#define VSIZE 100
#define HPOS 900
#define VPOS 500

#define openf(f,name,mode) \
	if((f=fopen(name,mode))==NULL){\
		printf("can't open file\n");\
		exit(FILEERR);\
	}

int horpos=HPOS,vertpos=VPOS,horsize=HSIZE,vertsize=VSIZE,timeout=TIMEOUT;
FILE *f;

static gboolean delete_event(GtkWidget * widget, GdkEvent * event);
static void destroy(GtkWidget * widget);
static void button_clicked(GtkWidget * widget);
gboolean clicked(GtkWidget *widget,GdkEventButton *event,   gpointer user_data);
void print_menu(GtkWidget *window,GdkEventButton *event);
void move(GdkEvent *event);

int main(int argc, char ** argv){
	GtkWidget * window,* label,*button,*vbox,*falign;
	char string[BUFF];
	int i, horpos=HPOS, vertpos=VPOS,horsize=HSIZE,vertsize=VSIZE;
	for(i=0;i<BUFF;i++){
		string[i]=getchar();
		if(string[i]==EOF || string[i]==0)
			break;
	}
	string[i]=0;
	openf(f,PROG_RC,"r");
	if(fscanf(f,"%d %d %d %d %d",&horpos,&vertpos,&horsize,&vertsize,&timeout)!=5){
		fclose(f);
		openf(f,PROG_RC,"w");
		fprintf(f,"%d %d %d %d %d",horpos,vertpos=VPOS,horsize=HSIZE,vertsize=VSIZE,timeout);
		fclose(f);
	}
	fclose(f);
	
	char *opts="h:v:";
	int opt;	
	while((opt=getopt(argc,argv,opts))!=-1){
		switch(opt){
			case 'h':
				horpos=atoi(optarg);
				break;
			case 'v':
				vertpos=atoi(optarg);
				break;
		}
	}
	//g_print("string =%s\n",string);
	gtk_init(&argc, &argv);
  	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  	gtk_container_set_border_width(GTK_CONTAINER(window), 2);
  	gtk_window_set_default_size(GTK_WINDOW(window), horsize, vertsize);
	//gtk_widget_set_size_request(window,HORSIZE,VERTSIZE);

	gtk_window_move(GTK_WINDOW(window),horpos,vertpos);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK);

  	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
  	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
  	button = gtk_button_new_with_label("ok");
 	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(button_clicked), NULL);
  	g_signal_connect_swapped(G_OBJECT(button), "clicked", G_CALLBACK(gtk_widget_destroy), G_OBJECT(window));


	falign = gtk_alignment_new(0.5, 0.5, 1, 1);
	label=gtk_label_new(string);
	gtk_container_add (GTK_CONTAINER (falign), label);

	vbox = gtk_vbox_new(FALSE, 0);
  	gtk_container_add(GTK_CONTAINER(window), vbox);

  	gtk_container_add(GTK_CONTAINER(vbox), falign);
  	gtk_container_add(GTK_CONTAINER(vbox), button);
  	gtk_widget_add_events(GTK_WIDGET(window), GDK_CONFIGURE);

  	g_signal_connect(GTK_CONTAINER(window), "configure-event",G_CALLBACK(move), G_OBJECT(window));
  	g_signal_connect_swapped(GTK_CONTAINER(window), "button-press-event",G_CALLBACK(clicked), G_OBJECT(window));
  	gtk_widget_show_all(window);
	g_timeout_add(TIMEOUT,destroy,(gpointer *) window);

  	gtk_main();
  	return 0;
}
 
static gboolean delete_event(GtkWidget * widget, GdkEvent * event){
//  	g_print("Delete event occurred\n");
  	return FALSE;
}
 
static void destroy(GtkWidget * widget){
//  	g_print("Destroy signal was sent\n");
	openf(f,PROG_RC,"w");
	fprintf(f,"%d %d %d %d %d",horpos,vertpos,horsize,vertsize,timeout);
	fclose(f);
  	gtk_main_quit();
}
static void button_clicked(GtkWidget * widget){
//  	g_print("button clicked\n");
}

gboolean clicked(GtkWidget *widget, GdkEventButton *event,   gpointer user_data){
    	if (event->button == 3) {
		print_menu(widget,event);
    	}
    	return TRUE;
}

void print_menu(GtkWidget *window,GdkEventButton *event){
  	GtkWidget *dialog;
  	dialog = gtk_dialog_new_with_buttons ("settings",GTK_WINDOW(window),
                                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                                         "ok",
                                                         GTK_RESPONSE_OK,
							NULL);
	GtkWidget *a;
 	gtk_window_set_title(GTK_WINDOW(dialog), "Information");
  	gtk_dialog_run(GTK_DIALOG(dialog));
  	gtk_widget_destroy(dialog);
}
void move(GdkEvent *event){
	int x, y;
   	horpos += event->configure.x;
}
