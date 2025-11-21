/*
  EXTERNAL.H
 */

#ifndef _EXTERNAL_H_
#define _EXTERNAL_H_

/* prototypes */
#include"mt19937ar.h"

typedef struct{
        int revolution;
        int spe3;
}ivent_info;

char* num_to_str(char* str,int flag);
int isint(char* a);
void strupr2(char *dat);
int count_card_num(int tmp_card[8][15],int *card_num);
int count_card_num_r(int tmp_card[8][15],int *error);
void shuffle_card(int initial_person,int player_card[5][8][15], int RAND_TYPE, int sekijun[5]);
void print_player_card(int player_card[8][15]);
int analyze_card(int player_card[8][15], int status_of_submitted_card[4], int revers);
int check_include_card(int org_card[8][15], int subset_card[8][15]);
int drop_card_flag(int org_card[8][15], int subset_card[8][15]);
int check_special_card(int special_card, int status_of_submitted_card[4], int revers);
int search_card(int players_card[5][8][15], int num, int suit);
int compare_card_status(int stage_card[4], int now_card[4], int revers);
int marge_card(int target_card[8][15], int add_card[8][15]);
int trans_work_card(int source_card[8][15], int target_card[8][15], int work_card[8][15]);
int trans_strong_card(int source_card[8][15], int target_card[8][15], int trans_card[8][15], int number_of_card);
int search_strong_card(int player_card[8][15], int strong_card[8][15], int number_of_card);
void tn_rand_init(unsigned long seed, int type);
double tn_rand_gen(int type);
void tn_sekigae(int now_number_of_games, int sekijun[5], int RULE_SEKIGAE, int RULE_SEKIGAE_NUM, int RAND_TYPE);
int get_seat(int sekijun[5], int now_player);
#endif 

