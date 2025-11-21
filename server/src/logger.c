#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "logger.h"

typedef struct
{
  char rules[1000];
  char player_name[5][20];
  char seat[10];
  char mibun[10];
  char result[10];
  char points[100];
  char player_cards[200];
  char play_log[1000];
  char change_log[1000];
  char ivent_log[1000];
} log_info;

char suitstring[] = "SHDCJKLMNJ";
char strongstring[] = "x3456789TJQKA2x";
char filename[100];
FILE *fp;
log_info logdata = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
int game_num = -1;
int flag_logger = 0;

int so_log_rc(int flag)
{
  flag_logger = flag;
}

int so_log_set_playername(char player_name[2][5][15])
{
  sprintf(logdata.player_name[0], "%s", player_name[0][0]);
  sprintf(logdata.player_name[1], "%s", player_name[0][1]);
  sprintf(logdata.player_name[2], "%s", player_name[0][2]);
  sprintf(logdata.player_name[3], "%s", player_name[0][3]);
  sprintf(logdata.player_name[4], "%s", player_name[0][4]);
}

int so_log_set_rules(char tmp_strings[15])
{
  sprintf(logdata.rules, "%s%%-----%%%s", logdata.rules, tmp_strings);
  return 0;
}

int so_log_set_seat(int player_card[5][8][15])
{
  int i;
  logdata.seat[0] = '\0';
  logdata.mibun[0] = '\0';
  int tmp_cal = 0;
  int tmp_mibun[5];
  for (i = 0; i < 5; i++)
  {
    tmp_cal += player_card[0][6][5 + i];
  }
  if (tmp_cal != 0)
  {
    for (i = 0; i < 5; i++)
    {
      tmp_mibun[i] = player_card[0][6][5 + i];
    }
  }
  else
  {
    for (i = 0; i < 5; i++)
    {
      tmp_mibun[i] = 2;
    }
  }
  for (i = 0; i < 5; i++)
  {
    sprintf(logdata.mibun, "%s%i", logdata.mibun, tmp_mibun[i]);
    sprintf(logdata.seat, "%s%i", logdata.seat, player_card[0][6][10 + i]);
  }
}

int so_log_set_card(int player_card[5][8][15])
{
  int i, j, k;

  logdata.player_cards[0] = '\0';
  for (k = 0; k < 5; k++)
  {
    for (i = 0; i < 5; i++)
    {
      for (j = 0; j < 15; j++)
      {
        if (player_card[k][i][j] != 0)
        {
          sprintf(logdata.player_cards, "%s%c%c", logdata.player_cards, suitstring[i + 5 * (player_card[k][i][j] == 2)], strongstring[j]);
        }
      }
    }
    sprintf(logdata.player_cards, "%s ", logdata.player_cards);
  }
}

int so_log_set_oneplay(int player, int tables[8][15])
{
  int i, j;
  char tmp[100];

  sprintf(tmp, "%i", player);

  for (i = 0; i < 5; i++)
  {
    for (j = 0; j < 15; j++)
    {
      if (tables[i][j] != 0)
      {
        sprintf(tmp, "%s%c%c", tmp, suitstring[i + 5 * (tables[i][j] == 2)], strongstring[j]);
      }
    }
  }
  sprintf(tmp, "%s%s", tmp, "EE");
  sprintf(logdata.play_log, "%s %s", logdata.play_log, tmp);
}

int so_log_set_onechange(int from_player, int to_player, int tables[8][15])
{
  int i, j;
  char tmp[100];

  sprintf(tmp, "%i%i", from_player, to_player);

  for (i = 0; i < 5; i++)
  {
    for (j = 0; j < 15; j++)
    {
      if (tables[i][j] != 0)
      {
        sprintf(tmp, "%s%c%c", tmp, suitstring[i + 5 * (tables[i][j] == 2)], strongstring[j]);
      }
    }
  }
  sprintf(tmp, "%s%s", tmp, "EE");
  sprintf(logdata.change_log, "%s %s", logdata.change_log, tmp);
}

int so_log_set_result(int result[5])
{
  sprintf(logdata.result, "%i%i%i%i%i", result[0], result[1], result[2], result[3], result[4]);
}

int so_log_set_points(unsigned int points[5][15000], int gamenum)
{
  sprintf(logdata.points, "%u %u %u %u %u", points[0][gamenum], points[1][gamenum], points[2][gamenum], points[3][gamenum], points[4][gamenum]);
}

int so_log_openfile()
{
  int i;
  time_t timedata;
  struct tm *tmdata;
  char hname[100];

  if (flag_logger)
  {
    timedata = (unsigned long)time(NULL);
    tmdata = localtime(&timedata);
    sprintf(filename, "logs/%04i%02i%02i%02i%02i%02i.log",
            tmdata->tm_year + 1900, tmdata->tm_mon + 1, tmdata->tm_mday,
            tmdata->tm_hour, tmdata->tm_min, tmdata->tm_sec);
    printf("%s\n", filename);
    fp = fopen(filename, "w");

    gethostname(hname, sizeof(hname));
    fprintf(fp, "%%-----%%hostname%% %s\n", hname);

    fprintf(fp, "%%-----%%TIME_START%% %04i/%02i/%02i %02i:%02i:%02i\n",
            tmdata->tm_year + 1900, tmdata->tm_mon + 1, tmdata->tm_mday,
            tmdata->tm_hour, tmdata->tm_min, tmdata->tm_sec);

    fprintf(fp, "%%-----%%palyer_name%%");
    for (i = 0; i < 5; i++)
    {
      fprintf(fp, " %s", logdata.player_name[i]);
    }
    fprintf(fp, "\n");

    fprintf(fp, "%s", logdata.rules);
  }
}

int so_log_closefile()
{
  time_t timedata;
  struct tm *tmdata;

  if (flag_logger)
  {
    timedata = (unsigned long)time(NULL);
    tmdata = localtime(&timedata);
    fprintf(fp, "%%-----%%TIME_END%% %04i/%02i/%02i %02i:%02i:%02i\n",
            tmdata->tm_year + 1900, tmdata->tm_mon + 1, tmdata->tm_mday,
            tmdata->tm_hour, tmdata->tm_min, tmdata->tm_sec);
    fprintf(fp, "%%-----%%TOTAL_POINT%% %s\n", logdata.points);
    fclose(fp);
  }
}

int so_log_print_all()
{
  if (flag_logger)
  {
    fprintf(fp, "%%%05i%%mibun%% %s\n", game_num, logdata.mibun);
    fprintf(fp, "%%%05i%%seat%% %s\n", game_num, logdata.seat);
    fprintf(fp, "%%%05i%%palyer_cards%% %s\n", game_num, logdata.player_cards);
    if (logdata.change_log[0] == '\0')
    {
      fprintf(fp, "%%%05i%%change_cards%% NULL\n", game_num, logdata);
    }
    else
    {
      fprintf(fp, "%%%05i%%change_cards%% %s\n", game_num, logdata.change_log);
    }
    fprintf(fp, "%%%05i%%play%% %s\n", game_num, logdata.play_log);
    fprintf(fp, "%%%05i%%result%% %s\n", game_num, logdata.result);
    fprintf(fp, "%%%05i%%points%% %s\n", game_num, logdata.points);
  }
}

int so_log_init_onegame()
{
  logdata.play_log[0] = '\0';
  logdata.change_log[0] = '\0';
  logdata.ivent_log[0] = '\0';
  game_num++;
}
