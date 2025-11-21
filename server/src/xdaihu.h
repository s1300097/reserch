/*
  XDAIHU.H
 */

#ifndef _XDAIHU_H_
#define _XDAIHU_H_


#ifndef HAVE_NO_CONFIG_H
#include "config.h"
#endif


#ifndef X_DISPLAY_MISSING

/* prototypes */
#include<X11/Intrinsic.h>
#include<X11/StringDefs.h>
#include<X11/Xaw/Label.h>
#include<X11/xpm.h>
#include<X11/Xutil.h>


typedef struct{
        int window_width;
        int window_height;
        Window window;
        Pixmap pixmap;
        Pixmap pixmap2;
        int root;
        int screen;
        Display *display;
        GC gc;
        unsigned long background;
        unsigned long foreground;

	Font font;
	double x_scale, y_scale, y_scale2;
	double x_border, y_border;
        int FLAG_HIDDEN_NAME;
        unsigned long color[5];
        unsigned long write_junban[5];
}g_window_info;

typedef struct{
	int card_width;
	int card_height;
	int card_offset_x;
	int card_offset_y;
	int window_width;
	int window_height;
	int mibun_width;
	int mibun_height;
	int control_width;
	int control_height;

	int on_stage_card_x_offset;
	int on_stage_card_y_offset;
	int on_stage_player_size;
	int on_stage_player_x_offset;
	int on_stage_player_y_offset;

	int window_type;
	Font font;

	Display *display;
	int root;
	int screen;
	unsigned long foreground;
	unsigned long background;
	Pixmap screen0;
	Pixmap screen1;
	Pixmap screen2;
	Window screen3;
	Pixmap screen4;
	Pixmap screen5;
	Pixmap screen6;
	Pixmap mibun_win;
	Pixmap pix;
	Window control_win;
	GC gc;
} c_window_info;


void wait_control(c_window_info *win_info, g_window_info *g_win_info, int *flag_wait_type, int *graph_mode, int *g_flag);
void initialize_window(int stage_card[8][15], int old_stage_card[8][15], int players_card[5][8][15], c_window_info *win_info,int now_pass[5],char player_name[5][15], int sekijun[5]);
void initialize_window2(int stage_card[8][15], int old_stage_card[8][15], int players_card[5][8][15], c_window_info *win_info,int now_pass[5],char player_name[5][15], int sekijun[5],int  *accept_flag , int mibun[5]);
void graph_initialize(g_window_info *g_win);
void graph_initialize2(g_window_info *g_win);
void graph_test(unsigned int point[5][15000], int game_num, g_window_info *g_win, char player_name[5][15], int *g_flag);
void graph_test2(unsigned int point[5][15000], int game_num, c_window_info *win_info, g_window_info *g_win, char player_name[5][15], int *g_flag);
int xinitial_setting(c_window_info *win_info, g_window_info *g_win_info, XSetWindowAttributes *attr, int WINDOW_TYPE, int GRAPH_WINDOW, int argc, char *argv[], char XPM_PATH[80]);
int x_rewrite_mibun(c_window_info *win_info, g_window_info *g_win_info, int WINDOW_TYPE, int now_number_of_games, int FLASH_MIBUN_NUMBER, int players_card[5][8][15], int sekijun[5], int mibun[5], int stage_card[8][15], int old_stage_card[8][15], int now_pass[5], char player_name[2][5][15], int *accept_flag, int *flag_wait_type,int *graph_mode,int *g_flag, int RULE_CHANGE);
int stop_control(c_window_info *win_info);

#endif 

extern char* one_to_yes(int flag);
extern XSetWindowAttributes attr;
extern c_window_info win_info;
extern g_window_info g_win_info;


#endif 




