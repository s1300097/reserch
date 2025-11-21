
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

#define PROTOCOL_VERSION	20070		// プロトコルバージョン（2007仕様）
#define DEFAULT_SERVER		"127.0.0.1"	// デフォルト接続先のサーバーアドレス
#define DEFAULT_PORT		42485		// デフォルトで使うポート番号
#define DEFAULT_NAME		"default"	// デフォルトのクライアント名

extern const int g_logging;

/* 内部関数のプロトタイプ */
static int refreshTable(int table_val[8][15]);
static int sendTable(int table_val[8][15]);
static int openSocket(const char ip_addr[], uint16_t portnum_data);
static int sendProfile(const char user[15]);

/* connection で使う変数 */

// ソケット関連のグローバル変数
static int g_sockfd;
static int g_buf_len;
static struct sockaddr_in g_client_addr;

// サーバー・ポートなど接続先設定
static char     server_name[256]= DEFAULT_SERVER;
static uint16_t port            = DEFAULT_PORT;
// サーバーに通知するクライアント名
static char     user_name[15]   = DEFAULT_NAME;

// テーブル送受信回数（ログ用）
static int table_count=0;

// サーバーに接続してプレイヤー番号を取得
int entryToGame(void){
  int my_playernum;  // 自分のプレイヤー番号
  // サーバーにソケット接続を張る
  if((openSocket(server_name,port))== -1){
    printf("failed to open socket to server[%s:%d].\n",server_name,port);
    exit (1);
  }
  if(g_logging==1){
    printf("connectiong to server was finished successfully[%s:%d].\n",server_name,port);
  }

  sendProfile(user_name);     // クライアント情報を送信

  if(g_logging==1){
    printf("send profile .\n");
  }
  // プレイヤー番号を受信
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

  return my_playernum; // プレイヤー番号を返す
}


// ソケットを閉じる 成功:0 / 失敗:-1
int closeSocket(){
  return close(g_sockfd);
}

// シャッフル後のカードテーブルを受信してゲーム開始
int startGame(int table[8][15]){
  static int game_count=0;
  // ログ出力: ゲーム回数
  if(g_logging == 1){
    printf("game number is %d.\n",game_count);
  }
  table_count=0;// 交換テーブル受信回数をリセット
  // 初期の交換用カードを受信
  if(((refreshTable(table)))== -1) {
    printf("failed to get initial cards table for exchange.\n");
    exit(1) ;
  }

  // ログ出力: 初期交換カードのテーブル
  if(g_logging ==1){
    printf("initial cards table is follow.\n");
    outputTable(table);
  }

  game_count++;

  return game_count;
}

// 交換後のカードテーブルを送信
void sendChangingCards(int cards[8][15]){
  if(sendTable(cards)==-1){
    // 送信に失敗したら終了
    printf("sending cards table was failed.\n");
    exit (1);
  }
  else{
    // loggingフラグが立っていれば完了メッセージ
    if(g_logging==1){
      printf("sending cards-table was done successfully.\n");
    }
  }
  // loggingフラグが立っていれば受信テーブルも表示
  if(g_logging == 1){
    // テーブル内容を出力
    printf("sent card table was......\n");
    outputTable(cards);
  }
}

// カード交換は1回のみ
int receiveCards(int cards[8][15]){
  // カードを受信
  if(((refreshTable(cards)))== -1 ){
    // エラーなら終了
    printf("failed to get my cards table.\n");
    exit (1);
  }
  else{
    // loggingフラグが立っていれば交換後テーブルも表示
    if(g_logging == 1){
      printf("recieved my cards table.\n");
      printf("  table count=%d\n",table_count);
      outputTable(cards);
    }
  }
  getState(cards);   // 現在の場の状態を取得
  return cards[5][2];
}

// サーバーからのカード提出要求に応答
int sendCards(int cards[8][15]){
  int accept_flag; // 提出可否のフラグ
  // カードを送信
  if(g_logging==1){
    printf("send card table was following.\n");
    outputTable(cards);
  }

  if((sendTable(cards)) == -1 ){ // 送信に失敗したらメッセージを出して終了
    printf("failed to send card sending table. \n");
    exit(1);
  }

  // accept_flag はサーバーからの応答
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

// 1ラウンド終了で結果を送受信し、順位を決定
int beGameEnd(void){
  int one_gameend_flag;
  // ゲーム終了フラグを確認
  if ((read(g_sockfd, &one_gameend_flag, sizeof(one_gameend_flag))) < 0 ){
    // 送信に失敗したらエラーメッセージ
    printf("failed to check if the game was finished.\n");
    exit(1);
  }else{
    // 受信した順位をネットワークオーダーから復元
    one_gameend_flag=ntohl( one_gameend_flag);
  }
  return one_gameend_flag;
}

void lookField(int cards[8][15]){
  if(((refreshTable(cards)))== -1 ){
    // loggingフラグが立っていれば結果テーブルを表示
    printf("failed to get result table.\n");
    exit (1);
  }else{
    // 結果のログ出力終了
    if(g_logging == 1){
      printf("received result table.\n");
      outputTable(cards);
      printf("end bacards\n");
    }
  }
  getField(cards); // 場に出たカード情報を取得
}

void checkArg(int argc,char* argv[]){
  /*
    -h サーバーアドレス, -p ポート番号, -n ユーザー名の指定を受け付ける
    コマンドラインオプションの使い方を示す
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

// テーブルを受信してメモリに展開。成功なら0、失敗なら-1を返す
static int refreshTable(int table_val[8][15]){
  uint32_t net_table[8][15];
  if ((g_buf_len = read(g_sockfd,net_table, 480)) > 0){
    int i,j;
    // 各要素をネットワークバイトオーダーからホスト順に変換
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

// テーブルを送信する。成功:0 失敗:-1
static int sendTable(int table_val[8][15]){
  uint32_t net_table[8][15];
  int i,j;
  // 送信時もネットワークバイトオーダーに変換
  for(i=0;i<8;i++)
    for(j=0;j<15;j++)
      net_table[i][j]=htonl(table_val[i][j]);
  // テーブル本体を送信
  if((g_buf_len = write(g_sockfd, net_table, 480))> 0){
    return (0);
  }
  else{
    return (-1);
  }
}

// ソケット接続を開く 成功:0 失敗:-1
static int openSocket(const char addr[], uint16_t port_num){
  // ソケットを作成
  if ((g_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    return(-1);
  }

  /* ポートとアドレスの設定 */
  bzero((char*)&g_client_addr,sizeof(g_client_addr));
  g_client_addr.sin_family = PF_INET;
  g_client_addr.sin_port = htons(port_num);
  g_client_addr.sin_addr.s_addr = inet_addr(addr);

  // IPアドレスに変換できなければホスト名解決も試す
  if (g_client_addr.sin_addr.s_addr == 0xffffffff) {
    struct hostent *host;
    host = gethostbyname(addr);
    if (host == NULL) {
      printf("failed to gethostbyname() : %s.\n",addr);
      return -1;// 名前解決に失敗したら -1 を返す
    }
    g_client_addr.sin_addr.s_addr=
      *(unsigned int *)host->h_addr_list[0];
  }

  /* サーバーに connect */
  if (connect(g_sockfd,(struct sockaddr *)&g_client_addr, sizeof(g_client_addr)) == 0){
    return 0;
  }
  return -1;
}

// クライアント情報を送信
static int sendProfile(const char user[15]){
  int profile[8][15];
  int i;

  bzero((char *) &profile, sizeof(profile));        // プロファイルテーブルを初期化
  profile[0][0]=PROTOCOL_VERSION;                   // 2007年版のプロトコル番号
  for(i=0;i<15;i++) profile[1][i]=(int)user[i];     // ユーザー名をテーブルに格納

  // 送信する
  if(sendTable(profile)==-1){                       // 送信に失敗したらエラー終了
    printf("sending profile table was failed.\n");
    exit (1);
  }

  // loggingフラグが立っていれば送信結果を表示
  if(g_logging==1){
    printf("sending profile was done successfully.\n");
    printf("sent profile table was......\n");
    outputTable(profile);
  }
  return 0;
}
