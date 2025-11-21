#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>

int conv_table_20140(char string[100], int cards[8][15]){
  int i,j;
  int tmp;

  sprintf(string,"\0");
 
  // cards 
  for(i=1;i<14;i++){
    tmp=1*(cards[0][i]==1)+2*(cards[1][i]==1)+4*(cards[2][i]==1)+8*(cards[3][i]==1); 
    sprintf(string,"%s%X",string,tmp);
  }

  tmp=0xFF;
  for(i=0;i<5*15;i++){
    if(cards[i/15][i%15]==2){
      tmp=i;
      break;
    }
  }
  sprintf(string,"%s%02X",string,tmp);

  // change cards
  tmp=8*(cards[5][0]==1)+(cards[5][1]+2);
  sprintf(string,"%s%X",string,tmp);

  // now player
  tmp=8*(cards[5][2]==1)+cards[5][3];
  sprintf(string,"%s%X",string,tmp);
  
  // game flags 1
  tmp=0;
  for(i=0;i<4;i++){
    tmp |= ((cards[5][4+i]==1) << i);
  }
  sprintf(string,"%s%X",string,tmp);

  // players info and game info
  for(i=0;i<15;i++){
    sprintf(string,"%s%X",string,cards[6][i]);
  }
}

int conv_20140_table(int cards[8][15], char string[100]){
  int i,j;
  long tmp;
  char read_char[2];
  char *endptr;

  bzero(cards, 8*15*sizeof(int));

  // cards
  for(i=0;i<=12;i++){
    sprintf(read_char,"%c",string[i]);
    tmp=strtol(read_char,&endptr,16);
    cards[0][i+1]=tmp%2;
    cards[1][i+1]=tmp/2%2;
    cards[2][i+1]=tmp/4%2;
    cards[3][i+1]=tmp/8%2;
  }
  sprintf(read_char,"%c%c",string[13],string[14]);
  tmp=strtol(read_char,&endptr,16);
  if(tmp != 0XFF){
    cards[tmp/15][tmp%15]=2;
  }
  
  // card change
  sprintf(read_char,"%c",string[15]);
  tmp=strtol(read_char,&endptr,16);
  cards[5][0]=tmp/8%2;
  cards[5][1]=tmp%8-2;

  // now player
  sprintf(read_char,"%c",string[16]);
  tmp=strtol(read_char,&endptr,16);
  cards[5][2]=tmp/8%2;
  cards[5][3]=tmp%8;

  // game flags 1
  sprintf(read_char,"%c",string[17]);
  tmp=strtol(read_char,&endptr,16);
  cards[5][4]=tmp%2;
  cards[5][5]=tmp/2%2;
  cards[5][6]=tmp/4%2;
  cards[5][7]=tmp/8%2;

  // players info and game info
  for(i=0;i<=14;i++){
    sprintf(read_char,"%c",string[18+i]);
    tmp=strtol(read_char,&endptr,16);
    cards[6][i]=tmp;
  }

} 

