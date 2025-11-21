
/*connection*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>

// #include "connection.h"
#include "daihinmin.h"

#define PROTOCOL_VERSION	20070		//�v���g�R�����@�[�W������\������
#define DEFAULT_SERVER		"127.0.0.1"	//�f�t�H���g�̃T�[�o�̃A�h���X ������Ŏw��
#define DEFAULT_PORT		42485		//�f�t�H���g�̃|�[�g�ԍ� �����Ŏw��
#define DEFAULT_NAME		"default"	//�f�t�H���g�̃N���C�A���g�� ������Ŏw��

extern const int g_logging;

/* �ÓI�֐��̊֐��v���g�^�C�v�錾 */
static int refreshTable(int table_val[8][15]);
static int sendTable(int table_val[8][15]);
static int openSocket(const char ip_addr[], uint16_t portnum_data);
static int sendProfile(const char user[15]);

/*connection�����ϐ�*/

//�\�P�b�g�֘A�̕ϐ���ÓI�O���[�o���ϐ��Ƃ��Đ錾
static int g_sockfd;
static int g_buf_len;
static struct sockaddr_in g_client_addr;

//�ڑ�����T�[�o�A�|�[�g���i�[����ϐ�
static char     server_name[256]= DEFAULT_SERVER;
static uint16_t port            = DEFAULT_PORT;
//�T�[�o�ɒʒm����N���C�A���g��
static char     user_name[15]   = DEFAULT_NAME;

//�e�[�u������M�����񐔂��J�E���g
static int table_count=0;

//host�ɐڑ��� �Q�[���ɎQ������ �v���[���[�ԍ���Ԃ�
int entryToGame(void){
  int my_playernum;  //�v���C���[�ԍ����L������
  //�T�[�o�ɑ΂��ă\�P�b�g��p�ӂ��Aconnect����
  if((openSocket(server_name,port))== -1){
    printf("failed to open socket to server[%s:%d].\n",server_name,port);
    exit (1);
  }
  if(g_logging==1){
    printf("connectiong to server was finished successfully[%s:%d].\n",server_name,port);
  }

  sendProfile(user_name);     //�N���C�A���g�̏��𑗐M

  if(g_logging==1){
    printf("send profile .\n");
  }
  //���g�̃v���C���[�ԍ����T�[�o������炤
  if(read(g_sockfd, &my_playernum, sizeof(my_playernum)) > 0){
    my_playernum=ntohl(my_playernum);
    if(g_logging==1){
      printf("my player number is %d\n",my_playernum);
    }
  }
  else{
    printf("failed to get player number.\n");
    exit (1);
  }

  return my_playernum; //���g�̃v���C���[�ԍ���Ԃ��B
}


//�\�P�b�g��close���s�� ���������ꍇ�� 0 ��Ԃ��B�G���[�����������ꍇ�� -1 ��Ԃ�
int closeSocket(){
  return close(g_sockfd);
}

//�V���b�t�����ꂽ�J�[�h���󂯎�� ����ڂ̃Q�[������Ԃ�
int startGame(int table[8][15]){
  static int game_count=0;
  //���O��肪�L���Ȃ�Q�[���ԍ���\��
  if(g_logging == 1){
    printf("game number is %d.\n",game_count);
  }
  table_count=0;// �Q�[���J�n�Ɠ����Ƀe�[�u���̃J�E���g��0�ɂ���B
  //����̃J�[�h���󂯎��
  if(((refreshTable(table)))== -1) {
    printf("failed to get initial cards table for exchange.\n");
    exit(1) ;
  }

  //���O��肪�L���Ȃ�A�z�z���ꂽ�J�[�h(�Ƃ������e�[�u��)��\��
  if(g_logging ==1){
    printf("initial cards table is follow.\n");
    outputTable(table);
  }

  game_count++;

  return game_count;
}

//�J�[�h�������̃J�[�h�̒�o
void sendChangingCards(int cards[8][15]){
  if(sendTable(cards)==-1){
    //���s������G���[���͂��Ē�~
    printf("sending cards table was failed.\n");
    exit (1);
  }
  else{
    //��������loggin�t���O�������Ă�����A���b�Z�[�W��f��
    if(g_logging==1){
      printf("sending cards-table was done successfully.\n");
    }
  }
  //loggin�t���O�������Ă�����A�e�[�u���̓��e���o��
  if(g_logging == 1){
    //�e�[�u���̓��e���o��
    printf("sent card table was......\n");
    outputTable(cards);
  }
}

//�J�[�h���󂯎�� �����̃^�[���ł����1��Ԃ�
int receiveCards(int cards[8][15]){
  //�J�[�h���󂯎��
  if(((refreshTable(cards)))== -1 ){
    //���s�����ꍇ�̃G���[����
    printf("failed to get my cards table.\n");
    exit (1);
  }
  else{
    //������ logging�t���O�ɉ������O�̕\��
    if(g_logging == 1){
      printf("recieved my cards table.\n");
      printf("  table count=%d\n",table_count);
      outputTable(cards);
    }
  }
  getState(cards);   //��̏�Ԃ̓ǂݏo��
  return cards[5][2];
}

//�J�[�h���o���󗝂��ꂽ���ۂ���Ԃ�
int sendCards(int cards[8][15]){
  int accept_flag; //��o�����J�[�h���󗝂��ꂽ���𔻕ʂ���ϐ�
  //�J�[�h�𑗐M����
  if(g_logging==1){
    printf("send card table was following.\n");
    outputTable(cards);
  }

  if((sendTable(cards)) == -1 ){ //�J�[�h�𑗐M�� ���s���̓��b�Z�[�W��\��
    printf("failed to send card sending table. \n");
    exit(1);
  }

  //accept_flag���T�[�o����󂯎��
  if((read(g_sockfd, &accept_flag, sizeof(accept_flag))) < 0 ){
    printf("failed to get accept sign.\n");
    exit(1);
  }
  else{
     if(g_logging==1){
       printf("accept_flag=%d\n",(int)ntohl(accept_flag));
     }
  }
  return ntohl(accept_flag);
}

//���E���h�̍Ō�ɃQ�[�����I�������� �T�[�o����󂯂Ƃ� ���̒l��Ԃ��B
int beGameEnd(void){
  int one_gameend_flag;
  //�Q�[���I���̃t���O���󂯎��
  if ((read(g_sockfd, &one_gameend_flag, sizeof(one_gameend_flag))) < 0 ){
    //��M���s�� ���b�Z�[�W��\������~
    printf("failed to check if the game was finished.\n");
    exit(1);
  }else{
    //��M������ �l�̃o�C�g�I�[�_�[�𒼂�
    one_gameend_flag=ntohl( one_gameend_flag);
  }
  return one_gameend_flag;
}

void lookField(int cards[8][15]){
  if(((refreshTable(cards)))== -1 ){
    //���s�����ꍇ�̃G���[����
    printf("failed to get result table.\n");
    exit (1);
  }else{
    //���������O��\��
    if(g_logging == 1){
      printf("received result table.\n");
      outputTable(cards);
      printf("end bacards\n");
    }
  }
  getField(cards); //��ɏo�Ă���J�[�h�̏���ǂݏo��
}

void checkArg(int argc,char* argv[]){
  /*
    �n���ꂽ�R�}���h���C������^����ꂽ�����̏�����͂��A�K�v�ɉ�����
    �T�[�o�A�h���X�A�|�[�g�ԍ��A�N���C�A���g����ύX����B
  */
  const char Option[]="[-h server_adress] [-p port] [-n user_name]";
  int        arg_count=1;

  while(arg_count<argc){
    if( strcmp(argv[arg_count],"--help")==0){
      printf("usage : %s %s\n",argv[0],Option);
      exit(0);
    }else if (strcmp(argv[arg_count],"-h")==0){
      arg_count++;
      if (arg_count<argc){
	strcpy(server_name,argv[arg_count]);
      }else{
	printf("%s -h hostname\n",argv[0]);
	exit(1);
      }
    }else if ((strcmp(argv[arg_count],"-p"))==0){
      arg_count++;
      if (arg_count<argc){
	port = (uint16_t)atoi(argv[arg_count]);
      }else{
	printf("%s -p port\n",argv[0]);
	exit(1);
      }
    }else if ((strcmp(argv[arg_count],"-n"))==0){
      arg_count++;
      if (arg_count<argc){
	strcpy(user_name ,argv[arg_count]);
      }else{
	printf("%s -n user_name\n",argv[0]);
	exit(1);
      }
    }else{
      printf("%s : unknown option : %s \n",argv[0],argv[arg_count]);
      printf("usage : %s %s\n",argv[0],Option);
      exit(1);
    }
    arg_count++;
  }
}

//�T�[�o����e�[�u�������󂯎��A�����Ȃ�0���s�Ȃ�-1��Ԃ�
static int refreshTable(int table_val[8][15]){
  uint32_t net_table[8][15];
  if ((g_buf_len = read(g_sockfd,net_table, 480)) > 0){
    int i,j;
    //�S�Ẵe�[�u���̗v�f���l�b�g���[�N�I�[�_�[����z�X�g�I�[�_�[�ɕϊ�
    for(i=0;i<8;i++)
      for(j=0;j<15;j++)
	table_val[i][j]=ntohl(net_table[i][j]);
    table_count++;
    return 0;
  }
  else{
    return(-1);
  }
}

//�T�[�o�Ƀe�[�u�����𓊂���֐��B�����Ȃ�0�@���s��-1��Ԃ�
static int sendTable(int table_val[8][15]){
  uint32_t net_table[8][15];
  int i,j;
  //�S�Ẵe�[�u���̗v�f���z�X�g�I�[�_�[����l�b�g���[�N�I�[�_�[�ɕϊ�
  for(i=0;i<8;i++)
    for(j=0;j<15;j++)
      net_table[i][j]=htonl(table_val[i][j]);
  //�ϊ������e�[�u���𑗐M
  if((g_buf_len = write(g_sockfd, net_table, 480))> 0){
    return (0);
  }
  else{
    return (-1);
  }
}

//�\�P�b�g�̐ݒ�E�ڑ����s�� ������0�A���s��-1��Ԃ�
static int openSocket(const char addr[], uint16_t port_num){
  //�\�P�b�g�̐���
  if ((g_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    return(-1);
  }

  /* �|�[�g�ƃA�h���X�̐ݒ� */
  bzero((char*)&g_client_addr,sizeof(g_client_addr));
  g_client_addr.sin_family = PF_INET;
  g_client_addr.sin_port = htons(port_num);
  g_client_addr.sin_addr.s_addr = inet_addr(addr);

  //IP�A�h���X�Ŏw�肳��Ă��Ȃ��Ƃ��A�z�X�g���̉��������݂�
  if (g_client_addr.sin_addr.s_addr == 0xffffffff) {
    struct hostent *host;
    host = gethostbyname(addr);
    if (host == NULL) {
      printf("failed to gethostbyname() : %s.\n",addr);
      return -1;//�z�X�g�������Ɏ��s�����Ƃ��A-1��Ԃ�
    }
    g_client_addr.sin_addr.s_addr=
      *(unsigned int *)host->h_addr_list[0];
  }

  /* �T�[�o�ɃR�l�N�g���� */
  if (connect(g_sockfd,(struct sockaddr *)&g_client_addr, sizeof(g_client_addr)) == 0){
    return 0;
  }
  return -1;
}

//�N���C�A���g�̏��𑗐M
static int sendProfile(const char user[15]){
  int profile[8][15];
  int i;

  bzero((char *) &profile, sizeof(profile));        //�e�[�u����0�ł��߂�
  profile[0][0]=PROTOCOL_VERSION;                   //2007�N�x�ł�錾
  for(i=0;i<15;i++) profile[1][i]=(int)user[i];     //����Җ����e�[�u���Ɋi�[

  //���M
  if(sendTable(profile)==-1){                       //���s������G���[���o�͂���~
    printf("sending profile table was failed.\n");
    exit (1);
  }

  //��������logging�t���O�������Ă�����A���b�Z�[�W�ƃe�[�u���̓��e���o�͂���
  if(g_logging==1){
    printf("sending profile was done successfully.\n");
    printf("sent profile table was......\n");
    outputTable(profile);
  }
  return 0;
}