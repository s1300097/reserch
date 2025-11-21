#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#include "xdaihu.h"
#include "external.h"
#include "tn_protocol.h"
#include "logger.h"

#include "statistics.h"

#ifndef HAVE_NO_CONFIG_H
#include "config.h"
#endif

int main(int argc, char *argv[])
{

	/* setting of valuances */

	int i, j, k;	// roop
	char *dat1;		// roop
	char *dat2;		// roop
	char dat3[256]; // roop
	char dat4[256]; // roop
	unsigned long seed;

	/* controle game */
	int flag_wait_type = 3;
	int graph_mode = 0;
	int g_flag = 0;
	int hidden_name = 0;
	int stop_flag = 0;
	int tmp, tmp2;

	/* for initialize_windows2 */
	int accept_flag = 0;

	/* for cards */
	int work_card[8][15] = {0};			   // i.e. submitted card etc...
	int trans_card[8][15] = {0};		   // i.e. submitted card etc...
	int stage_card[8][15] = {0};		   // i.e. submitted card etc...
	int old_stage_card[8][15] = {0};	   // i.e. submitted card etc...
	int players_card[5][8][15] = {0};	   // players_card
	int tmp_card[2][8][15] = {0};		   // players_card
	int number_of_card;					   // number_of_card temporal
	int status_of_submitted_card[4] = {0}; // now card
	int status_of_stages_card[4] = {-1};   // cards on the stage

	int sekijun[5];									// sekijun
	int mibun[5];									// cards on the stage
	int human[5] = {0, 0, 0, 0, 0};					// cards on the stage
	unsigned int point[5][15000] = {0, 0, 0, 0, 0}; // cards on the stage
	int error;										// error check

	int now_number_of_games; // save number of played games

	/* flags */
	int now_pass[5] = {0, 0, 0, 0, 0};
	int flash = 0; // flash
	int last_player;
	int now_muki;
	int now_player;				 // now playing player number
	int now_kakumei;			 // flag of kakumei
	int now_11back;				 // flag of 11 back
	int now_number_of_goal = 0;	 // number of person whose card is none
	int goal_flag[5] = {0};		 // flags of person
	int now_jocker_single = 0;	 // flag of jocker
	int now_shibari = 0;		 // flag of jocker
	int number_renzoku_pass = 0; // flags of passed person
	int flag_include_5 = 0;		 // flags of passed person
	ivent_info ivent_flag;

	/* for tcp */
	int sockfd, client_sockfd[5];
	int port_number = 42485;
	int protocol_version[5] = {20070, 20070, 20070, 20070, 20070};
	struct sockaddr_in wait_addr;	   // waiting port
	struct sockaddr_in client_addr[5]; // port for each clients
	int client_len[5];				   // waiting port
	fd_set target_fds;
	fd_set org_target_fds;
	struct timeval waitval;

	/* for log */
	FILE *fp, *fp2;
	char cfg_file[100] = "tndhms.cfg";
	int debug = 0; // debug_flag
	char tmp_log[100];

	/* for window */
	char XPM_PATH[80] = "\0";
	int WINDOW_TYPE = 0;
	int GRAPH_WINDOW = 0;
	int CONSOLE_TYPE = 0;
	char player_name[2][5][15] = {"", "", "", "", "",
								  "unknown", "unknown", "unknown", "unknown", "unknown"};

	/* for rule */
	int RULE_KAKUMEI = 1;
	int RULE_SHIBARI = 1;
	int RULE_KINSOKU = 0;
	int RULE_CHANGE = 1;
	int RULE_KAIDAN = 1;
	int RULE_5TOBI = 0;
	int RULE_6REVERS = 0;
	int RULE_8GIRI = 1;
	int RULE_11BACK = 0;
	int RULE_SEKIGAE = 1;
	int RULE_SEKIGAE_NUM = 3;
	int RAND_TYPE = 0;
	int GAME_NUMBER = 100;
	int FLASH_MIBUN_NUMBER = GAME_NUMBER;

	int FLAG_STATISTICS = 0;
	int FLAG_LOGGER = 0;

	/* for player statistics */
	int count_turn;
	struct playerStatistics ps[5];

	int sum_of_turn = 0;
	int game_count = 0;

	for (i = 0; i <= 4; i++)
	{
		bzero(&ps[i], sizeof(struct playerStatistics));
	}

	/************************************/
	/*  setting initial value	    */
	/************************************/

	signal(SIGPIPE, SIG_IGN);

	for (i = 0; i <= 4; i++)
	{
		sprintf(player_name[0][i], "Player %i\0", i + 1);
	}
	ivent_flag.revolution = 0;
	ivent_flag.spe3 = 0;

	/************************************/
	/*  setting for argument part1	    */
	/************************************/

	for (i = 1; i <= argc - 1; i++)
	{
		if (strcmp(argv[i], "-v") == 0)
		{ /* print version */
			printf("tndhms version 0.29\n");
			return 0;
		}
		else if ((strcmp(argv[i], "-h") == 0))
		{ /* print help message */
			printf("tndhms [-vh] [-p port_number] \n");
			printf("   -v version\n");
			printf("   -h help\n");
			printf("   -c config_file\n");
			printf("   -p port_number\n");
			return 0;
		}
		else if ((strcmp(argv[i], "-c") == 0))
		{ /* config file name */
			if (i + 1 <= argc - 1)
			{
				fp = fopen(argv[i + 1], "r");
				if ((strlen(argv[i + 1]) >= 90) || fp == NULL)
				{
					printf("Bad file name. \n");
					return 0;
				}
				else
				{
					strcpy(cfg_file, argv[i + 1]);
					printf("a use config_file is %s\n", cfg_file);
				}
				fclose(fp);
			}
			else
			{
				printf("Bad file name. \n");
				return 0;
			}
		}
	}

	/************************************/
	/*  read config file	            */
	/************************************/
	fp = fopen(cfg_file, "r");
	if (fp == NULL)
	{
		printf("I can open no config file.\n");
		return 0;
	};
	error = 0;
	k = 0;
	while (fgets(dat3, 256, fp) != NULL)
	{
		k++;
		if (dat3[0] == '#')
		{
			strcpy(dat3, " \0");
		}
		strcpy(dat4, dat3);
		dat2 = strtok(dat3, "#");
		strupr2(dat2);
		dat1 = strtok(dat2, " \r\t\n");
		if (dat1 != NULL)
		{
			if (strcmp(dat1, "WINDOW_TYPE") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "SMALL") == 0)
				{
					WINDOW_TYPE = 0;
				}
				else if (strcmp(dat1, "BIG") == 0)
				{
					WINDOW_TYPE = 1;
				}
				else if (strcmp(dat1, "CONSOLE") == 0)
				{
					WINDOW_TYPE = 2;
				}
				else if (strcmp(dat1, "MIDDLE") == 0)
				{
					WINDOW_TYPE = 4;
				}
				else if (strcmp(dat1, "MIDDLE_EXTEND") == 0)
				{
					WINDOW_TYPE = 3;
				}
				else if (strcmp(dat1, "MIDDLE_XGA") == 0)
				{
					WINDOW_TYPE = 5;
				}
				else if (strcmp(dat1, "NEW_NORMAL") == 0)
				{
					WINDOW_TYPE = 6;
				}
				else if (strcmp(dat1, "NEW_ALICE") == 0)
				{
					WINDOW_TYPE = 7;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "CONSOLE_TYPE") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "CONSOLE") == 0)
				{
					CONSOLE_TYPE = 2;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "GRAPH_WINDOW") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NONE") == 0 || strcmp(dat1, "NO") == 0)
				{
					GRAPH_WINDOW = 0;
				}
				else if (strcmp(dat1, "BIG") == 0)
				{
					GRAPH_WINDOW = 1;
				}
				else if (strcmp(dat1, "MIDDLE") == 0)
				{
					GRAPH_WINDOW = 2;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "GAME_NUMBER") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (isint(dat1))
				{
					GAME_NUMBER = atoi(dat1);
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "FLASH_MIBUN_NUMBER") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (isint(dat1))
				{
					FLASH_MIBUN_NUMBER = atoi(dat1);
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "PORT_NUMBER") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (isint(dat1))
				{
					port_number = atoi(dat1);
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_KAKUMEI") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_KAKUMEI = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_KAKUMEI = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_SHIBARI") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_SHIBARI = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_SHIBARI = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_KINSOKU") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_KINSOKU = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_KINSOKU = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_KAIDAN") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_KAIDAN = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_KAIDAN = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_5TOBI") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_5TOBI = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_5TOBI = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_6REVERS") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_6REVERS = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_6REVERS = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_8GIRI") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_8GIRI = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_8GIRI = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_11BACK") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_11BACK = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_11BACK = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_CHANGE") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_CHANGE = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_CHANGE = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_SEKIGAE") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					RULE_SEKIGAE = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					RULE_SEKIGAE = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "RULE_SEKIGAE_NUM") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (isint(dat1))
				{
					RULE_SEKIGAE_NUM = atoi(dat1);
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "FLAG_STATISTICS") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					FLAG_STATISTICS = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					FLAG_STATISTICS = 1;
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "FLAG_LOGGER") == 0)
			{
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					so_log_rc(0);
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					so_log_rc(1);
				}
				else
				{
					error = 1;
				}
			}
			else if (strcmp(dat1, "FLAG_HIDDEN_NAME") == 0)
			{
#ifndef X_DISPLAY_MISSING
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "NO") == 0)
				{
					g_win_info.FLAG_HIDDEN_NAME = 0;
				}
				else if (strcmp(dat1, "YES") == 0)
				{
					g_win_info.FLAG_HIDDEN_NAME = 1;
				}
				else
				{
					error = 1;
				}
				hidden_name = g_win_info.FLAG_HIDDEN_NAME;
#endif
			}
			else if (strcmp(dat1, "PLAYER1_NAME") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (dat1 != NULL)
				{
					snprintf(player_name[0][0], 9, "%s", dat1);
				}
			}
			else if (strcmp(dat1, "PLAYER2_NAME") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (dat1 != NULL)
				{
					snprintf(player_name[0][1], 9, "%s", dat1);
				}
			}
			else if (strcmp(dat1, "PLAYER3_NAME") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (dat1 != NULL)
				{
					snprintf(player_name[0][2], 9, "%s", dat1);
				}
			}
			else if (strcmp(dat1, "PLAYER4_NAME") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (dat1 != NULL)
				{
					snprintf(player_name[0][3], 9, "%s", dat1);
				}
			}
			else if (strcmp(dat1, "PLAYER5_NAME") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (dat1 != NULL)
				{
					snprintf(player_name[0][4], 9, "%s", dat1);
				}
			}
			else if (strcmp(dat1, "XPM_DIR_PATH") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (dat1 != NULL)
				{
					snprintf(XPM_PATH, 100, "%s/", dat1);
				}
			}
			else if (strcmp(dat1, "RAND_TYPE") == 0)
			{
				dat1 = strtok(dat4, " \r\t\n");
				dat1 = strtok(NULL, " \r\t\n");
				if (strcmp(dat1, "ISO_NORMAL") == 0)
				{
					RAND_TYPE = 0;
				}
				else if (strcmp(dat1, "MT") == 0)
				{
					RAND_TYPE = 1;
				}
			}

			if (error)
			{
				printf("I found error in line %i\n", k);
				error = 0;
			}
		}
	}
	fclose(fp);

	/************************************/
	/*  setting for argument part2	    */
	/************************************/

	for (i = 1; i <= argc - 1; i++)
	{
		if (strcmp(argv[i], "-p") == 0)
		{ /* port number  */
			if (i + 1 <= argc - 1)
			{
				if (isint(argv[i + 1]))
				{
					port_number = atoi(argv[i + 1]);
					printf("port number is %i\n", port_number);
				}
				else
				{
					printf("bad argument\n");
					return 0;
				}
			}
			else
			{
				printf("bad argument\n");
				return 0;
			}
		}
		else if (strcmp(argv[i], "-d") == 0)
		{ /* debug flag */
			debug = 1;
		}
	}

	/************************************/
	/*  print setting	            */
	/************************************/
	printf("WINDOW_TYPE\t=\t%s\n", num_to_str("WINDOW_TYPE", WINDOW_TYPE));
	printf("GRAPH_WINDOW\t=\t%s\n", one_to_yes(GRAPH_WINDOW));
	printf("RAND_TYPE\t=\t%i\n", RAND_TYPE);
	printf("RULE_KAKUMEI\t=\t%s\n", one_to_yes(RULE_KAKUMEI));
	printf("RULE_SHIBARI\t=\t%s\n", one_to_yes(RULE_SHIBARI));
	printf("RULE_KINSOKU\t=\t%s\n", one_to_yes(RULE_KINSOKU));
	printf("RULE_KAIDAN\t=\t%s\n", one_to_yes(RULE_KAIDAN));
	printf("RULE_CHANGE\t=\t%s\n", one_to_yes(RULE_CHANGE));
	printf("RULE_5TOBI\t=\t%s\n", one_to_yes(RULE_5TOBI));
	printf("RULE_6REVERS\t=\t%s\n", one_to_yes(RULE_6REVERS));
	printf("RULE_8GIRI\t=\t%s\n", one_to_yes(RULE_8GIRI));
	printf("RULE_11BACK\t=\t%s\n", one_to_yes(RULE_11BACK));
	printf("RULE_SEKIGAE\t=\t%s\n", one_to_yes(RULE_SEKIGAE));
	printf("RULE_SEKIGAE_NUM\t=\t%i\n", RULE_SEKIGAE_NUM);
	printf("GAME_NUMBER\t=\t%i\n", GAME_NUMBER);
	printf("FLASH_MIBUN_NUMBER\t=\t%i\n", FLASH_MIBUN_NUMBER);
	printf("GAME_PORT\t=\t%i\n", port_number);

	sprintf(tmp_log, "WINDOW_TYPE%% %s\n", num_to_str("WINDOW_TYPE", WINDOW_TYPE));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "GRAPH_WINDOW%% %s\n", one_to_yes(GRAPH_WINDOW));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RAND_TYPE%% %i\n", RAND_TYPE);
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_KAKUMEI%% %s\n", one_to_yes(RULE_KAKUMEI));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_SHIBARI%% %s\n", one_to_yes(RULE_SHIBARI));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_KINSOKU%% %s\n", one_to_yes(RULE_KINSOKU));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_KAIDAN%% %s\n", one_to_yes(RULE_KAIDAN));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_CHANGE%% %s\n", one_to_yes(RULE_CHANGE));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_5TOBI%% %s\n", one_to_yes(RULE_5TOBI));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_6REVERS%% %s\n", one_to_yes(RULE_6REVERS));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_8GIRI%% %s\n", one_to_yes(RULE_8GIRI));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_11BACK%% %s\n", one_to_yes(RULE_11BACK));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_SEKIGAE%% %s\n", one_to_yes(RULE_SEKIGAE));
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "RULE_SEKIGAE_NUM%% %i\n", RULE_SEKIGAE_NUM);
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "GAME_NUMBER%% %i\n", GAME_NUMBER);
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "FLASH_MIBUN_NUMBER%% %i\n", FLASH_MIBUN_NUMBER);
	so_log_set_rules(tmp_log);
	sprintf(tmp_log, "GAME_PORT%% %i\n", port_number);
	so_log_set_rules(tmp_log);

	/************************************/
	/*  initialize random seed	    */
	/************************************/

	seed = (unsigned long)time(NULL);
	// srand((unsigned)time(NULL));
	tn_rand_init(seed, RAND_TYPE);
	// tn_rand_init((unsigned long)3,RAND_TYPE);

	sprintf(tmp_log, "RANDOM_SEED%% %i\n", seed);
	so_log_set_rules(tmp_log);

	/************************************/
	/*  setting for X11   		    */
	/************************************/
#ifndef X_DISPLAY_MISSING
	xinitial_setting(&win_info, &g_win_info, &attr, WINDOW_TYPE, GRAPH_WINDOW, argc, argv, XPM_PATH);
#endif

	/************************************/
	/*  file open for log	       */
	/************************************/

	fp = fopen("debug.dat", "w");
	fp2 = fopen("debug2.dat", "w");

	/********************************/
	/* setting for client/server	*/
	/*  make soket for each client  */
	/********************************/
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("client: socket");
		exit(1);
	}
	bzero((char *)&wait_addr, sizeof(wait_addr));
	wait_addr.sin_family = PF_INET;
	wait_addr.sin_addr.s_addr = htons(INADDR_ANY);
	wait_addr.sin_port = htons(port_number);

	i = 1;
	j = sizeof(i);
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&i, j) < 0)
	{
		perror("setsockopt");
	}
	if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &i, j) < 0)
	{
		perror("setsockopt2");
	}

	if (bind(sockfd, (struct sockaddr *)&wait_addr, sizeof(wait_addr)) < 0)
	{
		perror("reader: bind");
		exit(1);
	}
	if (listen(sockfd, 1) < 0)
	{
		perror("reader: listen");
		close(sockfd);
		exit(1);
	}
	for (i = 0; i <= 4; i++)
	{
		printf("now waiting %i \n", i);
		client_len[i] = sizeof(client_addr[i]);
		if ((client_sockfd[i] = accept(sockfd, (struct sockaddr *)&client_addr[i], &client_len[i])) < 0)
		{
			perror("reader: accept");
			exit(1);
		};
		FD_ZERO(&org_target_fds);
		FD_SET(client_sockfd[i], &org_target_fds);
		memcpy(&target_fds, &org_target_fds, sizeof(org_target_fds));
		waitval.tv_sec = 2;
		waitval.tv_usec = 500;
		switch (select(50, &target_fds, NULL, NULL, &waitval))
		{
		case -1:
			printf("protocol_version: NONE\n");
			exit(1);
		case 0: /* time out */
			protocol_version[i] = 20060;
			printf("protocol_version: 2006a\n");
			break;
		default: /* connect from client */
			tn_card_read(client_sockfd[i], work_card, 20070);
			protocol_version[i] = work_card[0][0];
			printf("protocol_version: %d\n", protocol_version[i]);
			for (j = 0; j <= 8; j++)
			{
				player_name[0][i][j] = (char)work_card[1][j];
			}
			player_name[0][i][9] = '\0';
			printf("NAME: %s\n", player_name[0][i]);
			break;
		}
		tn_int_write(client_sockfd[i], i, protocol_version[i]);
		printf("accepted from %s \n", inet_ntoa(client_addr[i].sin_addr));
	}
	/* end of socket setting */

	/*********************/
	/* initialize values */
	/*********************/

	for (i = 0; i <= 4; i++)
	{
		mibun[i] = i;
	}
	for (i = 0; i <= 4; i++)
	{
		sekijun[i] = i;
	}

	// player statistics

	if (FLAG_STATISTICS)
	{
		for (i = 0; i <= 4; i++)
		{
			bzero(&ps[i], sizeof(struct playerStatistics));
		}
	}

	/**************/
	/* game start */
	/**************/
	so_log_set_playername(player_name);
	so_log_openfile();

	for (now_number_of_games = 1; now_number_of_games <= GAME_NUMBER; now_number_of_games++)
	{
		// shuffle all cards for 5 players
		bzero(stage_card, sizeof(stage_card));
		bzero(old_stage_card, sizeof(old_stage_card));
		bzero(players_card, sizeof(players_card));
		bzero(goal_flag, sizeof(goal_flag));
		so_log_init_onegame();

		if (RULE_SEKIGAE != 0)
		{ // decide sekigae
			tn_sekigae(now_number_of_games, sekijun, RULE_SEKIGAE, RULE_SEKIGAE_NUM, RAND_TYPE);
			//pirnt --> go to in a function
			// printf("sekigae done\n");
			// for (i = 0; i <= 4; i++)
			// {
			// 	printf("-> %i\n", sekijun[i]);
			// }
		}

		if (((now_number_of_games - 1) % FLASH_MIBUN_NUMBER) == 0)
		{ // decide first member for shuffle
			// shuffle_card(rand()%5,players_card);
			shuffle_card((int)(tn_rand_gen(RAND_TYPE) * 5), players_card, RAND_TYPE, sekijun);
		}
		else
		{
			shuffle_card(mibun[0], players_card, RAND_TYPE, sekijun);
		}
		for (i = 0; i <= 4; i++)
		{
			work_card[6][mibun[i] + 5] = i;
		}
		for (i = 0; i <= 4; i++)
		{ // initialize each table[5]
			bcopy(work_card[5], players_card[i][5], 2 * sizeof(work_card[5]));
		}

		/* ******************************************** */
		/* refresh X11 */
		/* ******************************************** */
#ifndef X_DISPLAY_MISSING
		x_rewrite_mibun(&win_info, &g_win_info, WINDOW_TYPE, now_number_of_games, FLASH_MIBUN_NUMBER, players_card, sekijun, mibun, stage_card, old_stage_card, now_pass, player_name, &accept_flag, &flag_wait_type, &graph_mode, &g_flag, RULE_CHANGE);
		if (debug)
		{
			printf("shuffle OK\n");
		} // DEBUG
#endif

		/************************************/
		/* distribute cards to each players */
		/************************************/
		// convert status to table
		work_card[5][0] = 0;
		work_card[5][1] = 0;
		work_card[5][2] = 0;
		work_card[5][3] = 6;
		work_card[5][4] = 0;
		work_card[5][5] = 0;
		work_card[5][6] = 0;
		work_card[5][7] = 0;
		work_card[6][0] = count_card_num_r(players_card[0], &error);
		work_card[6][1] = count_card_num_r(players_card[1], &error);
		work_card[6][2] = count_card_num_r(players_card[2], &error);
		work_card[6][3] = count_card_num_r(players_card[3], &error);
		work_card[6][4] = count_card_num_r(players_card[4], &error);
		if (((now_number_of_games - 1) % FLASH_MIBUN_NUMBER) == 0)
		{
			for (i = 0; i <= 4; i++)
			{
				work_card[6][i + 5] = 0;
			}
		}
		else
		{
			for (i = 0; i <= 4; i++)
			{
				work_card[6][mibun[i] + 5] = i;
			}
		}
		for (i = 10; i <= 14; i++)
		{
			work_card[6][i] = sekijun[i - 10];
		}
		for (i = 0; i <= 4; i++)
		{
			bcopy(work_card[5], players_card[i][5], 2 * sizeof(work_card[5]));
		}
		// distribute
		so_log_set_card(players_card);
		so_log_set_seat(players_card);

		// search strong card in hinmin and data setting
		if (((now_number_of_games % FLASH_MIBUN_NUMBER) != 1) && (RULE_CHANGE == 1))
		{
			for (i = 0; i <= 4; i++)
			{ // data setting for all player
				players_card[mibun[i]][5][0] = 1;
				players_card[mibun[i]][5][1] = 2 - i;
			}
			bcopy(players_card[mibun[3]], tmp_card[0], sizeof(players_card[mibun[3]]));
			bcopy(players_card[mibun[4]], tmp_card[1], sizeof(players_card[mibun[4]]));
			for (k = 2; k >= 1; k--)
			{ // search strong card
				trans_strong_card(players_card[mibun[2 + k]], players_card[mibun[2 - k]], trans_card, k);
				so_log_set_onechange(mibun[2 + k], mibun[2 - k], trans_card);
			}
		}
		else
		{ // no change because of it's a 1st game
			for (i = 0; i <= 4; i++)
			{ // data setting for all player
				players_card[mibun[i]][5][0] = 1;
				players_card[mibun[i]][5][1] = 0;
			}
			bcopy(players_card[mibun[3]], tmp_card[0], sizeof(players_card[mibun[3]]));
			bcopy(players_card[mibun[4]], tmp_card[1], sizeof(players_card[mibun[4]]));
		}
		if (debug)
		{
			printf("hinmin and daihinmin card change is done\n");
		}																							// DEBUG
		tn_card_write(client_sockfd[mibun[0]], players_card[mibun[0]], protocol_version[mibun[0]]); // tuuchi
		tn_card_write(client_sockfd[mibun[1]], players_card[mibun[1]], protocol_version[mibun[1]]);
		tn_card_write(client_sockfd[mibun[2]], players_card[mibun[2]], protocol_version[mibun[2]]);
		// tn_card_write(client_sockfd[mibun[3]],players_card[mibun[3]],protocol_version[mibun[3]]);
		// tn_card_write(client_sockfd[mibun[4]],players_card[mibun[4]],protocol_version[mibun[4]]);
		tn_card_write(client_sockfd[mibun[3]], tmp_card[0], protocol_version[mibun[3]]);
		tn_card_write(client_sockfd[mibun[4]], tmp_card[1], protocol_version[mibun[4]]);

		// change faise of daihugou start
		if (((now_number_of_games % FLASH_MIBUN_NUMBER) != 1) && (RULE_CHANGE == 1))
		{																				  // change card
			tn_card_read(client_sockfd[mibun[0]], work_card, protocol_version[mibun[0]]); // uketori
			error = 0;
			error = count_card_num(work_card, &number_of_card); // number check
			if ((check_include_card(players_card[mibun[0]], work_card) == 0) && (number_of_card == 2) && (error == 0))
			{
				if (debug)
				{
					printf("change card - OK \n");
				}
				trans_work_card(players_card[mibun[0]], players_card[mibun[4]], work_card);
				so_log_set_onechange(mibun[0], mibun[4], work_card);
			}
			else
			{
				if (debug)
				{
					printf("change card - fault \n");
				}
				trans_strong_card(players_card[mibun[0]], players_card[mibun[4]], trans_card, 2);
				so_log_set_onechange(mibun[0], mibun[4], trans_card);
			} // fi
		} // fi
		if (debug)
		{
			printf("change daihugou - OK\n");
		} // DEBUG

		// change faise of hugou start
		if (((now_number_of_games % FLASH_MIBUN_NUMBER) != 1) && (RULE_CHANGE == 1))
		{ // change card
			tn_card_read(client_sockfd[mibun[1]], work_card, protocol_version[mibun[1]]);
			error = 0;
			error = count_card_num(work_card, &number_of_card); // number check
			if ((check_include_card(players_card[mibun[1]], work_card) == 0) && (number_of_card == 1))
			{
				if (debug)
				{
					printf("change card - OK \n");
				}
				trans_work_card(players_card[mibun[1]], players_card[mibun[3]], work_card);
				so_log_set_onechange(mibun[1], mibun[3], work_card);
			}
			else
			{
				if (debug)
				{
					printf("change card - fault \n");
				}
				trans_strong_card(players_card[mibun[1]], players_card[mibun[3]], trans_card, 1);
				so_log_set_onechange(mibun[1], mibun[3], trans_card);
			} // fi
		} // fi
		if (debug)
		{
			printf("change hugou - OK\n");
		} // DEBUG
		if (debug)
		{
			printf("end of distribute\n");
		} // DEBUG
		/* end of distribute */

		/****************************/
		/* each players phase start */
		/****************************/

		status_of_submitted_card[0] = -1; // flash of stage
		status_of_stages_card[0] = -1;	  // flash of stage
		status_of_stages_card[1] = -1;
		status_of_stages_card[2] = -1;
		status_of_stages_card[3] = -1;
		bzero(stage_card, sizeof(stage_card));
		bzero(work_card, sizeof(work_card));
		now_number_of_goal = 0;
		now_player = search_card(players_card, 2, 1); // search a "dia 3"
		last_player = now_player;

		now_muki = 1;
		now_kakumei = 0;
		now_11back = 0;
		now_jocker_single = 0; // flag of jocker
		now_shibari = 0;	   // flag of jocker
		number_renzoku_pass = 0;

		flash = 0;							 // flash
		bzero(goal_flag, sizeof(goal_flag)); // flags of person
		bzero(now_pass, sizeof(now_pass));	 // flags of passed person

		// player statistics
		count_turn = 0;

		while (now_number_of_goal <= 3)
		{

			// convert status to table
			work_card[5][0] = 0;
			work_card[5][1] = 0;
			work_card[5][2] = 0;
			work_card[5][3] = now_player;
			work_card[5][4] = (status_of_stages_card[0] == -1);
			work_card[5][5] = now_11back;
			work_card[5][6] = now_kakumei;
			work_card[5][7] = now_shibari;
			work_card[6][0] = count_card_num_r(players_card[0], &error);
			work_card[6][1] = count_card_num_r(players_card[1], &error);
			work_card[6][2] = count_card_num_r(players_card[2], &error);
			work_card[6][3] = count_card_num_r(players_card[3], &error);
			work_card[6][4] = count_card_num_r(players_card[4], &error);
			if (((now_number_of_games - 1) % FLASH_MIBUN_NUMBER) == 0)
			{
				for (i = 0; i <= 4; i++)
				{
					work_card[6][i + 5] = 0;
				}
			}
			else
			{
				for (i = 0; i <= 4; i++)
				{
					work_card[6][mibun[i] + 5] = i;
				}
			}
			for (i = 10; i <= 14; i++)
			{
				work_card[6][i] = sekijun[i - 10];
			}
			for (i = 0; i <= 4; i++)
			{
				bcopy(work_card[5], players_card[i][5], 2 * sizeof(work_card[5]));
			}
			bcopy(work_card[5], stage_card[5], 2 * sizeof(work_card[5]));
			players_card[now_player][5][2] = 1;

#ifndef X_DISPLAY_MISSING
			if (WINDOW_TYPE == 0 || WINDOW_TYPE == 1 || WINDOW_TYPE == 3 || WINDOW_TYPE == 4 || WINDOW_TYPE == 5 || WINDOW_TYPE == 6 || WINDOW_TYPE == 7)
			{
				if (flag_wait_type != 3)
				{
					if (WINDOW_TYPE == 6 || WINDOW_TYPE == 7)
					{
						initialize_window2(stage_card, old_stage_card, players_card, &win_info, now_pass, player_name[0], sekijun, &accept_flag, mibun);
					}
					else
					{
						initialize_window(stage_card, old_stage_card, players_card, &win_info, now_pass, player_name[0], sekijun);
					}
				}
				wait_control(&win_info, &g_win_info, &flag_wait_type, &graph_mode, &g_flag);
			}
#endif

			if (debug)
			{
				printf("To send prepared datas for each player now\n");
			} // DEBUG
			// prepare datas for each player
			// atode
			tn_card_write(client_sockfd[0], players_card[0], protocol_version[0]);
			tn_card_write(client_sockfd[1], players_card[1], protocol_version[1]);
			tn_card_write(client_sockfd[2], players_card[2], protocol_version[2]);
			tn_card_write(client_sockfd[3], players_card[3], protocol_version[3]);
			tn_card_write(client_sockfd[4], players_card[4], protocol_version[4]);
			if (debug)
			{
				printf("To send prepared date is done. \n");
			} // DEBUG

			if (debug)
			{
				printf("Read from player %i\n", now_player);
			} // DEBUG
			tn_card_read(client_sockfd[now_player], work_card, protocol_version[now_player]);
			if (debug)
			{
				printf("accepted card is \n");
			} // DEBUG
			if (debug)
			{
				print_player_card(work_card);
			} // DEBUG
			if (debug)
			{
				printf("player card is \n");
			} // DEBUG
			if (debug)
			{
				print_player_card(players_card[now_player]);
			} // DEBUG

			error = 0; // error initialize

			so_log_set_oneplay(now_player, work_card);

			error |= check_include_card(players_card[now_player], work_card); // include check
			if (debug)
			{
				printf("error01 = %i: include check\n", error);
			} // DEBUG

			error |= analyze_card(work_card, status_of_submitted_card, 0); // analyze check.  the number of joker > 2 then error and etc
			if (debug)
			{
				printf("error02 = %i: analyze check \n", error);
			} // DEBUG

			error |= compare_card_status(status_of_stages_card, status_of_submitted_card, (now_kakumei + now_11back) % 2); // compare check
			if (debug)
			{
				printf("error03 = %i: compare check \n", error);
			} // DEBUG
			if (debug)
			{
				printf(
					"stage value    %i - number %i - suit %i - type %i \n",
					status_of_stages_card[0], status_of_stages_card[1], status_of_stages_card[2], status_of_stages_card[3]); // DEBUG
				printf(
					"submitte value %i - number %i - suit %i - type %i \n",
					status_of_submitted_card[0], status_of_submitted_card[1], status_of_submitted_card[2], status_of_submitted_card[3]); // DEBUG
			}

			if (FLAG_STATISTICS)
			{
				count_turn++;
				if ((error == 0) && (status_of_stages_card[0] == -1))
				{
					ps[now_player].getStage++;
					ps[now_player].cardStrength += status_of_submitted_card[0];
					switch (status_of_submitted_card[3])
					{
					case 1:
						break;
					case 2:
						ps[now_player].fukusuu++;
						break;
					case 3:
						ps[now_player].kaidan++;
						break;
					case 4:
						ps[now_player].jokerCnt++;
						ps[now_player].jokerTurnSum += count_turn;
						break;
					default:
						break;
					}
				}
				else if (error == 0)
				{
					switch (status_of_submitted_card[3])
					{
					case 1:
						break;
					case 2:
						break;
					case 3:
						break;
					case 4:
						ps[now_player].jokerCnt++;
						ps[now_player].jokerTurnSum += count_turn;
						break;
					default:
						break;
					}
				}
			}

			if (RULE_SHIBARI)
			{
				if ((error == 0) && (status_of_submitted_card[3] != 4))
				{
					if (now_shibari == 1)
					{
						if (status_of_submitted_card[2] == status_of_stages_card[2])
						{
						}
						else
						{
							error |= 1;
							if (debug)
							{
								printf("error06 = %i: shibari \n", error);
							} // DEBUG
						}
					}
					else
					{
						if (FLAG_STATISTICS)
							ps[now_player].shibariCnt++;
						if (status_of_submitted_card[2] == status_of_stages_card[2])
						{
							now_shibari++;
							if (FLAG_STATISTICS)
								ps[now_player].shibari++;
						}
						else
						{
							now_shibari = 0;
						}
					}
				}
			}

			if (now_jocker_single)
			{ // spe3
				if ((status_of_submitted_card[0] == 1) && (status_of_submitted_card[1] == 1) && (status_of_submitted_card[2] == 0x0001) && (players_card[now_player][0][1] == 1))
				{
					error = 0;
					now_jocker_single = 0;
					flash = 1;
					if (FLAG_STATISTICS)
						ps[now_player].spe3++;
					ivent_flag.spe3 = 1;
				}
				else
				{
					error |= 1;
				}
			}
			if (debug)
			{
				printf("error04 = %i: jocker \n", error);
			} // DEBUG

			if (error == 0)
			{
				switch (status_of_submitted_card[3])
				{
				case 0: // pass
					break;
				case 1: // single
					break;
				case 2: // pair
					break;
				case 3: // kaidan
					if (RULE_KAIDAN)
					{
						break;
					}
					else
					{
						error |= 1;
					}
					break;
				case 4: // jocker only
					now_jocker_single = 1;
					now_shibari = 0;
					break;
				}
			}
			if (debug)
			{
				printf("error05 = %i: kaidan \n", error);
			} // DEBUG

			if (error || (status_of_submitted_card[1] == 0))
			{
				if (error)
				{
					if (debug)
					{
						printf("error 01 - error = %i - number of card is %i\n", error, status_of_submitted_card[1]);
					}
					i = 8;
				}
				else
				{
					if (debug)
					{
						printf("OK 02\n");
					}
					i = 9;
				}
				j = 0;
				while (j <= 0)
				{
					j = tn_int_write(client_sockfd[now_player], i, protocol_version[now_player]);
					if (debug)
					{
						printf("ack roop a %i\n", j);
					}
				};
				if (debug)
				{
					printf("ack to player %i -- number %i\n", now_player, i);
				} // DEBUG
				now_pass[now_player] = 1;
				number_renzoku_pass++;
			}
			else
			{
				if (debug)
				{
					printf("OK 03\n");
				}
				bcopy(status_of_submitted_card, status_of_stages_card, sizeof(status_of_submitted_card));
				i = 9;
				j = 0;
				while (j <= 0)
				{
					j = tn_int_write(client_sockfd[now_player], i, protocol_version[now_player]);
					if (debug)
					{
						printf("ack roop b %i\n", j);
					}
				};
				if (debug)
				{
					printf("ack to player %i -- number %i\n", now_player, i);
				} // DEBUG
				bcopy(stage_card, old_stage_card, sizeof(work_card));
				bcopy(work_card, stage_card, sizeof(work_card));
				drop_card_flag(players_card[now_player], work_card);
				last_player = now_player;
				accept_flag = last_player;
				number_renzoku_pass = 0;

				count_card_num(players_card[now_player], &number_of_card);
				if ((number_of_card == 0) && (goal_flag[now_player] == 0))
				{
					j = 0;
					for (i = 0; i <= 4; i++)
					{
						count_card_num(players_card[i], &number_of_card);
						j = j + (number_of_card == 0);
					}
					goal_flag[now_player] = 1;
					mibun[j - 1] = now_player;
					now_number_of_goal = j;
				}

				if (now_jocker_single == 0)
				{
					if (RULE_KAKUMEI)
					{
						if ((status_of_submitted_card[3] == 2) && (status_of_submitted_card[1] >= 4))
						{
							now_kakumei = (now_kakumei + 1) % 2;
							if (FLAG_STATISTICS)
								ps[now_player].kakumei++;
							ivent_flag.revolution = 1;
						}
						if ((status_of_submitted_card[3] == 3) && (status_of_submitted_card[1] >= 5))
						{
							now_kakumei = (now_kakumei + 1) % 2;
							if (FLAG_STATISTICS)
								ps[now_player].kakumei++;
							ivent_flag.revolution = 1;
						}
					}
					if (RULE_8GIRI)
					{
						if (check_special_card(8 - 2, status_of_submitted_card, 0))
						{
							flash = 1;
							if (FLAG_STATISTICS)
								ps[now_player].eightGiriCnt++;
							if (FLAG_STATISTICS)
								ps[now_player].eightGiriTurnSum += count_turn;
						}
					}
					if (RULE_6REVERS)
					{
						if (check_special_card(6 - 2, status_of_submitted_card, 0))
						{
							if (now_muki == 1)
							{
								now_muki = 4;
							}
							else
							{
								now_muki = 1;
							}
						}
					}
					if (RULE_5TOBI)
					{
						if (check_special_card(5 - 2, status_of_submitted_card, 0))
						{
							flag_include_5 = 1;
						}
						else
						{
							flag_include_5 = 0;
						}
					}
					if (RULE_11BACK)
					{
						if (check_special_card(11 - 2, status_of_submitted_card, 0))
						{
							now_11back = (now_11back + 1) % 2;
						}
					}
				}
			}

			// convert status to table
			work_card[5][0] = 0;
			work_card[5][1] = 0;
			work_card[5][2] = 0;
			work_card[5][3] = now_player;
			work_card[5][4] = (status_of_stages_card[0] == -1);
			work_card[5][5] = now_11back;
			work_card[5][6] = now_kakumei;
			work_card[5][7] = now_shibari;
			work_card[6][0] = count_card_num_r(players_card[0], &error);
			work_card[6][1] = count_card_num_r(players_card[1], &error);
			work_card[6][2] = count_card_num_r(players_card[2], &error);
			work_card[6][3] = count_card_num_r(players_card[3], &error);
			work_card[6][4] = count_card_num_r(players_card[4], &error);
			if (((now_number_of_games - 1) % FLASH_MIBUN_NUMBER) == 0)
			{
				for (i = 0; i <= 4; i++)
				{
					work_card[6][i + 5] = 0;
				}
			}
			else
			{
				for (i = 0; i <= 4; i++)
				{
					work_card[6][mibun[i] + 5] = i;
				}
			}
			for (i = 10; i <= 14; i++)
			{
				work_card[6][i] = sekijun[i - 10];
			}
			for (i = 0; i <= 4; i++)
			{
				bcopy(work_card[5], stage_card[5], 2 * sizeof(work_card[5]));
			}
			bcopy(work_card[5], stage_card[5], 2 * sizeof(work_card[5]));
			players_card[now_player][5][2] = 1;

			// send information "stage cards" to clients.
			tn_card_write(client_sockfd[0], stage_card, protocol_version[0]);
			tn_card_write(client_sockfd[1], stage_card, protocol_version[1]);
			tn_card_write(client_sockfd[2], stage_card, protocol_version[2]);
			tn_card_write(client_sockfd[3], stage_card, protocol_version[3]);
			tn_card_write(client_sockfd[4], stage_card, protocol_version[4]);

			if (number_renzoku_pass >= 20)
			{
				if (debug)
				{
					printf("renzoku pass \n");
				}
				// srand((unsigned)time(NULL));
				while (now_number_of_goal <= 3)
				{
					j = (int)(tn_rand_gen(RAND_TYPE) * (5 - now_number_of_goal) + 1);
					i = 0;
					k = 0;
					while (i < j)
					{
						k++;
						if (goal_flag[k - 1] == 0)
						{
							i++;
						}
					}
					goal_flag[k - 1] = 1;
					mibun[now_number_of_goal] = k - 1;
					now_number_of_goal++;
				}
			}

			if ((now_pass[0] + now_pass[1] + now_pass[2] + now_pass[3] + now_pass[4]) >= (4 - now_number_of_goal))
			{
				flash = 1;
			}

#ifndef X_DISPLAY_MISSING
			if ((WINDOW_TYPE == 6 || WINDOW_TYPE == 7) && (flag_wait_type != 3))
			{
				if (ivent_flag.spe3 == 1)
				{
					if ((WINDOW_TYPE == 6 || WINDOW_TYPE == 7) && (flag_wait_type != 3))
					{
						stage_card[0][7] += 100;
						initialize_window2(stage_card, old_stage_card, players_card, &win_info, now_pass, player_name[0], sekijun, &accept_flag, mibun);
						stage_card[0][7] -= 100;
						wait_control(&win_info, &g_win_info, &flag_wait_type, &graph_mode, &g_flag);
					}
				}
				if (ivent_flag.revolution == 1)
				{
					stage_card[5][6] += 100;
					initialize_window2(stage_card, old_stage_card, players_card, &win_info, now_pass, player_name[0], sekijun, &accept_flag, mibun);
					stage_card[5][6] -= 100;
					wait_control(&win_info, &g_win_info, &flag_wait_type, &graph_mode, &g_flag);
				}
				initialize_window2(stage_card, old_stage_card, players_card, &win_info, now_pass, player_name[0], sekijun, &accept_flag, mibun);
				wait_control(&win_info, &g_win_info, &flag_wait_type, &graph_mode, &g_flag);
			}
#endif

			if (flash)
			{
				flash = 0;
				now_11back = 0;
				now_player = last_player;
				now_jocker_single = 0;
				now_shibari = 0; // flag of jocker
				bzero(now_pass, sizeof(now_pass));
				status_of_stages_card[0] = -1;
				status_of_stages_card[1] = -1;
				status_of_stages_card[2] = -1;
				status_of_stages_card[3] = -1;
				bcopy(stage_card, old_stage_card, sizeof(stage_card));
				bzero(stage_card, sizeof(stage_card));

				count_card_num(players_card[now_player], &number_of_card);
				while (number_of_card == 0)
				{
					if (debug)
					{
						printf("now_player search %i \n", now_player);
					} // DEBUG
					i = get_seat(sekijun, now_player);
					now_player = sekijun[(i + now_muki) % 5];
					count_card_num(players_card[now_player], &number_of_card);
				}
				last_player = now_player;
				if (debug)
				{
					printf("flashed 01==>next player is %i \n", now_player);
				}
			}
			else
			{
				if (debug)
				{
					printf("no flash==>%i %i %i \n", now_player, now_muki, (now_player + now_muki) % 5);
				}
				i = get_seat(sekijun, now_player);
				now_player = sekijun[(i + now_muki) % 5];

				count_card_num(players_card[now_player], &number_of_card);
				while (((number_of_card == 0) || (now_pass[now_player] == 1)))
				{
					if (debug)
					{
						printf("now_player search %i \n", now_player);
					} // DEBUG
					i = get_seat(sekijun, now_player);
					now_player = sekijun[(i + now_muki) % 5];
					count_card_num(players_card[now_player], &number_of_card);
				}
				if (RULE_5TOBI && flag_include_5)
				{
					count_card_num(players_card[now_player], &number_of_card);
					while (((number_of_card == 0) || (now_pass[now_player] == 1)))
					{
						if (debug)
						{
							printf("now_player search %i \n", now_player);
						} // DEBUG
						i = get_seat(sekijun, now_player);
						now_player = sekijun[(i + now_muki) % 5];
						count_card_num(players_card[now_player], &number_of_card);
					}
				}
			}
			if (debug)
			{
				printf("game is contineous = %i \n", now_number_of_goal);
			}

			if (now_number_of_goal == 4)
			{
				accept_flag = 10;
				if (now_number_of_games == GAME_NUMBER)
				{ // send a information "all game is overd" to clients.
					i = 2;
					for (j = 0; j <= 4; j++)
					{
						tn_int_write(client_sockfd[j], i, protocol_version[j]);
					}
				}
				else
				{ // send a information "One game is overd" to clients.
					i = 1;
					for (j = 0; j <= 4; j++)
					{
						tn_int_write(client_sockfd[j], i, protocol_version[j]);
					}
				}
			}
			else
			{
				i = 0;
				for (j = 0; j <= 4; j++)
				{
					tn_int_write(client_sockfd[j], i, protocol_version[j]);
				}
			}
			sum_of_turn++;
			ivent_flag.revolution = 0;
			ivent_flag.spe3 = 0;
		} // elihw of 1 game
		for (i = 0; i <= 4; i++)
		{
			if (goal_flag[i] == 0)
			{
				mibun[4] = i;
			}
		}
		if (debug)
		{
			printf("end of 1 game\n");
		}
		if (CONSOLE_TYPE == 2)
		{
			printf("================ game %i \n", now_number_of_games);
			printf("daihugou   %i\n", mibun[0]);
			printf("hugou      %i\n", mibun[1]);
			printf("heimin     %i\n", mibun[2]);
			printf("himmin     %i\n", mibun[3]);
			printf("daihinnmin %i\n", mibun[4]);
			printf("--------- total point\n");
		}
		for (i = 0; i <= 4; i++)
		{
			human[mibun[i]] = i;
			point[mibun[i]][now_number_of_games] = point[mibun[i]][now_number_of_games - 1] + (5 - i);
		}

		so_log_set_result(mibun);
		so_log_set_points(point, now_number_of_games);
		so_log_print_all();

		if (CONSOLE_TYPE == 2)
		{
			printf("%u %u %u %u %u\n", point[0][now_number_of_games], point[1][now_number_of_games], point[2][now_number_of_games], point[3][now_number_of_games], point[4][now_number_of_games]);
		}
		fprintf(fp2, "%i %i %i %i %i\n", human[0], human[1], human[2], human[3], human[4]);
		fprintf(fp, "%u %u %u %u %u\n", point[0][now_number_of_games], point[1][now_number_of_games], point[2][now_number_of_games], point[3][now_number_of_games], point[4][now_number_of_games]);

#ifndef X_DISPLAY_MISSING
		if (GRAPH_WINDOW == 1 || GRAPH_WINDOW == 2)
		{
			switch (graph_mode)
			{
			case 0:
				graph_test(point, now_number_of_games, &g_win_info, player_name[hidden_name], &g_flag);
				break;
			case 1:
				graph_test2(point, now_number_of_games, &win_info, &g_win_info, player_name[hidden_name], &g_flag);
				break;
			}
			XFlush(g_win_info.display);
		} // fi GRAPH_WINDOW
#endif
		game_count++;

	} // rof now_number_of_games

	// player statistics
	if (FLAG_STATISTICS)
	{
		printf("Player Statistics\n");
		for (i = 0; i <= 4; i++)
		{
			printf("%s\n", player_name[0][i]);
			if (ps[i].getStage != 0)
			{
				printf("average of card strength:\t\t%f\n", (double)ps[i].cardStrength / (double)ps[i].getStage);
				printf("average of shibari:\t\t%f\n", (double)ps[i].shibari / (double)ps[i].shibariCnt);
				printf("average of fukusuu(done/chance):\t\t%f\n", (double)ps[i].fukusuu / (double)ps[i].getStage);
				printf("average of kaidan(done/chance):\t\t%f\n", (double)ps[i].kaidan / (double)ps[i].getStage);
			}
			if (ps[i].jokerCnt != 0)
				printf("average of joker turn:\t\t%d\n", ps[i].jokerTurnSum / ps[i].jokerCnt);
			if (ps[i].eightGiriCnt != 0)
				printf("average of 8giri turn:\t\t%d\n", ps[i].eightGiriTurnSum / ps[i].eightGiriCnt);
			printf("average of spe3:\t\t%d\n", ps[i].spe3);
			printf("average of kakumei:\t\t%d\n", ps[i].kakumei);
		}
		printf("\n");
		printf("all turn ;\t\t%d , all game: \t\t%d\n", sum_of_turn, game_count);
		printf("average of turn:\t\t%f\n", (double)sum_of_turn / (double)game_count);
	}
	/*************/
	/* game over */
	/*************/
	printf("All games are overed \n");
	for (i = 0; i <= 4; i++)
	{
		shutdown(client_sockfd[i], 2);
		close(client_sockfd[i]);
	}
	printf("All clients are disconnected \n");

#ifndef X_DISPLAY_MISSING
	if (WINDOW_TYPE == 0 || WINDOW_TYPE == 1 || WINDOW_TYPE == 3 || WINDOW_TYPE == 4 || WINDOW_TYPE == 5 || WINDOW_TYPE == 6 || WINDOW_TYPE == 7)
	{
		tmp2 = 0;
		while (1)
		{
			tmp = 1;
			stop_flag = stop_control(&win_info);
			if (stop_flag == 0)
			{
				hidden_name = 1 - hidden_name;
				switch (graph_mode)
				{
				case 0:
					graph_test(point, now_number_of_games - 1, &g_win_info, player_name[hidden_name], &tmp);
					break;
				case 1:
					graph_test2(point, now_number_of_games - 1, &win_info, &g_win_info, player_name[hidden_name], &tmp);
					break;
				}
				tmp2 = 0;
			}
			else if (stop_flag == 1)
			{
				tmp2 = 0;
			}
			else if (stop_flag == 2)
			{
				switch (graph_mode)
				{
				case 1:
					graph_mode = 0;
					graph_test(point, now_number_of_games - 1, &g_win_info, player_name[hidden_name], &tmp);
					tmp2 = 0;
					break;
				case 0:
					graph_mode = 1;
					graph_test2(point, now_number_of_games - 1, &win_info, &g_win_info, player_name[hidden_name], &tmp);
					tmp2 = 0;
					break;
				}
			}
			else
			{
				tmp2++;
				if (tmp2 == 3)
				{
					break;
				}
			}
		}
	}
#endif
	printf("shutting down server...\n");
	shutdown(sockfd, 2);
	close(sockfd);
	so_log_closefile();
	fclose(fp);
	fclose(fp2);

} // niam
