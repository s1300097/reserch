#include"tn_protocol.h"
#include"external.h"

static int num[65536];

int tn_card_write(int client_sockfd, int send_card[8][15], int protocol_version){
	uint32_t tmp_card[8][15];	
	char tmp_string[100];	
	int i,j;	

	switch(protocol_version){
		case 20060:
			write(client_sockfd, send_card, sizeof(i)*8*15);
			break;
		case 20070:
			for(i=0;i<=7;i++){
				for(j=0;j<=14;j++){
					tmp_card[i][j]=htonl((uint32_t)send_card[i][j]*(send_card[i][j]>=0));
				}
			}
			write(client_sockfd, tmp_card, sizeof(i)*8*15);
			break;
		case 20140:
                        conv_table_20140(tmp_string, send_card);
                        tmp_string[33]='\0';
			write(client_sockfd, tmp_string, sizeof(char)*33);
			break;
	}
       return 1;
}

int tn_int_write(int client_sockfd, int i, int protocol_version){
	uint32_t tmp;

	switch(protocol_version){
		case 20060:
			write(client_sockfd, &i, sizeof(i));
			break;
		case 20070:
		case 20140:
			tmp=htonl((uint32_t)i*(i>=0));
			write(client_sockfd, &tmp, sizeof(tmp));
			break;
	}
       return 1;
}

int tn_card_read(int client_sockfd, int read_card[8][15], int protocol_version){
	uint32_t tmp_card[8][15];	
	int tmp_card2[8][15];	
	char tmp_string[100];	
	int i,j;	

	bzero(tmp_card,sizeof(tmp_card));
	switch(protocol_version){
		case 20060:
			read(client_sockfd, read_card, sizeof(i)*8*15);
			break;
		case 20070:
			read(client_sockfd, tmp_card, sizeof(tmp_card));
			for(i=0;i<=7;i++){
				for(j=0;j<=14;j++){
					read_card[i][j]=(int)ntohl(tmp_card[i][j]);
				}
			}
			break;
		case 20140:
			read(client_sockfd, tmp_string, sizeof(char)*33);
                        conv_20140_table(read_card, tmp_string);
                        tmp_string[33]='\0';
			break;
	}
       return 1;
}

