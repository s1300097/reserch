/*daihinmin*/ 

struct state_type
{
  int ord;
  int sequence;
  int qty;
  int rev;
  int b11;
  int lock;
  int onset;
  int suit[5];

  int player_qty[5];
  int player_rank[5];
  int seat[5];

  int joker;
};

extern struct state_type state;


void showState(struct state_type *state);
void getField(int cards[8][15]);
void getState(int cards[8][15]);
void getField(int cards[8][15]);

void cardsOr(int cards1[8][15],int cards2[8][15]);
void cardsAnd(int cards1[8][15],int cards2[8][15]);
void cardsDiff(int cards1[8][15],int cards2[8][15]);
void cardsNot(int cards[8][15]);

void outputTable(int table_val[8][15]);
void copyTable(int dest_table[8][15],int org_table[8][15]);
void copyCards(int cardsTar[8][15],int cardsOrg[8][15]);
void clearCards( int cards[8][15]);
void clearTable( int cards[8][15]);
int beEmptyCards(int cards[8][15]);
int qtyOfCards(int cards[8][15]);

void makeJKaidanTable(int tgt_cards[][15], int my_cards[][15]);
void makeKaidanTable(int tgt_cards[][15], int my_cards[][15]);
void makeGroupTable(int tgt_cards[][15], int my_cards[][15]);
void makeJGroupTable(int tgt_cards[][15], int my_cards[][15]);

void lowCards(int out_cards[8][15],int my_cards[8][15],int threshold);
void highCards(int out_cards[8][15],int my_cards[8][15],int threshold);
int nCards(int n_cards[8][15],int target[8][15],int n);

void lockCards(int target_cards[8][15],int suit[5]);
void lowGroup(int out_cards[8][15],int my_cards[8][15],int group[8][15]);
void highGroup(int out_cards[8][15],int my_cards[8][15],int group[8][15]) ;
void lowSequence(int out_cards[8][15],int my_cards[8][15],int sequence[8][15]);
void highSequence (int out_cards[8][15],int my_cards[8][15],int sequence[8][15]);
void removeGroup(int out_cards[8][15],int my_cards[8][15],int group[8][15]);
void removeSequence(int out_cards[8][15],int my_cards[8][15],int sequence[8][15]);
void lowSolo(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void highSolo(int out_cards[8][15],int my_cards[8][15],int joker_flag);
int allGroup(int candidates[10][8][15], int my_cards[8][15], int group[8][15]);
int allSequence(int candidates[20][8][15], int my_cards[8][15], int sequence[8][15]);
int allSolo(int candidates[20][8][15], int my_cards[8][15], int joker_flag);

void change(int out_cards[8][15],int my_cards[8][15],int num_of_change);

void lead(int out_cards[8][15],int my_cards[8][15]);
void leadRev(int out_cards[8][15],int my_cards[8][15]);

void followSolo(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void followGroup(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void followSequence(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void followSoloRev(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void followGroupRev(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void followSequenceRev(int out_cards[8][15],int my_cards[8][15],int joker_flag);
void follow(int out_cards[8][15],int my_cards[8][15]);
void followRev(int out_cards[8][15],int my_cards[8][15]);

int cmpCards(int cards1[8][15],int  cards2[8][15]);
int cmpState(struct state_type* state1,struct state_type* state2);

int getLastPlayerNum(int ba_cards[8][15]);
