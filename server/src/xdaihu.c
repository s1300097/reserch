/*
  Grover's amplitude
 */
#include<sys/time.h>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<ctype.h>
#include"xdaihu.h"
#include"external.h"

char* one_to_yes(int flag);
XSetWindowAttributes attr;
c_window_info win_info;
g_window_info g_win_info;

int isint(char* a){
	/************************************************************/
	/* input  : A string                                        */
	/* return : If a input string are integer and               */
	/*        : the length of the string smaller than 5 then 1, */
	/*        : otherwize 0.                                    */
	/* destroy: none                                            */
	/************************************************************/
	int i;

	if(strlen(a)>=6){
		return 0;
	}
	for(i=0;i<strlen(a);i++){
		if(isdigit(a[i])==0){
			return 0;
		}
	}
	return 1;
}

char* num_to_str(char* str,int flag){
	/************************************************************/
	/* input  : str : strings                                   */
	/*        : flag : an integer which corresponding to str.   */
	/* return : an strings corresponding to str and flag        */
	/* destroy: none                                            */
	/************************************************************/
	if(strcmp(str,"WINDOW_TYPE")==0){	
		switch(flag){ 
			case 0:
				return "SMALL";
			case 1:
				return "BIG";
			case 2:
				return "CONSOLE";
			case 3:
				return "MIDDLE_EXTEND";
			case 4:
				return "MIDDLE";
			case 5:
				return "MIDDLE_EXTEND";
			case 6:
				return "MIDDLE_XGA";
			case 7:
				return "ALICE";
		}
	}
	return "ERROR";
}

char* one_to_yes(int flag){
	/************************************************************/
	/* input  : flag : integer                                  */
	/* return : If flag==1 the retunr YES otherwize NO          */
	/* destroy: none                                            */
	/************************************************************/
	if(flag){ 
		return "YES";
	}else{
		return "NO";
	}
}

void strupr2(char *dat){
	while(*dat != '\0'){ 
		//if(*dat >= 'a' && *dat <= 'z')
		// 	*dat = *dat - 0x20;
		*dat=toupper(*dat);
		dat++; 
	}
}

#ifndef X_DISPLAY_MISSING

unsigned long MyColor(Display *display, char *color){
	Colormap cmap;
	XColor c0, c1;

	cmap = DefaultColormap(display,0);

	XAllocNamedColor(display, cmap,color,&c1, &c0);
	return (c1.pixel);
}


void graph_test(unsigned int point[5][15000],int game_num,g_window_info *g_win_info, char player_name[5][15], int *flag){

	int i,j,k;
	char value[6][10];
	int font_offset=10;
	int y_font_offset=20;
	double average[5];

	for(i=0;i<=4;i++){
		if(g_win_info->y_border<=g_win_info->y_scale*point[i][game_num]){
			g_win_info->y_scale=g_win_info->y_scale/2;
			*flag=1;
		}
		if(g_win_info->x_border<=g_win_info->x_scale*game_num){
			g_win_info->x_scale=g_win_info->x_scale/2;
			*flag=1;
		}
	}
	if(game_num==1){
		*flag=1;
		//printf("debug: graph 1st\n");
	}

	if(*flag){
		sprintf(value[0],"%.0fpt \0",(g_win_info->y_border)/g_win_info->y_scale/2);
		sprintf(value[1],"%.0fpt \0",(g_win_info->y_border)/g_win_info->y_scale);
		sprintf(value[2],"%.0f\0",1000/g_win_info->x_scale);
		sprintf(value[3],"%.0f\0",750/g_win_info->x_scale);
		sprintf(value[4],"%.0f\0",500/g_win_info->x_scale);
		sprintf(value[5],"%.0f\0",250/g_win_info->x_scale);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->background); // clean pixmap
		XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);
		XFillRectangle(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 0,0, g_win_info->window_width, g_win_info->window_height);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground);
		XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);

		for(i=1;i<=4;i++){ // draw axis 
			XDrawLine(g_win_info->display, g_win_info->pixmap, g_win_info->gc, i*250,0,
				i*250, g_win_info->window_height-20);
			XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				1000-250*(i-1)-font_offset, g_win_info->window_height, value[i+1], strlen(value[i+1]));
		}
		for(i=1;i<=2;i++){
			XDrawLine(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 0,g_win_info->window_height-g_win_info->y_border*i/2,
				g_win_info->window_width, g_win_info->window_height-g_win_info->y_border*i/2);
			XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				0, g_win_info->window_height-g_win_info->y_border*i/2+font_offset, value[i-1], strlen(value[i-1]));

		}

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground); // write player number
		for(i=0;i<=4;i++){
			sprintf(value[0],":%5u \0",point[g_win_info->write_junban[i]][game_num]);
			XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				g_win_info->window_width-280, g_win_info->window_height-180+30*i+font_offset, player_name[g_win_info->write_junban[i]], strlen(player_name[g_win_info->write_junban[i]]));
			XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				g_win_info->window_width-155, g_win_info->window_height-180+30*i+font_offset, value[0], strlen(value[0]));
		}

		for(k=0;k<=4;k++){ // old graph
			XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->color[g_win_info->write_junban[k]]);
			for(j=1;j<=game_num;j++){ 
				XDrawLine(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
					g_win_info->x_scale*(j-1),g_win_info->window_height-g_win_info->y_scale*point[g_win_info->write_junban[k]][j-1],
					g_win_info->x_scale*j, g_win_info->window_height-g_win_info->y_scale*point[g_win_info->write_junban[k]][j]);
			}
			// player name's color
			XFillRectangle(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				g_win_info->window_width-300,g_win_info->window_height-195+30*k, 10, 30);
			XFillRectangle(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				g_win_info->window_width-300,g_win_info->window_height-195+30*(k+1)-2,
				150,2);
		}


	}


	for(i=0;i<=4;i++){ // draw line
		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->color[g_win_info->write_junban[i]]);
		XDrawLine(g_win_info->display, g_win_info->pixmap, g_win_info->gc, g_win_info->x_scale*(game_num-1),g_win_info->window_height-g_win_info->y_scale*point[g_win_info->write_junban[i]][game_num-1],
			g_win_info->x_scale*game_num, g_win_info->window_height-g_win_info->y_scale*point[g_win_info->write_junban[i]][game_num]);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground); // write point
		sprintf(value[0],":%5u \0",point[g_win_info->write_junban[i]][game_num]);
		XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
			g_win_info->window_width-155, g_win_info->window_height-180+30*i+font_offset, value[0], strlen(value[0]));
	}

	if((!(game_num%5) || *flag)&&(game_num!=1)){
		for(i=0;i<=4;i++){
			average[i]=((double)point[g_win_info->write_junban[i]][game_num]-(double)point[g_win_info->write_junban[i]][game_num-10])/(double)10;
			sprintf(value[0],"%5.1f\0",average[i]);
			XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
				//1030, g_win_info->window_height-180+30*i+font_offset, value[0], strlen(value[0]));
				g_win_info->window_width-70, g_win_info->window_height-180+30*i+font_offset, value[0], strlen(value[0]));
		}
	}
	*flag=0;
	XCopyArea(g_win_info->display, g_win_info->pixmap, g_win_info->window, g_win_info->gc,  // Copy pixmap to window
		0,0, g_win_info->window_width, g_win_info->window_height, 0,0); 
}

void graph_initialize(g_window_info *g_win_info){

	int i;
	char value[6][10];
	unsigned long  color[5];
	int font_offset=10;
	int y_font_offset=20;
       	int randomcolor[5];
	int tmp,tmp2;
	
	for(i=0;i<=4;i++){
	   g_win_info->write_junban[i]=i;
        }
	for(i=0;i<=4;i++){
	  randomcolor[i]=i;
        }

        if(g_win_info->FLAG_HIDDEN_NAME){
	  for(i=0;i<=3;i++){
            tmp=(int)(tn_rand_gen(1) *(5-i)-0.01) +i;
	    tmp2=g_win_info->write_junban[i];
	    g_win_info->write_junban[i]=g_win_info->write_junban[tmp];
	    g_win_info->write_junban[tmp]=tmp2;
          }
	  for(i=0;i<=3;i++){
            tmp=(int)(tn_rand_gen(1) *(5-i)-0.01) +i;
	    tmp2=randomcolor[i];
	    randomcolor[i]=randomcolor[tmp];
	    randomcolor[tmp]=tmp2;
          }
        }

	g_win_info->color[randomcolor[0]]=MyColor(g_win_info->display,"rgb:99/00/ff");
	g_win_info->color[randomcolor[1]]=MyColor(g_win_info->display,"red");
	g_win_info->color[randomcolor[2]]=MyColor(g_win_info->display,"rgb:00/00/ff");
	g_win_info->color[randomcolor[3]]=MyColor(g_win_info->display,"brown");
	g_win_info->color[randomcolor[4]]=MyColor(g_win_info->display,"rgb:00/75/00");



	sprintf(value[0],"%.0fpt \0",(g_win_info->y_scale*50)/g_win_info->y_scale);
	sprintf(value[1],"%.0fpt \0",(g_win_info->y_scale*100)/g_win_info->y_scale);
	sprintf(value[2],"%.0f\0",1000/g_win_info->x_scale);
	sprintf(value[3],"%.0f\0",750/g_win_info->x_scale);
	sprintf(value[4],"%.0f\0",500/g_win_info->x_scale);
	sprintf(value[5],"%.0f\0",250/g_win_info->x_scale);

	XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->background); // clean pixmap
	XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);
	XFillRectangle(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 0,0, g_win_info->window_width, g_win_info->window_height);

	XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground);
	XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);

	for(i=1;i<=4;i++){ // draw axis 
		XDrawLine(g_win_info->display, g_win_info->pixmap, g_win_info->gc, i*250,0,
			i*250, g_win_info->window_height-20);
		XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
			1000-250*(i-1)-font_offset, g_win_info->window_height, value[i+1], strlen(value[i+1]));
	}
	for(i=1;i<=2;i++){
		XDrawLine(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 0,g_win_info->window_height-(g_win_info->y_scale*50)*i,
			g_win_info->window_width, g_win_info->window_height-(g_win_info->y_scale*50)*i);
		XDrawImageString(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
			0, g_win_info->window_height-(g_win_info->y_scale*50)*i+font_offset, value[i-1], strlen(value[i-1]));
	}

	for(i=0;i<=4;i++){
		XSetForeground(g_win_info->display,g_win_info->gc,color[i]);
		XFillRectangle(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
			g_win_info->window_width-300, g_win_info->window_height-195+30*i, 10, 30);
		XFillRectangle(g_win_info->display, g_win_info->pixmap, g_win_info->gc, 
			g_win_info->window_width-300,g_win_info->window_height-195+30*(i+1)-2,
			120,2);
	}
	
	XCopyArea(g_win_info->display, g_win_info->pixmap, g_win_info->window, g_win_info->gc,  // Copy pixmap to window
		0,0, g_win_info->window_width, g_win_info->window_height, 0,0); 
}

void initialize_window(int stage_card[8][15], int old_stage_card[8][15], int players_card[5][8][15], c_window_info *win_info,int now_pass[5], char player_name[5][15], int sekijun[5]){

	int i,j,k;
	int xflag,yflag;
	int number_of_stage_card;
	int number_of_player_card;
	Colormap cmap;
	XColor color, exact;
	unsigned long green, blue, red;


	// initialize stage on piro

	if(stage_card[5][6]){  // kakumei 
		XCopyArea(win_info->display, win_info->screen4, win_info->screen0, win_info->gc, 0,0,win_info->window_width, win_info->window_height, 0,0); // delete old card on stage
	}else{
		XCopyArea(win_info->display, win_info->screen1, win_info->screen0, win_info->gc, 0,0,win_info->window_width, win_info->window_height, 0,0); // delete old card on stage
	}
	if(stage_card[5][7]){  // shibari
		XCopyArea(win_info->display,win_info->mibun_win,win_info->screen0,win_info->gc,0,win_info->card_height*7,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->card_width*10,win_info->on_stage_card_y_offset-(win_info->window_type==3)*10);
	}
	for(i=0;i<=4;i++){ // mibun and pass and turn
		XCopyArea(win_info->display,win_info->mibun_win,win_info->screen0,win_info->gc,0,win_info->card_height*6,win_info->card_width,win_info->card_height,5+win_info->card_width+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,players_card[0][5][3])+win_info->on_stage_player_y_offset); // turn	
		if(now_pass[i]==1){
			XCopyArea(win_info->display,win_info->mibun_win,win_info->screen0,win_info->gc,0,0,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->card_width+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,i)+win_info->on_stage_player_y_offset); //pass	
		}
	}

	if(win_info->window_type==3 || win_info->window_type==5){
		// initialize stage for old cards 
		number_of_stage_card=0;
		for(j=0;j<=14;j++){
			for(i=0;i<=4;i++){
				switch (old_stage_card[i][j]){
					case 1:
						xflag=(j-1)*win_info->card_width;
						yflag=i*win_info->card_height;
						XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->card_width*number_of_stage_card+10,win_info->on_stage_card_y_offset-20);
						number_of_stage_card++;
						break;
					case 2:
						xflag=0*win_info->card_width;
						yflag=4*win_info->card_height;
						XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->card_width*number_of_stage_card+10,win_info->on_stage_card_y_offset-20);
						number_of_stage_card++;
				}
			}
		}
	}

	if(win_info->window_type==5){
		for(i=0;i<=4;i++){
			XDrawImageString(win_info->display, win_info->screen0, win_info->gc,
				5, 220+115*get_seat(sekijun,i), player_name[i], strlen(player_name[i]));
		}
	}


	// initialize stage on piro
	number_of_stage_card=0;
	for(j=0;j<=14;j++){
		for(i=0;i<=4;i++){
			switch (stage_card[i][j]){
				case 1:
					xflag=(j-1)*win_info->card_width;
					yflag=i*win_info->card_height;
					XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->card_width*number_of_stage_card,win_info->on_stage_card_y_offset);
					number_of_stage_card++;
					break;
				case 2:
					xflag=0*win_info->card_width;
					yflag=4*win_info->card_height;
					XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->card_width*number_of_stage_card,win_info->on_stage_card_y_offset);
					number_of_stage_card++;
			}
		}
	}
	if(win_info->window_type==3 || win_info->window_type==5){
		if(number_of_stage_card==0){
			XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, 
				5*win_info->card_width, 4*win_info->card_height,
				4*win_info->card_width,win_info->card_height,
				win_info->on_stage_card_x_offset+win_info->card_width*4,win_info->on_stage_card_y_offset-20);
		}
	}

	// initializ cards on players cards
	for(k=0;k<=4;k++){
		number_of_player_card=0;
		for(j=1;j<=13;j++){
			for(i=0;i<=3;i++){
				if(players_card[k][i][j]==1){
					xflag=(j-1)*win_info->card_width;
					yflag=i*win_info->card_height;
					XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->card_offset_x+win_info->card_width*number_of_player_card+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,k)+win_info->on_stage_player_y_offset);
					number_of_player_card++;
				}
			}
		}
		if(players_card[k][4][1]==2){
			xflag=0*win_info->card_width;
			yflag=4*win_info->card_height;
			XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->card_offset_x+win_info->card_width*number_of_player_card+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,k)+win_info->on_stage_player_y_offset);
			number_of_player_card++;
		}
	}
	XCopyArea(win_info->display, win_info->screen0, win_info->screen3, win_info->gc, 0,0,win_info->window_width, win_info->window_height, 0,0); // delete old card on stage
	XFlush( win_info->display );

}

void wait_control(c_window_info *win_info, g_window_info *g_win_info, int *flag_wait_type, int *graph_mode, int *g_flag){

	XEvent event;
	int i;
	int press;
	struct timeval waitval;

	while( (XEventsQueued( win_info->display, QueuedAfterFlush ) !=0) ){ // flash XEvent que
		XNextEvent(win_info->display, &event);
//		XCheckMaskEvent(win_info->display,  ButtonPressMask,&event);
		switch(event.type){
			case ButtonPress :
				press=event.xbutton.y/50;
				if((0<=press) && (press <=3)){ 
					*flag_wait_type=press;
				}else if(press==4){
					*graph_mode=(*graph_mode+1)%2;
					*g_flag=1;
				}
				break;
		}
	}

	switch (*flag_wait_type){
		case 0:
			XRaiseWindow(win_info->display,win_info->screen3);
			XLowerWindow(win_info->display,g_win_info->window);
			break;
		case 1:
			waitval.tv_sec  = 1;
			waitval.tv_usec = 00;
			select(0,NULL,NULL,NULL,&waitval);
			XRaiseWindow(win_info->display,win_info->screen3);
			XLowerWindow(win_info->display,g_win_info->window);
			break;
		case 2: // wait press bottun
			while( XEventsQueued( win_info->display, QueuedAfterFlush ) != 0){ // flash XEvent que
				XNextEvent(win_info->display, &event);
			}
			XRaiseWindow(win_info->display,win_info->screen3);
			XLowerWindow(win_info->display,g_win_info->window);
			XMaskEvent(win_info->display,  ButtonPressMask,&event);
			*flag_wait_type=event.xbutton.y/50;
			break;
		case 3: // XRaiseWindow(win_info->display,g_win_info->window);
			XRaiseWindow(win_info->display,g_win_info->window);
			XLowerWindow(win_info->display,win_info->screen3);
			break;
	}
}

int stop_control(c_window_info *win_info){
	XEvent event;
	int i;

	XMaskEvent(win_info->display,  ButtonPressMask,&event);
        switch(event.type){
           case ButtonPress :
             return event.xbutton.y/50;
        }
}

void graph_test2(unsigned int point[5][15000],int game_num,c_window_info *win_info, g_window_info *g_win_info, char player_name[5][15], int *flag){
	int i,j,k;
	unsigned long  color[5];
	char value[6][10];
	int font_offset=10;
	int y_font_offset=20;
	double average[5];

	color[0]=MyColor(g_win_info->display,"rgb:99/00/ff");
	color[1]=MyColor(g_win_info->display,"red");
	color[2]=MyColor(g_win_info->display,"rgb:00/00/ff");
	color[3]=MyColor(g_win_info->display,"brown");
	color[4]=MyColor(g_win_info->display,"rgb:00/75/00");

	for(i=0;i<=4;i++){
		if(g_win_info->y_border<=g_win_info->y_scale2*point[i][game_num]){
			g_win_info->y_scale2=g_win_info->y_scale2/2;
			*flag=1;
		}
	}
	if(game_num==1){
		*flag=1;
	}

	if(*flag){
		sprintf(value[0],"%.0f\0",0*g_win_info->y_scale2);
		sprintf(value[1],"%.0f\0",(g_win_info->y_border)/g_win_info->y_scale2/2);
		sprintf(value[2],"%.0f\0",(g_win_info->y_border)/g_win_info->y_scale2);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->background); // clean pixmap
		XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);
		XFillRectangle(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 0,0, g_win_info->window_width, g_win_info->window_height);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground);
		XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);

		for(i=0;i<=2;i++){ // draw axis 
			XDrawLine(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 150+(g_win_info->y_border/2*i),0,
				150+(g_win_info->y_border/2*i), g_win_info->window_height-20);
			XDrawImageString(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 
				150+(g_win_info->y_border/2*i)-font_offset, g_win_info->window_height, value[i], strlen(value[i]));
		}

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground); // write player number
		for(i=0;i<=4;i++){
			sprintf(value[0],":%6u \0",point[i][game_num]);
			if(win_info->window_type==6){
				XDrawImageString(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 
					30, 125+130*i, player_name[i], strlen(player_name[i]));
				XCopyArea(g_win_info->display, win_info->screen6, g_win_info->pixmap2, g_win_info->gc,  // Copy pixmap to window
					650+125*i,710, 125, 115, 15,10+130*i); 
			}else if(win_info->window_type==7){
				XDrawImageString(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 
					30, 132+130*i, player_name[i], strlen(player_name[i]));
				XCopyArea(g_win_info->display, win_info->screen6, g_win_info->pixmap2, g_win_info->gc,  // Copy pixmap to window
					130*i,700, 130, 100, 15,10+130*i); 
			}
		}
	}

	for(i=0;i<=4;i++){ // draw line
		XSetForeground(g_win_info->display,g_win_info->gc,color[i]);
		//XFillRectangle(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 15,650+130*i, 130, 105 );
		XFillRectangle(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 150,20+130*i, g_win_info->y_scale2*point[i][game_num], 100 );
		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground); // write point
		sprintf(value[0],"%6u  \0",point[i][game_num]);
		XDrawImageString(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 
			g_win_info->window_width-100, 80+130*i, value[0], strlen(value[0]));
	}
	if((!(game_num%5) || *flag)&&(game_num!=1)){
		for(i=0;i<=4;i++){
			average[i]=((double)point[i][game_num]-(double)point[i][game_num-10])/(double)10;
			sprintf(value[0],"%5.1f\0",average[i]);
			XDrawImageString(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 
				g_win_info->window_width-100, 110+130*i, value[0], strlen(value[0]));
		}
	}
	*flag=0;
	XCopyArea(g_win_info->display, g_win_info->pixmap2, g_win_info->window, g_win_info->gc,  // Copy pixmap to window
		0,0, g_win_info->window_width, g_win_info->window_height, 0,0); 
}

void graph_initialize2(g_window_info *g_win_info){

	int i,j,k;
	int flag=1;
	unsigned long  color[5];
	char value[6][10];
	int font_offset=10;
	int y_font_offset=20;
	double average[5];

	color[0]=MyColor(g_win_info->display,"rgb:99/00/ff");
	color[1]=MyColor(g_win_info->display,"red");
	color[2]=MyColor(g_win_info->display,"rgb:00/00/ff");
	color[3]=MyColor(g_win_info->display,"brown");
	color[4]=MyColor(g_win_info->display,"rgb:00/75/00");

	for(i=0;i<=4;i++){
		if(g_win_info->y_border<=g_win_info->y_scale2*1){
			g_win_info->y_scale2=g_win_info->y_scale2/2;
			flag=1;
		}
	}

	if(flag){
		sprintf(value[0],"%.0f\0",0*g_win_info->y_scale2);
		sprintf(value[1],"%.0f\0",(g_win_info->y_border)/g_win_info->y_scale2/2);
		sprintf(value[2],"%.0f\0",(g_win_info->y_border)/g_win_info->y_scale2);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->background); // clean pixmap
		XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);
		XFillRectangle(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 0,0, g_win_info->window_width, g_win_info->window_height);

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground);
		XSetBackground(g_win_info->display,g_win_info->gc,g_win_info->background);

		for(i=0;i<=2;i++){ // draw axis 
			XDrawLine(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 150+(g_win_info->y_border/2*i),0,
				150+(g_win_info->y_border/2*i), g_win_info->window_height-20);
			XDrawImageString(g_win_info->display, g_win_info->pixmap2, g_win_info->gc, 
				150+(g_win_info->y_border/2*i)-font_offset, g_win_info->window_height, value[i], strlen(value[i]));
		}

		XSetForeground(g_win_info->display,g_win_info->gc,g_win_info->foreground); // write player number

	}
	XCopyArea(g_win_info->display, g_win_info->pixmap2, g_win_info->window, g_win_info->gc,  // Copy pixmap to window
		0,0, g_win_info->window_width, g_win_info->window_height, 0,0); 

}

void initialize_window2(int stage_card[8][15], int old_stage_card[8][15], int players_card[5][8][15], c_window_info *win_info,int now_pass[5], char player_name[5][15], int sekijun[5], int *accept_flag, int mibun[5]){

	int i,j,k;
	int xflag,yflag;
	int number_of_stage_card;
	int tmp,tmp2,loser;
	int number_of_player_card_x;
	int number_of_player_card_y;
	int rightside;
	int sa;
	int seat_x[5],seat_y[5];
	unsigned long  color[5];
	int analyze_result[4];
	int analyze_result2[4];
	int joker_flag=0;
	Colormap cmap;
	unsigned long green, blue, red;

	int win_mark_xoffset;
	int win_mark_yoffset;
	int win_mark_xoffset2;
	int win_mark_yoffset2;
	int bold=2;

	// set table
	seat_x[0]=0;seat_y[0]=0;
	seat_x[1]=1;seat_y[1]=0;
	seat_x[2]=2;seat_y[2]=0;
	seat_x[3]=0;seat_y[3]=1;
	seat_x[4]=2;seat_y[4]=1;
	color[0]=MyColor(win_info->display,"rgb:99/00/ff");
	color[1]=MyColor(win_info->display,"red");
	color[2]=MyColor(win_info->display,"rgb:00/00/ff");
	color[3]=MyColor(win_info->display,"brown");
	color[4]=MyColor(win_info->display,"rgb:00/75/00");

	// analyze_stages_card
	analyze_card(stage_card,analyze_result,0);

	// initialize state lump

	if(stage_card[5][6]){  // kakumei 
		XCopyArea(win_info->display, win_info->screen4, win_info->screen0, win_info->gc, 0,0,900,700, 0,0); // delete old card on stage
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,0,60,70,180,300+10+0,350+5);
	}else{
		XCopyArea(win_info->display, win_info->screen1, win_info->screen0, win_info->gc, 0,0,900,700, 0,0); // delete old card on stage
	}
	if(stage_card[5][0]){  //  card koukan
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,680,60,70*4,180,300+10+0,350+5);
	}
	if(analyze_result[3]==2){ //pair
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,140,60,70,180,300+10+70,350+5);
	}else if(analyze_result[3]==3){ // kaidan
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,70,60,70,180,300+10+140,350+5);
	}
	if(stage_card[5][7]){  // shibari
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,210,60,70,180,300+10+210,350+5);
	}
	XSetForeground(win_info->display,win_info->gc,color[players_card[0][5][3]]);
	XSetLineAttributes(win_info->display,win_info->gc,10,LineSolid,CapNotLast,JoinMiter);
	XDrawRectangle(win_info->display,win_info->screen0,win_info->gc,300*seat_x[get_seat(sekijun,players_card[0][5][3])], 350* seat_y[get_seat(sekijun,players_card[0][5][3])],300,350);
	XSetForeground(win_info->display,win_info->gc,win_info->foreground); // write point
	for(i=0;i<=4;i++){ // mibun and pass and turn
		if(now_pass[i]==1){
			XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,750,0,150,90, 130+300*seat_x[get_seat(sekijun,i)], 5+350*seat_y[get_seat(sekijun,i)]); //pass	
		}
	}

	// name and figure
	XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 160,95 , player_name[sekijun[0]], strlen(player_name[sekijun[0]]));
	XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 460,95 , player_name[sekijun[1]], strlen(player_name[sekijun[1]]));
	XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 760,95 , player_name[sekijun[2]], strlen(player_name[sekijun[2]]));
	XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 160,445 , player_name[sekijun[3]], strlen(player_name[sekijun[3]]));
	XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 760,445 , player_name[sekijun[4]], strlen(player_name[sekijun[4]]));
	XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,130*sekijun[0],700,130,105, 10,5); // turn	
	XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,130*sekijun[1],700,130,105, 310,5); // turn	
	XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,130*sekijun[2],700,130,105, 610,5); // turn	
	XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,130*sekijun[3],700,130,105, 10,355); // turn	
	XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,130*sekijun[4],700,130,105, 610,355); // turn	

	// initialize stage on piro
		// old card
	analyze_card(old_stage_card,analyze_result2,0);
	if(analyze_result2[1]==0){
		XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
			914,320,
			140,80,
			300+50,550);
	}else if(analyze_result2[1]<=3){
		rightside=300+50+(analyze_result2[1]-1)*70;
		sa=70;	
	}else{
		rightside=550-70;
		sa=(240-70)/(analyze_result2[1]-1);	
	};
	number_of_stage_card=0;
	for(j=14;j>=0;j--){
		for(i=4;i>=0;i--){
			switch (old_stage_card[i][j]){
				case 1:
					xflag=(j-1)*70+2;
					yflag=240+i*80 ;
					XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
						xflag,yflag,
						70,80,
						rightside-number_of_stage_card*sa,550);
					number_of_stage_card++;
					break;
				case 2:
					xflag=910+2;
					yflag=240;
					XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
						xflag,yflag,
						70,80,
						rightside-number_of_stage_card*sa,550);
					number_of_stage_card++;
			}
		}
	}

		// now card
	if(analyze_result[1]==0){
		XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
			914,320,
			140,80,
			310,600);
	}else if(analyze_result[1]<=3){
		rightside=300+10+(analyze_result[1]-1)*70;
		sa=70;	
	}else{
		rightside=550-70;
		sa=(240-70)/(analyze_result[1]-1);	
	};
	number_of_stage_card=0;
	for(j=14;j>=0;j--){
		for(i=4;i>=0;i--){
			switch (stage_card[i][j]){
				case 1:
					xflag=(j-1)*70+2;
					yflag=240+i*80 ;
					XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
						xflag,yflag,
						70,80,
						rightside-number_of_stage_card*sa,600);
					if((*accept_flag!=10)){
						XSetForeground(win_info->display,win_info->gc,color[players_card[0][5][3]]);
						XSetLineAttributes(win_info->display,win_info->gc,10,LineSolid,CapNotLast,JoinMiter);
						XDrawRectangle(win_info->display,win_info->screen0,win_info->gc,
							rightside-number_of_stage_card*sa+2,600+2,
							70-8,80-8);
						XSetForeground(win_info->display,win_info->gc,win_info->foreground); // write point
					}
					number_of_stage_card++;
					break;
				case 2:
					xflag=910+2;
					yflag=240;
					XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
						xflag,yflag,
						70,80,
						rightside-number_of_stage_card*sa,600);
					if((*accept_flag!=10)){
						XSetForeground(win_info->display,win_info->gc,color[players_card[0][5][3]]);
						XSetLineAttributes(win_info->display,win_info->gc,10,LineSolid,CapNotLast,JoinMiter);
						XDrawRectangle(win_info->display,win_info->screen0,win_info->gc,
							rightside-number_of_stage_card*sa+2,600+2,
							70-8,80-8);
						XSetForeground(win_info->display,win_info->gc,win_info->foreground); // write point
					}
					joker_flag=1;
					number_of_stage_card++;
			}
		}
	}

	if(stage_card[0][7]>=100){ // spe3 tokubetu
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,
			912,425,
			118,135,
			310,600+80-135);
		XSetForeground(win_info->display,win_info->gc,color[players_card[0][5][3]]);
		XSetLineAttributes(win_info->display,win_info->gc,10,LineSolid,CapNotLast,JoinMiter);
		XDrawRectangle(win_info->display,win_info->screen0,win_info->gc,
			312+2 ,600+80-135+2,
			118-8,135-8);
		XSetForeground(win_info->display,win_info->gc,win_info->foreground); // write point
	}

	// initializ cards on players cards
	for(k=0;k<=4;k++){
		number_of_player_card_x=0;
		number_of_player_card_y=0;
		for(j=1;j<=13;j++){
			for(i=0;i<=3;i++){
				if(players_card[k][i][j]==1){
					xflag=(j-1)*70+2;
					yflag=240+i*80 ;
					XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
						xflag,yflag,
						70,80,
						10+70*number_of_player_card_x + 300*seat_x[get_seat(sekijun,k)],
						105+80*number_of_player_card_y +350*seat_y[get_seat(sekijun,k)]);
					number_of_player_card_x++;
					if(number_of_player_card_x>=4){
						number_of_player_card_y++;
						number_of_player_card_x=0;
					}
				}
				if((*accept_flag==k)&&(stage_card[i][j]==1)){
					xflag=(j-1)*70+2;
					yflag=240+i*80 ;
					XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
						xflag,yflag,
						70,80,
						10+70*number_of_player_card_x + 300*seat_x[get_seat(sekijun,k)],
						105+80*number_of_player_card_y +350*seat_y[get_seat(sekijun,k)]);
					XSetForeground(win_info->display,win_info->gc,color[players_card[0][5][3]]);
					XSetLineAttributes(win_info->display,win_info->gc,10,LineSolid,CapNotLast,JoinMiter);
					XDrawRectangle(win_info->display,win_info->screen0,win_info->gc,
						10+70*number_of_player_card_x + 300*seat_x[get_seat(sekijun,k)]+4,
						105+80*number_of_player_card_y +350*seat_y[get_seat(sekijun,k)]+4,
						70-8,80-8);
					XSetForeground(win_info->display,win_info->gc,win_info->foreground); // write point
					number_of_player_card_x++;
					if(number_of_player_card_x>=4){
						number_of_player_card_y++;
						number_of_player_card_x=0;
					}
				}
			}
		}
		if(players_card[k][4][1]==2){
			xflag=910+2;
			yflag=240;
			XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
				xflag,yflag,
				70,80,
				10+70*number_of_player_card_x + 300*seat_x[get_seat(sekijun,k)],
				105+80*number_of_player_card_y +350*seat_y[get_seat(sekijun,k)]);
	//		XCopyArea(win_info->display, win_info->screen2, win_info->screen0, win_info->gc, xflag,yflag,win_info->card_width,win_info->card_height,win_info->card_offset_x+win_info->card_width*number_of_player_card+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,k)+win_info->on_stage_player_y_offset);
			number_of_player_card_x++;
		}
		if((*accept_flag==k) && (joker_flag==1)){
			xflag=910+2;
			yflag=240;
			XCopyArea(win_info->display, win_info->screen5, win_info->screen0, win_info->gc, 
				xflag,yflag,
				70,80,
				10+70*number_of_player_card_x + 300*seat_x[get_seat(sekijun,k)],
				105+80*number_of_player_card_y +350*seat_y[get_seat(sekijun,k)]);
			XSetForeground(win_info->display,win_info->gc,color[players_card[0][5][3]]);
			XSetLineAttributes(win_info->display,win_info->gc,10,LineSolid,CapNotLast,JoinMiter);
			XDrawRectangle(win_info->display,win_info->screen0,win_info->gc,
				10+70*number_of_player_card_x + 300*seat_x[get_seat(sekijun,k)]+4,
				105+80*number_of_player_card_y +350*seat_y[get_seat(sekijun,k)]+4,
				70-4,80-4);
			XSetForeground(win_info->display,win_info->gc,win_info->foreground); // write point
		}
	}
	if(stage_card[5][6]>=100){ //kakumei
		XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,
			0+400* ((*accept_flag)%3), 560+200* (*accept_flag>3),
			//400,560,
			400,200,
			250,200);
	}


	*accept_flag=10;

	// winner mark 
        if(win_info->window_type==6){
	  win_mark_xoffset=185;
	  win_mark_yoffset=330;
	  win_mark_xoffset2=195;
	  win_mark_yoffset2=250;
        } else if(win_info->window_type==7){
	  win_mark_xoffset=42;
	  win_mark_yoffset=338;
	  win_mark_xoffset2=0;
	  win_mark_yoffset2=300;
        }

	tmp2=0;
	for(i=0;i<=4;i++){
		if(count_card_num_r(players_card[i],&tmp)==0){
			XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,i*300,0,300,350,300*seat_x[get_seat(sekijun,i)],350*seat_y[get_seat(sekijun,i)]);
			tmp2++;
			XSetForeground(win_info->display,win_info->gc,win_info->background); // clean pixmap
        		if(win_info->window_type==6){
				XFillRectangle(win_info->display, win_info->screen0, win_info->gc,  
					300*seat_x[get_seat(sekijun,i)]+180,350*seat_y[get_seat(sekijun,i)]+300, 
					115, 45);
				XSetForeground(win_info->display,win_info->gc,win_info->foreground);
				XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset, 
					player_name[i], strlen(player_name[i]));
				XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,
					280+100*get_seat(mibun,i),60,
					100,50,
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset2,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset2);
        		} else if(win_info->window_type==7){
				//XFillRectangle(win_info->display, win_info->screen0, win_info->gc,  
			//		300*seat_x[get_seat(sekijun,i)]+95,350*seat_y[get_seat(sekijun,i)]+312, 
			//		105, 35);
			bold=3;
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset+bold,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset-bold, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset-bold,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset+bold, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset-bold,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset-bold, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset+bold,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset+bold, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset-bold, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset+bold, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset+bold,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset, 
					player_name[i], strlen(player_name[i]));
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset-bold,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset, 
					player_name[i], strlen(player_name[i]));
				XSetForeground(win_info->display,win_info->gc,win_info->foreground);
				XDrawString(win_info->display, win_info->screen0, win_info->gc, 
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset, 
					player_name[i], strlen(player_name[i]));

				XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,
					281+38*get_seat(mibun,i),112,
					38,50,
					300*seat_x[get_seat(sekijun,i)]+win_mark_xoffset2,350*seat_y[get_seat(sekijun,i)]+win_mark_yoffset2);
			}
		}else{
			loser=i;
		}
	}
	if(tmp2==4){
       		if(win_info->window_type==6){
	 		XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,loser*300,350,300,350,300*seat_x[get_seat(sekijun,loser)],350*seat_y[get_seat(sekijun,loser)]);
			XSetForeground(win_info->display,win_info->gc,win_info->background); // clean pixmap
			XFillRectangle(win_info->display, win_info->screen0, win_info->gc,  
				300*seat_x[get_seat(sekijun,loser)]+180,350*seat_y[get_seat(sekijun,loser)]+300, 
				115, 50);
			XSetForeground(win_info->display,win_info->gc,win_info->foreground);
			XDrawImageString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+185,350*seat_y[get_seat(sekijun,loser)]+330 , 
				player_name[loser], strlen(player_name[loser]));
			//XCopyArea(win_info->display,win_info->screen5,win_info->screen0,win_info->gc,
			//	280+100*4,60,
			//	100,50,
			//	300*seat_x[get_seat(sekijun,loser)]+195,350*seat_y[get_seat(sekijun,loser)]+250);
        	} else if(win_info->window_type==7){
	 		XCopyArea(win_info->display,win_info->screen6,win_info->screen0,win_info->gc,loser*300,350,300,350,300*seat_x[get_seat(sekijun,loser)],350*seat_y[get_seat(sekijun,loser)]);
	  		win_mark_xoffset=110;
			XSetForeground(win_info->display,win_info->gc,win_info->background); // clean pixmap
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset+bold,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset-bold, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset-bold,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset+bold, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset-bold,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset-bold, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset+bold,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset+bold, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset-bold, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset+bold, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset+bold,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset, 
				player_name[loser], strlen(player_name[loser]));
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset-bold,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset, 
				player_name[loser], strlen(player_name[loser]));
			XSetForeground(win_info->display,win_info->gc,win_info->foreground);
			XDrawString(win_info->display, win_info->screen0, win_info->gc, 
				300*seat_x[get_seat(sekijun,loser)]+win_mark_xoffset,350*seat_y[get_seat(sekijun,loser)]+win_mark_yoffset , 
				player_name[loser], strlen(player_name[loser]));
		}
	}

	XCopyArea(win_info->display, win_info->screen0, win_info->screen3, win_info->gc, 0,0,win_info->window_width, win_info->window_height, 0,0); // delete old card on stage
	XFlush( win_info->display );

}

/* ************************ */
/*                          */
/* ************************ */
int xinitial_setting(c_window_info *win_info, g_window_info *g_win_info, XSetWindowAttributes *attr, int WINDOW_TYPE, int GRAPH_WINDOW, int argc, char *argv[], char XPM_PATH[80]){
        int i;

	int screen;
	int root;
	Display *display;
	unsigned long foreground;
	unsigned long background;
        int err;
	GC gc;
        char XPM_CARD[100];
        char XPM_TEFUDA[100];
        char XPM_TEFUDA2[100];
        char XPM_MIBUN[100];
	
	if(WINDOW_TYPE==0 || WINDOW_TYPE==1 || WINDOW_TYPE==3 || WINDOW_TYPE==4 || WINDOW_TYPE==5 || WINDOW_TYPE==6  || WINDOW_TYPE==7||GRAPH_WINDOW==1 || GRAPH_WINDOW==2){
		display = XOpenDisplay(NULL);
		root  = DefaultRootWindow( display );
		screen = DefaultScreen( display );
		background = WhitePixel(display, 0); //set windows
		foreground = BlackPixel(display, 0);
		attr->backing_store = Always; //backing store
	}

	if(WINDOW_TYPE==0 || WINDOW_TYPE==1 || WINDOW_TYPE==3 || WINDOW_TYPE==4 || WINDOW_TYPE==5 ||WINDOW_TYPE==6  || WINDOW_TYPE==7){
		switch(WINDOW_TYPE){
			case 0: // small
				win_info->card_width=38;
				win_info->card_height=57;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->window_width=586;
				win_info->window_height=467;
				win_info->mibun_width=win_info->card_width;
				win_info->mibun_height=win_info->card_height*8;
				win_info->control_width=50;
				win_info->control_height=200;

				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=20;
				win_info->on_stage_player_size=78;
				win_info->on_stage_player_x_offset=0;
				win_info->on_stage_player_y_offset=99;
				strcpy(XPM_CARD,"card_s.xpm");
				strcpy(XPM_TEFUDA,"tefuda_s.xpm");
				strcpy(XPM_TEFUDA2,"tefuda2_s.xpm");
				strcpy(XPM_MIBUN,"mibun_s.xpm");
				break;
			case 1: // big
				win_info->card_width=77;
				win_info->card_height=114;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->window_width=1192;
				win_info->window_height=933;
				win_info->mibun_width=win_info->card_width;
				win_info->mibun_height=win_info->card_height*8;
				win_info->control_width=50;
				win_info->control_height=200;

				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=41;
				win_info->on_stage_player_size=155;
				win_info->on_stage_player_x_offset=0;
				win_info->on_stage_player_y_offset=198;
				strcpy(XPM_CARD,"card_b.xpm");
				strcpy(XPM_TEFUDA,"tefuda_b.xpm");
				strcpy(XPM_TEFUDA2,"tefuda2_b.xpm");
				strcpy(XPM_MIBUN,"mibun_b.xpm");
				break;
			case 4: // middle
				win_info->card_width=45;
				win_info->card_height=67;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->window_width=703;
				win_info->window_height=550;
				win_info->mibun_width=win_info->card_width;
				win_info->mibun_height=win_info->card_height*8;
				win_info->control_width=50;
				win_info->control_height=200;

				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=24;
				win_info->on_stage_player_size=24+win_info->card_height;
				win_info->on_stage_player_x_offset=0;
				win_info->on_stage_player_y_offset=136;
				strcpy(XPM_CARD,"card_m.xpm");
				strcpy(XPM_TEFUDA,"tefuda_m.xpm");
				strcpy(XPM_TEFUDA2,"tefuda2_m.xpm");
				strcpy(XPM_MIBUN,"mibun_m.xpm");
				break;
			case 3: // middle extend
				win_info->card_width=45;
				win_info->card_height=67;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->window_width=703;
				win_info->window_height=570;
				win_info->mibun_width=win_info->card_width;
				win_info->mibun_height=win_info->card_height*8;
				win_info->control_width=50;
				win_info->control_height=200;

				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=44;
				win_info->on_stage_player_size=24+win_info->card_height;
				win_info->on_stage_player_x_offset=0;
				win_info->on_stage_player_y_offset=136;
				strcpy(XPM_CARD,"card_me.xpm");
				strcpy(XPM_TEFUDA,"tefuda_me.xpm");
				strcpy(XPM_TEFUDA2,"tefuda2_me.xpm");
				strcpy(XPM_MIBUN,"mibun_me.xpm");
				break;
			case 5: // middle xga
				win_info->card_width=56;
				win_info->card_height=84;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->window_width=950;
				win_info->window_height=720;
				win_info->mibun_width=win_info->card_width;
				win_info->mibun_height=win_info->card_height*8;
				win_info->control_width=50;
				win_info->control_height=200;

				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=52;
				win_info->on_stage_player_size=32+win_info->card_height;
				win_info->on_stage_player_x_offset=100;
				win_info->on_stage_player_y_offset=168;
				sprintf(XPM_CARD,"%scard_l.xpm",XPM_PATH);
				sprintf(XPM_TEFUDA,"%stefuda_l.xpm",XPM_PATH);
				sprintf(XPM_TEFUDA2,"%stefuda2_l.xpm",XPM_PATH);
				sprintf(XPM_MIBUN,"%smibun_l.xpm",XPM_PATH);
				break;
			case 6: // NEW_NORMAL
				win_info->window_width=900;
				win_info->window_height=700;
				win_info->control_width=50;
				win_info->control_height=250;
				win_info->card_width=70;
				win_info->card_height=80;
				win_info->mibun_width=150;
				win_info->mibun_height=60;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=52;
				win_info->on_stage_player_size=32+win_info->card_height;
				win_info->on_stage_player_x_offset=100;
				win_info->on_stage_player_y_offset=168;
				sprintf(XPM_CARD,"%scard_newn.xpm",XPM_PATH);
				sprintf(XPM_MIBUN,"%smibun_newn.xpm",XPM_PATH);
				sprintf(XPM_TEFUDA,"%stefuda_newn.xpm",XPM_PATH);
				sprintf(XPM_TEFUDA2,"%stefuda2_newn.xpm",XPM_PATH);
				break;
			case 7: // NEW_ALICE
				win_info->window_width=900;
				win_info->window_height=700;
				win_info->control_width=50;
				win_info->control_height=250;
				win_info->card_width=70;
				win_info->card_height=80;
				win_info->mibun_width=150;
				win_info->mibun_height=60;
				win_info->card_offset_x=5+win_info->card_width*2;
				win_info->card_offset_y=30;
				win_info->on_stage_card_x_offset=5;
				win_info->on_stage_card_y_offset=52;
				win_info->on_stage_player_size=32+win_info->card_height;
				win_info->on_stage_player_x_offset=100;
				win_info->on_stage_player_y_offset=168;
				sprintf(XPM_CARD,"%scard_alice.xpm",XPM_PATH);
				sprintf(XPM_MIBUN,"%smibun_alice.xpm",XPM_PATH);
				sprintf(XPM_TEFUDA,"%stefuda_newn.xpm",XPM_PATH);
				sprintf(XPM_TEFUDA2,"%stefuda2_newn.xpm",XPM_PATH);
				break;
		}	
		
		win_info->root=root;
		win_info->screen=screen;
		(win_info)->display=display;
	
		win_info->window_type=WINDOW_TYPE;
		
		win_info->screen0 = XCreatePixmap(win_info->display, // tmp screen
			DefaultRootWindow(win_info->display),
			win_info->window_width, win_info->window_height,
			DefaultDepth(win_info->display,win_info->screen ));
		win_info->screen1 = XCreatePixmap(win_info->display, // screen1 : background bitmap in the case of normal
			DefaultRootWindow(win_info->display),
			win_info->window_width, win_info->window_height,
			DefaultDepth(win_info->display, win_info->screen ));
		win_info->screen2 = XCreatePixmap(win_info->display, // screen2 : card bitmap
			DefaultRootWindow(win_info->display),
			win_info->window_width, win_info->window_height,
			DefaultDepth(win_info->display, win_info->screen));
		win_info->screen3 = XCreateSimpleWindow(win_info->display, // display screen
			DefaultRootWindow(win_info->display),
			0, 0, win_info->window_width, win_info->window_height,
			0, 0, win_info->background);
		XSetStandardProperties(win_info->display, win_info->screen3, 
			"daihinmin", "daihugou",
			None, argv, argc, NULL);
		win_info->screen4 = XCreatePixmap(win_info->display, // screen4 : background bitmap in the case of kakumei
			DefaultRootWindow(win_info->display),
			win_info->window_width, win_info->window_height,
			DefaultDepth(win_info->display, win_info->screen ));
		if(WINDOW_TYPE==6 || WINDOW_TYPE==7){
			win_info->screen5 = XCreatePixmap(win_info->display, // tmp screen
				DefaultRootWindow(win_info->display),
				1201,960,
				DefaultDepth(win_info->display,win_info->screen ));
			win_info->screen6 = XCreatePixmap(win_info->display, // tmp screen
				DefaultRootWindow(win_info->display),
				1500,800,
				DefaultDepth(win_info->display,win_info->screen ));
		}
		win_info->control_win = XCreateSimpleWindow(win_info->display, // control_win : control window bitmsp
			DefaultRootWindow(win_info->display),
			0, 0, win_info->control_width, win_info->control_height,
			0, 0, win_info->background);
		XSetStandardProperties(win_info->display, win_info->control_win, 
			"control", "daihugou",
			None, argv, argc, NULL);
		win_info->mibun_win = XCreatePixmap(win_info->display, // mibun bitmap
			DefaultRootWindow(win_info->display),
			win_info->mibun_width, win_info->mibun_height,
			DefaultDepth(win_info->display, win_info->screen ));

		XSelectInput(win_info->display, win_info->control_win, ExposureMask | ButtonPressMask);

		XChangeWindowAttributes( win_info->display, win_info->screen3, CWBackingStore, attr );
		XChangeWindowAttributes( win_info->display, win_info->control_win, CWBackingStore, attr );

		XMapWindow(win_info->display, win_info->screen3);
		XMapWindow(win_info->display, win_info->control_win);
		
		win_info->background = WhitePixel(display, 0); //set windows
		win_info->foreground = BlackPixel(display, 0);

		win_info->gc = XCreateGC(win_info->display, win_info->screen0, 0, 0);
		win_info->font=XLoadFont(win_info->display,"-adobe-times-bold-i-normal--*-240-*-*-p-*-iso8859-1");
		XSetFont(win_info->display, win_info->gc, win_info->font);
		XSetForeground(win_info->display,win_info->gc,win_info->foreground);
		XSetBackground(win_info->display,win_info->gc,win_info->background);

		if(WINDOW_TYPE==0 || WINDOW_TYPE==1 || WINDOW_TYPE==3 || WINDOW_TYPE==4 || WINDOW_TYPE==5 ){
			err = XpmReadFileToPixmap(win_info->display, win_info->screen2,XPM_CARD,&win_info->pix,NULL,NULL);
			XCopyArea(win_info->display, win_info->pix, win_info->screen2, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen2 : card bitmap
			err = XpmReadFileToPixmap(win_info->display,win_info->screen1,XPM_TEFUDA,&win_info->pix,NULL,NULL);
			XCopyArea(win_info->display, win_info->pix, win_info->screen1, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen1 : background bitmap in the case of normal
			XCopyArea(win_info->display, win_info->pix, win_info->screen0, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen0 : tmp screen
			err = XpmReadFileToPixmap(win_info->display,win_info->screen1,XPM_TEFUDA2,&win_info->pix,NULL,NULL);  
			XCopyArea(win_info->display, win_info->pix, win_info->screen4, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen4 : background bitmap in the case of kakumei
			err = XpmReadFileToPixmap(win_info->display,win_info->mibun_win,XPM_MIBUN,&win_info->pix,NULL,NULL);
			XCopyArea(win_info->display, win_info->pix, win_info->mibun_win, win_info->gc, 0,0,win_info->mibun_width,win_info->mibun_height,0,0); // mibun   : mibun bitmap
		}
		if(WINDOW_TYPE==6 || WINDOW_TYPE==7){
			err = XpmReadFileToPixmap(win_info->display,win_info->screen1,XPM_TEFUDA,&win_info->pix,NULL,NULL);
			XCopyArea(win_info->display, win_info->pix, win_info->screen1, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen1 : background bitmap in the case of normal
			XCopyArea(win_info->display, win_info->pix, win_info->screen0, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen0 : tmp screen
			err = XpmReadFileToPixmap(win_info->display,win_info->screen1,XPM_TEFUDA2,&win_info->pix,NULL,NULL);  
			XCopyArea(win_info->display, win_info->pix, win_info->screen4, win_info->gc, 0,0,win_info->window_width,win_info->window_height,0,0); // screen4 : background bitmap in the case of kakumei

			err = XpmReadFileToPixmap(win_info->display, win_info->screen5,XPM_CARD,&win_info->pix,NULL,NULL);
			XCopyArea(win_info->display, win_info->pix, win_info->screen5, win_info->gc, 0,0,1201,960,0,0); // screen5 : card bitmap
			err = XpmReadFileToPixmap(win_info->display, win_info->screen6,XPM_MIBUN,&win_info->pix,NULL,NULL);
			XCopyArea(win_info->display, win_info->pix, win_info->screen6, win_info->gc, 0,0,1500,800,0,0); // screen6 : win
		}

		XCopyArea(win_info->display, win_info->screen0, win_info->screen3, win_info->gc, 0,0,win_info->window_width, win_info->window_height, 0,0); // delete old card on stage

                XSetForeground(win_info->display,win_info->gc,win_info->background); // clean pixmap
                XSetBackground(win_info->display,win_info->gc,win_info->background);
                XFillRectangle(win_info->display, win_info->control_win, win_info->gc, 0,0, win_info->control_width, win_info->control_height);
                XSetForeground(win_info->display,win_info->gc,win_info->foreground); // clean pixmap
		for(i=1;i<=3;i++){
			XDrawLine(win_info->display, win_info->control_win, win_info->gc, 
				0, i*50, win_info->control_width, i*50);
		}

		XFlush( display );

	} // fi (WINDOW_TYPE == 0 or 1 or 2 or 3 or 4 or 5)


	if(GRAPH_WINDOW==1 || GRAPH_WINDOW==2){
		switch(GRAPH_WINDOW){
			case 1:
				g_win_info->window_width=1100;
				g_win_info->window_height=410;
				g_win_info->x_scale=20;
				g_win_info->y_scale=4;
				g_win_info->x_border=g_win_info->x_scale*50;
				g_win_info->y_border=g_win_info->y_scale*100;
				break;
			case 2:
				g_win_info->window_width=900;
				g_win_info->window_height=700;
				g_win_info->x_scale=20;
				g_win_info->y_scale=6;
				g_win_info->x_border=g_win_info->x_scale*50/4*3;
				//g_win_info->y_border=g_win_info->y_scale*100;
				g_win_info->y_border=600;
				break;
		}
		g_win_info->y_scale2=1.5/2/2/2;

		g_win_info->background = WhitePixel(display, 0); //set windows
		g_win_info->foreground = BlackPixel(display, 0);
		g_win_info->root=root;
		g_win_info->screen=screen;
		(g_win_info)->display=display;

		g_win_info->window = XCreateSimpleWindow(g_win_info->display,
			DefaultRootWindow(g_win_info->display),
			0, 0, g_win_info->window_width, g_win_info->window_height,
			0, 0, g_win_info->background);
		g_win_info->pixmap = XCreatePixmap(g_win_info->display, 
			DefaultRootWindow(g_win_info->display),
			g_win_info->window_width, g_win_info->window_height,
			DefaultDepth(g_win_info->display,g_win_info->screen));
		g_win_info->pixmap2 = XCreatePixmap(g_win_info->display, 
			DefaultRootWindow(g_win_info->display),
			g_win_info->window_width, g_win_info->window_height,
			DefaultDepth(g_win_info->display,g_win_info->screen));
		XSetStandardProperties(g_win_info->display, g_win_info->window, 
			"control", "graph",
			None, argv, argc, NULL);
		XChangeWindowAttributes( g_win_info->display, g_win_info->window, CWBackingStore, attr );

		gc = XCreateGC(display, g_win_info->window, 0, 0);
		g_win_info->gc=gc;
		g_win_info->font=XLoadFont(g_win_info->display,"-adobe-times-bold-i-normal--*-240-*-*-p-*-iso8859-1");
		XSetFont(g_win_info->display, g_win_info->gc, g_win_info->font);


		XMapWindow(g_win_info->display,g_win_info->window);
		graph_initialize(g_win_info);
		graph_initialize2(g_win_info);
		XFlush( g_win_info->display );
	}//fi GRAPH_WINDOW
}

/* ************************ */
/*                          */
/* ************************ */
int x_rewrite_mibun(c_window_info *win_info, g_window_info *g_win_info, int WINDOW_TYPE, int now_number_of_games, int FLASH_MIBUN_NUMBER, int players_card[5][8][15], int sekijun[5], int mibun[5], int stage_card[8][15], int old_stage_card[8][15], int now_pass[5], char player_name[2][5][15], int *accept_flag, int *flag_wait_type,int *graph_mode,int *g_flag, int RULE_CHANGE){
   int i,j;
   int tmp;

		if(WINDOW_TYPE==0 || WINDOW_TYPE==1 || WINDOW_TYPE==3 || WINDOW_TYPE==4){
			for(i=0;i<=4;i++){ //  all user is heimin
				if(((now_number_of_games-1 )% FLASH_MIBUN_NUMBER)==0){
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen1,win_info->gc,0,win_info->card_height*3,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset,win_info->on_stage_player_size*i+win_info->on_stage_player_y_offset);	
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen4,win_info->gc,0,win_info->card_height*3,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset,win_info->on_stage_player_size*i+win_info->on_stage_player_y_offset);	
				}else{
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen1,win_info->gc,0,win_info->card_height*(players_card[0][6][5+i]+1),win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset,win_info->on_stage_player_size*i+win_info->on_stage_player_y_offset);	
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen4,win_info->gc,0,win_info->card_height*(players_card[0][6][5+i]+1),win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset,win_info->on_stage_player_size*i+win_info->on_stage_player_y_offset);	
				}
			}
		}
		if(WINDOW_TYPE==5){
			for(i=0;i<=4;i++){ //  all user is heimin
				if(((now_number_of_games-1 )% FLASH_MIBUN_NUMBER)==0){
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen1,win_info->gc,0,win_info->card_height*3,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*i+win_info->on_stage_player_y_offset);	
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen4,win_info->gc,0,win_info->card_height*3,win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*i+win_info->on_stage_player_y_offset);	
				}else{
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen1,win_info->gc,0,win_info->card_height*(players_card[0][6][5+i]+1),win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,i)+win_info->on_stage_player_y_offset);	
					XCopyArea(win_info->display,win_info->mibun_win,win_info->screen4,win_info->gc,0,win_info->card_height*(players_card[0][6][5+i]+1),win_info->card_width,win_info->card_height,win_info->on_stage_card_x_offset+win_info->on_stage_player_x_offset,win_info->on_stage_player_size*get_seat(sekijun,i)+win_info->on_stage_player_y_offset);	
				}
			}
		}
		if(WINDOW_TYPE==6 || WINDOW_TYPE==7){
				if(((now_number_of_games-1 )% FLASH_MIBUN_NUMBER)==0){
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,300,0,150,60,130,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,300,0,150,60,430,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,300,0,150,60,730,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,300,0,150,60,130,355);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,300,0,150,60,730,355);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,300,0,150,60,130,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,300,0,150,60,430,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,300,0,150,60,730,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,300,0,150,60,130,355);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,300,0,150,60,730,355);	

				}else{
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,(4-get_seat(mibun,sekijun[0]))*150,0,150,60,130,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,(4-get_seat(mibun,sekijun[1]))*150,0,150,60,430,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,(4-get_seat(mibun,sekijun[2]))*150,0,150,60,730,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,(4-get_seat(mibun,sekijun[3]))*150,0,150,60,130,355);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen1,win_info->gc,(4-get_seat(mibun,sekijun[4]))*150,0,150,60,730,355);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,(4-get_seat(mibun,sekijun[0]))*150,0,150,60,130,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,(4-get_seat(mibun,sekijun[1]))*150,0,150,60,430,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,(4-get_seat(mibun,sekijun[2]))*150,0,150,60,730,5);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,(4-get_seat(mibun,sekijun[3]))*150,0,150,60,130,355);	
					XCopyArea(win_info->display,win_info->screen5,win_info->screen4,win_info->gc,(4-get_seat(mibun,sekijun[4]))*150,0,150,60,730,355);	
				}
		}
	
		if(WINDOW_TYPE==0 || WINDOW_TYPE==1 || WINDOW_TYPE==3 || WINDOW_TYPE==4 || WINDOW_TYPE==5 || WINDOW_TYPE==6 || WINDOW_TYPE==7){
			if(*flag_wait_type!=3){
				if(WINDOW_TYPE==6 || WINDOW_TYPE==7){
					tmp=stage_card[5][0];
					if(((now_number_of_games % FLASH_MIBUN_NUMBER)!=1)&&(RULE_CHANGE==1)){ 
						stage_card[5][0]=1;
					}
					initialize_window2(stage_card, old_stage_card, players_card,win_info,now_pass, player_name[0], sekijun, accept_flag, mibun);
					stage_card[5][0]=tmp;
				}else{
					initialize_window(stage_card, old_stage_card, players_card,win_info,now_pass, player_name[0], sekijun);
				}
				if((now_number_of_games % FLASH_MIBUN_NUMBER)!=1){
					if(WINDOW_TYPE==5){
						XCopyArea(win_info->display,win_info->screen1,win_info->screen3,win_info->gc,
							win_info->on_stage_card_x_offset+4*win_info->card_width,win_info->on_stage_card_y_offset-20, 
							4*win_info->card_width,win_info->card_height,
							win_info->on_stage_card_x_offset+4*win_info->card_width,win_info->on_stage_card_y_offset-20);	
						XCopyArea(win_info->display,win_info->screen2,win_info->screen3,win_info->gc,
							win_info->card_width*2,win_info->card_height*4,3*win_info->card_width,win_info->card_height,
							win_info->on_stage_card_x_offset+4*win_info->card_width,win_info->on_stage_card_y_offset-20);	
					}
				}
			}
			wait_control(win_info, g_win_info, flag_wait_type, graph_mode, g_flag);
		}
}

#endif
