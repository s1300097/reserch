/*daifugo*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "daihinmin.h"

extern const int g_logging;
struct state_type state;

void getState(int cards[8][15])
{
	/*
	  カードテーブルから得られる情報を読み込む
	  引数は手札のカードテーブル
	  情報は広域変数stateに格納される
	*/
	int i;
	// 状態
	if (cards[5][4] > 0)
		state.onset = 1; // 場にカードがないとき 1
	else
		state.onset = 0;
	if (cards[5][6] > 0)
		state.rev = 1; // 革命状態の時 1
	else
		state.rev = 0;
	if (cards[5][5] > 0)
		state.b11 = 1; // 11バック時 1 未使用
	else
		state.b11 = 0;
	if (cards[5][7] > 0)
		state.lock = 1; // しばり時 1
	else
		state.lock = 0;

	if (state.onset == 1)
	{ // 新たな場のとき札の情報をリセット
		state.qty = 0;
		state.ord = 0;
		state.lock = 0;
		for (i = 0; i < 5; i++)
			state.suit[i] = 0;
	}

	for (i = 0; i < 5; i++)
		state.player_qty[i] = cards[6][i]; // 手持ちのカード
	for (i = 0; i < 5; i++)
		state.player_rank[i] = cards[6][5 + i]; // 各プレーヤのランク
	for (i = 0; i < 5; i++)
		state.seat[i] = cards[6][10 + i]; // 誰がどのシートに座っているか
										  // シートiにプレーヤ STATE.SEAT[I]が座っている

	if (cards[4][1] == 2)
		state.joker = 1; ////Jokerがある時 1
	else
		state.joker = 0;
}

void getField(int cards[8][15])
{
	/*
	  場に出たカードの情報を得る。
	  引数は場に出たカードのテーブル
	  情報は広域変数stateに格納される
	*/
	int i, j, count = 0;
	int found_joker = 0;
	i = j = 0;

	// カードのある位置を探す
	while (j < 15 && cards[i][j] == 0)
	{
		state.suit[i] = 0;
		i++;
		if (i == 4)
		{
			j++;
			i = 0;
		}
	}

	// 見つけたカードがジョーカーならば、found_joker=1。
	if (cards[i][j] == 2)
	{
		found_joker = 1;
	}

	// 階段が否か
	if (j < 14)
	{
		if (cards[i][j + 1] > 0)
			state.sequence = 1;
		else
			state.sequence = 0;
	}
	// 枚数を数える また強さを調べる
	if (state.sequence == 0)
	{
		// 枚数組
		for (; i < 5; i++)
		{
			if (cards[i][j] > 0)
			{
				count++;
				state.suit[i] = 1;
			}
			else
			{
				state.suit[i] = 0;
			}
		}

		// ジョーカー単騎が場に出ているならば、state.ordを最大の強さを示すものに設定
		// ノーマルカードの場合は、その強さをそのままstate.ordへ格納
		if ((found_joker == 1) && (count == 1))
		{
			if (state.rev == 0)
			{
				state.ord = 14;
			}
			else
			{
				state.ord = 0;
			}
		}
		else
		{
			state.ord = j;
		}
	}
	else
	{
		// 階段
		while (j + count < 15 && cards[i][j + count] > 0)
		{
			count++;
		}
		if ((state.rev == 0 && state.b11 == 0) || (state.rev == 1 && state.b11 == 1))
		{
			state.ord = j + count - 1;
		}
		else
		{
			state.ord = j;
		}
		state.suit[i] = 1;
	}
	// 枚数を記憶
	state.qty = count;

	if (state.qty > 0)
	{ // 枚数が0より大きいとき 新しい場のフラグを0にする
		state.onset = 0;
	}
}

void showState(struct state_type *state)
{
	/*引数で渡された状態stateの内容を表示する*/
	int i;
	printf("state rev   : %d\n", state->rev);
	printf("state lock  : %d\n", state->lock);
	printf("state joker : %d\n", state->joker);

	printf("state qty   : %d\n", state->qty);
	printf("state ord   : %d\n", state->ord);
	printf("state seq   : %d\n", state->sequence);
	printf("state onset : %d\n", state->onset);
	printf("state suit :");
	for (i = 0; i < 4; i++)
		printf("%d ", state->suit[i]);
	printf("\n");
	printf("state player qty :");
	for (i = 0; i < 5; i++)
		printf("%d ", state->player_qty[i]);
	printf("\n");
	printf("state player rank :");
	for (i = 0; i < 5; i++)
		printf("%d ", state->player_rank[i]);
	printf("\n");
	printf("state player_num on seat :");
	for (i = 0; i < 5; i++)
		printf("%d ", state->seat[i]);
	printf("\n");
}

// それぞれカードの和 共通 差分 逆転 をとる
void cardsOr(int cards1[8][15], int cards2[8][15])
{
	/*
	  cards1にcards2にあるカードを加える
	*/
	int i, j;

	for (i = 0; i < 15; i++)
		for (j = 0; j < 5; j++)
			if (cards2[j][i] > 0)
				cards1[j][i] = 1;
}

void cardsAnd(int cards1[8][15], int cards2[8][15])
{
	/*
	  cards1のカードのうち、cards2にあるものだけをcards1にのこす。
	*/
	int i, j;

	for (i = 0; i < 15; i++)
		for (j = 0; j < 5; j++)
			if (cards1[j][i] == 1 && cards2[j][i] == 1)
				cards1[j][i] = 1;
			else
				cards1[j][i] = 0;
}

void cardsDiff(int cards1[8][15], int cards2[8][15])
{
	/*
	  cards1からcards2にあるカードを削除する
	*/
	int i, j;

	for (i = 0; i < 15; i++)
		for (j = 0; j < 5; j++)
			if (cards2[j][i] == 1)
				cards1[j][i] = 0;
}
void cardsNot(int cards[8][15])
{
	/*
	  カードの有無を反転させる
	*/
	int i, j;

	for (i = 0; i < 15; i++)
		for (j = 0; j < 5; j++)
			if (cards[j][i] == 1)
				cards[j][i] = 0;
			else
				cards[j][i] = 1;
}

void outputTable(int table[8][15])
{
	/*
	  引数で渡されたカードテーブルを出力する
	*/
	int i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 15; j++)
		{
			printf("%i ", table[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void copyTable(int dest_table[8][15], int org_table[8][15])
{
	/*
	  引数で渡されたカードテーブルorg_tableを
	  カードテーブルdest_tableにコピーする
	*/
	int i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 15; j++)
		{
			dest_table[i][j] = org_table[i][j];
		}
	}
}

void copyCards(int dest_cards[8][15], int org_cards[8][15])
{
	/*
	  引数で渡されたカードテーブルorg_cardsのカード情報の部分を
	  カードテーブルdest_cardsにコピーする
	*/
	int i, j;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 15; j++)
		{
			dest_cards[i][j] = org_cards[i][j];
		}
	}
}

void clearCards(int cards[8][15])
{
	/*
	  引数で渡されたカードテーブルcardsのカード情報の部分を全て0にし、カードを一枚も無い状態にする。
	*/
	int s, t;

	for (s = 0; s < 5; s++)
	{
		for (t = 0; t < 15; t++)
		{
			cards[s][t] = 0;
		}
	}
}

void clearTable(int cards[8][15])
{
	/*
	  引数で渡されたカードテーブルcardsを全て0にする。
	*/
	int s, t;

	for (s = 0; s < 8; s++)
	{
		for (t = 0; t < 15; t++)
		{
			cards[s][t] = 0;
		}
	}
}

int beEmptyCards(int cards[8][15])
{
	/*
	  引数で渡されたカードテーブルcardsの含むカードの枚数が0のとき1を、
	  それ以外のとき0を返す
	*/
	int i, j, f = 1;

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 15; j++)
		{
			if (cards[i][j] > 0)
				f = 0;
		}
	}
	return f;
}

int qtyOfCards(int cards[8][15])
{
	/*
	  引数で渡されたカードテーブルcardsの含むカードの枚数を返す
	*/
	int i, j, count = 0;

	for (i = 0; i < 5; i++)
		for (j = 0; j < 15; j++)
			if (cards[i][j] > 0)
				count++;

	return count;
}

void makeJKaidanTable(int tgt_cards[][15], int my_cards[][15])
{
	/*
	  渡されたカードテーブルmy_cardsから、ジョーカーを考慮し階段で出せるかどうかを解析し、
	  結果をテーブルtgt_cardsに格納する。
	*/
	int i, j;
	int count, noJcount; // ジョーカーを使用した場合のカードの枚数,使用しない枚数

	clearTable(tgt_cards); // テーブルのクリア
	if (state.joker == 1)
	{ // jokerがあるとき
		for (i = 0; i < 4; i++)
		{ // 各スート毎に走査し
			noJcount = 0;
			for (j = 13; j >= 0; j--)
			{ // 順番にみて
				if (my_cards[i][j] == 1)
				{			 // カードがあるとき
					count++; // 2つのカウンタを進める
					noJcount++;
				}
				else
				{						  // カードがないとき
					count = noJcount + 1; // ジョーカーありの階段の枚数にジョーカー分を足す
					noJcount = 0;		  // ジョーカーなしの階段の枚数をリセットする
				}

				if (count > 2)
				{							 // 3枚以上のとき
					tgt_cards[i][j] = count; // その枚数をテーブルに格納
				}
				else
				{
					tgt_cards[i][j] = 0; // その他は0にする
				}
			}
		}
	}

	if (g_logging == 1)
	{
		printf("make Joker kaidan \n");
		outputTable(tgt_cards);
	}
}

void makeKaidanTable(int tgt_cards[][15], int my_cards[][15])
{
	/*
	  渡されたカードテーブルmy_cardsから、階段で出せるかどうかを解析し、
	  結果をテーブルtgt_cardsに格納する。
	*/
	int i, j;
	int count;

	clearTable(tgt_cards);
	for (i = 0; i < 4; i++)
	{ // 各スート毎に走査し
		for (j = 13, count = 0; j > 0; j--)
		{ // 順番にみて
			if (my_cards[i][j] == 1)
			{			 // カードがあるとき
				count++; // カウンタを進め
			}
			else
			{
				count = 0; // カードがないときリセットする
			}

			if (count > 2)
			{ // 3枚以上のときその枚数をテーブルに格納
				tgt_cards[i][j] = count;
			}
			else
			{
				tgt_cards[i][j] = 0; // その他は0にする
			}
		}
	}
	if (g_logging == 1)
	{
		printf("make kaidan \n");
		outputTable(tgt_cards);
	}
}

void makeGroupTable(int tgt_cards[][15], int my_cards[][15])
{
	/*
	  渡されたカードテーブルmy_cardsから、2枚以上の枚数組で出せるかどうかを解析し、
	  結果をテーブルtgt_cardsに格納する。
	*/
	int i, j;
	int count;

	clearTable(tgt_cards);
	for (i = 0; i < 15; i++)
	{ // それそれの強さのカードの枚数を数え
		count = my_cards[0][i] + my_cards[1][i] + my_cards[2][i] + my_cards[3][i];
		if (count > 1)
		{ // 枚数が2枚以上のとき
			for (j = 0; j < 4; j++)
			{
				if (my_cards[j][i] == 1)
				{							 // カードを持っている部分に
					tgt_cards[j][i] = count; // その枚数を格納
				}
			}
		}
	}
	if (g_logging == 1)
	{
		printf("make group \n");
		outputTable(tgt_cards);
	}
}

void makeJGroupTable(int tgt_cards[][15], int my_cards[][15])
{
	/*
	  渡されたカードテーブルmy_cardsから、
	  ジョーカーを考慮し2枚以上の枚数組で出せるかどうかを解析し、
	  結果をテーブルtgt_cardsに格納する。
	*/
	int i, j;
	int count;

	clearTable(tgt_cards);
	if (state.joker != 0)
	{
		for (i = 0; i < 14; i++)
		{ // それそれの強さのカードの枚数を数え ジョーカーの分を加える
			count = my_cards[0][i] + my_cards[1][i] + my_cards[2][i] + my_cards[3][i] + 1;
			if (count > 1)
			{ // 枚数が2枚以上のとき
				for (j = 0; j < 4; j++)
				{
					if (my_cards[j][i] == 1)
					{							 // カードを持っている部分に
						tgt_cards[j][i] = count; // その枚数を格納
					}
				}
			}
		}
	}
	if (g_logging == 1)
	{
		printf("make Joker group \n");
		outputTable(tgt_cards);
	}
}

void lowCards(int out_cards[8][15], int my_cards[8][15], int threshold)
{
	/*
	  渡されたカードテーブルmy_cardsのカード部分を
	  threshold以上の部分は0でうめ,thresholdより低い部分をのこし、
	  カードテーブルout_cardsに格納する。
	*/
	int i;
	copyTable(out_cards, my_cards); // my_cardsをコピーして
	for (i = threshold; i < 15; i++)
	{						 // thresholdから15まで
		out_cards[0][i] = 0; // 0でうめる
		out_cards[1][i] = 0;
		out_cards[2][i] = 0;
		out_cards[3][i] = 0;
	}
}

void highCards(int out_cards[8][15], int my_cards[8][15], int threshold)
{
	/*
	  渡されたカードテーブルmy_cardsのカード部分を
	  threshold以下の部分は0でうめ,thresholdより高い部分をのこし
	  カードテーブルout_cardsに格納する
	*/
	int i;
	copyTable(out_cards, my_cards); // my_cardsをコピーして
	for (i = 0; i <= threshold; i++)
	{						 // 0からthresholdまで
		out_cards[0][i] = 0; // 0でうめる
		out_cards[1][i] = 0;
		out_cards[2][i] = 0;
		out_cards[3][i] = 0;
	}
}
int nCards(int n_cards[8][15], int target[8][15], int n)
{
	/*
	  n枚のペアあるいは階段のみをn_cards にのこす。このときテーブルにのる数字はnのみ。
	  カードが無いときは0,あるときは1をかえす。
	*/
	int i, j, flag = 0;
	clearTable(n_cards); // テーブルをクリア
	for (i = 0; i < 4; i++)
		for (j = 0; j < 15; j++) // テーブル全体を走査し
			if (target[i][j] == (int)n)
			{ // nとなるものをみつけたとき
				n_cards[i][j] = n;
				flag = 1; // フラグをたて
			}
			else
			{					   // n以外の場所は
				n_cards[i][j] = 0; // 0で埋める
			}
	return flag;
}

void lockCards(int target_cards[8][15], int suit[5])
{
	/*
	  大域変数state.suitの１が立っているスートのみカードテーブルtarget_cardsに残す。
	*/
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 15; j++)
			target_cards[i][j] *= suit[i]; // suit[i]==1 のときはそのまま,==0のとき0である。
}

void lowGroup(int out_cards[8][15], int my_cards[8][15], int group[8][15])
{
	/*
	  渡された枚数組で出せるカードの情報をのせたgroupとカードテーブルmy_cardsから
	  最も低い枚数組を探し、見つけたらカードテーブルout_cardsにそのカードを載せる。
	*/
	int i, j;				// カウンタ
	int count = 0, qty = 0; // カードの枚数,総数

	clearTable(out_cards);
	for (j = 1; j < 14; j++)
	{ // ランクが低い順に探索する
		for (i = 0; i < 4; i++)
		{
			if (group[i][j] > 1)
			{
				out_cards[i][j] = 1; // out_cardsにフラグを立てる
				count++;
				qty = group[i][j];
			}
		}
		if (count > 0)
			break; // ループ脱出用フラグが立っていたら
	}

	for (i = 0; count < qty; i++)
	{
		if (my_cards[i][j] == 0 && (state.lock == 0 || state.suit[i] == 1))
		{
			out_cards[i][j] = 2; // ジョーカー用フラグを立てる
			count++;
		}
	}
}

void highGroup(int out_cards[8][15], int my_cards[8][15], int group[8][15])
{
	/*
	  渡された枚数組で出せるカードの情報をのせたgroupとカードテーブルmy_cardsから
	  最も高い枚数組を探し、見つけたらカードテーブルout_cardsにそのカードを載せる。
	*/
	int i, j;				// カウンタ
	int count = 0, qty = 0; // カードの枚数,総数

	clearTable(out_cards);
	for (j = 13; j > 0; j--)
	{ // ランクが低い順に探索する
		for (i = 0; i < 4; i++)
		{
			if (group[i][j] > 1)
			{						 // groupテーブルに2以上の数字を発見したら
				out_cards[i][j] = 1; // out_cardsにフラグを立てる
				count++;
				qty = group[i][j];
			}
		}
		if (count > 0)
			break; // ループ脱出用フラグが立っていたら
	}

	for (i = 0; count < qty; i++)
	{
		if (my_cards[i][j] == 0 && (state.lock == 0 || state.suit[i] == 1))
		{
			out_cards[i][j] = 2; // ジョーカー用フラグを立てる
			count++;
		}
	}
}

void lowSequence(int out_cards[8][15], int my_cards[8][15], int sequence[8][15])
{
	/*
	  渡された階段で出せるカードの情報をのせたgroupとカードテーブルmy_cardsから
	  最も低い階段を探し、見つけたらカードテーブルout_cardsにそのカードを載せる。
	*/
	int i, j, lowvalue, lowline = 0, lowcolumn = 0;

	lowvalue = 0;

	clearTable(out_cards);
	i = 0;

	// lowsequenceの発見
	while ((i < 15) && (lowvalue == 0))
	{ // 階段テーブル中に階段が見つかるまで繰り返し
		j = 0;
		while (j < 4)
		{
			if (sequence[j][i] != 0)
			{ // 低い数字から調べ,階段テーブルが0以外だったら分岐
				if (sequence[j][i] > lowvalue)
				{							   // 同じ数字を起点として作られる階段の中で最長か否か
					lowvalue = sequence[j][i]; // 最長だったら値と場所を保存
					lowline = j;
					lowcolumn = i;
				}
			}
			j++;
		}
		if (lowvalue == 0)
		{
			i++;
		}
	}

	// out_cardsへの書出し
	if (lowvalue != 0)
	{ // 階段が見つからなかったらout_cardsには書出さない
		for (i = lowcolumn; i < (lowcolumn + lowvalue); i++)
		{
			if (my_cards[lowline][i] == 1)
			{
				out_cards[lowline][i] = 1; // 普通の手札として持っていたら1を立てる
			}
			else
			{
				out_cards[lowline][i] = 2; // 持っていなかったらジョーカーなので2を立てる
			}
		}
	}
}

void highSequence(int out_cards[8][15], int my_cards[8][15], int sequence[8][15])
{
	/*
	  渡された階段で出せるカードの情報をのせたgroupとカードテーブルmy_cardsから
	  最も高い階段を探し、見つけたらカードテーブルout_cardsにそのカードを載せる。
	*/
	int i, j, k, highvalue, highline = 0, highcolumn = 0, prevalue;
	highvalue = 0;

	clearTable(out_cards);
	i = 14;

	// highsequenceの発見
	while ((i > 0) && (highvalue == 0))
	{ // 階段テーブル中に階段が見つかるまで繰り返し
		j = 0;
		while (j < 4)
		{
			k = -1;
			if ((sequence[j][i] != 0) && (my_cards[j][i] != 0))
			{ // 高い数字から調べ,階段テーブルが0以外だったら分岐
				do
				{ // 見つけた階段の最高値から,最長の階段を探す
					if (sequence[j][i - k] >= highvalue)
					{									// 同じ最高値を持つ階段の中で最長か否か
						highvalue = sequence[j][i - k]; // 最長だったら記録
						highline = j;
						highcolumn = i - k;
					}
					prevalue = sequence[j][i - k];
					k++;
				} while ((i - k >= 0) && (prevalue <= sequence[j][i - k]));
			}
			j++;
		}
		if (highvalue == 0)
		{
			i--;
		}
	}

	// out_cardsへの書出し
	for (i = highcolumn; i < (highcolumn + highvalue); i++)
	{
		if (my_cards[highline][i] == 1)
		{
			out_cards[highline][i] = 1; // 普通の手札として持っていたら1を立てる
		}
		else
		{
			out_cards[highline][i] = 2; // 持っていなかったらジョーカーなので2を立てる
		}
	}
}

// my_cards(手札)からペア,階段等の役のカードを除去したものをout_cardに格納する
void removeGroup(int out_cards[8][15], int my_cards[8][15], int group[8][15])
{
	/*
	  渡された枚数組で出せるカードの情報をのせたgroupとカードテーブルmy_cardsから
	  枚数組以外のカードを探し、カードテーブルout_cardsにそのカードを載せる。
	*/
	int i, j;

	for (i = 0; i < 15; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if ((my_cards[j][i] == 1) && (group[j][i] == 0))
			{
				out_cards[j][i] = 1; // mycardsに存在し,かつ役テーブルにない場合1
			}
			else
			{
				out_cards[j][i] = 0; // それ以外(mycardsにないか,役テーブルにある)の場合0
			}
		}
	}
}

void removeSequence(int out_cards[8][15], int my_cards[8][15], int sequence[8][15])
{
	/*
	  渡された階段で出せるカードの情報をのせたgroupとカードテーブルmy_cardsから
	  階段以外のカードを探し、カードテーブルout_cardsにそのカードを載せる。
	*/
	int i, j, k;

	for (j = 0; j < 4; j++)
	{
		for (i = 0; i < 15; i++)
		{
			if ((my_cards[j][i] == 1) && (sequence[j][i] == 0))
			{
				out_cards[j][i] = 1; // mycardsに存在し,かつ役テーブルにない場合1
			}
			else if (sequence[j][i] > 2)
			{
				for (k = 0; k < sequence[j][i]; k++)
				{
					out_cards[j][i + k] = 0;
				}
				i += k - 1;
			}
			else
			{
				out_cards[j][i] = 0; // それ以外(mycardsにないか,役テーブルにある)の場合0
			}
		}
	}
}
void lowSolo(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  低い方から探して,最初に見つけたカードを一枚out_cardsにのせる。
	  joker_flagが1のとき,カードが見つからなければ,jokerを一枚out_cardsにのせる。
	*/
	int i, j, find_flag = 0;

	clearTable(out_cards); // テーブルをクリア
	for (j = 1; j < 14 && find_flag == 0; j++)
	{ // 低い方からさがし
		for (i = 0; i < 4 && find_flag == 0; i++)
		{
			if (my_cards[i][j] == 1)
			{									  // カードを見つけたら
				find_flag = 1;					  // フラグを立て
				out_cards[i][j] = my_cards[i][j]; // out_cardsにのせ,ループを抜ける。
			}
		}
	}
	if (find_flag == 0 && joker_flag == 1)
	{						  // 見つからなかったとき
		out_cards[0][14] = 2; // ジョーカーをのせる
	}
}

void highSolo(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  高い方から探して,最初に見つけたカードを一枚out_cardsにのせる。
	  joker_flagがあるとき,カードが見つからなければ,jokerを一枚out_cardsにのせる。
	*/
	int i, j, find_flag = 0;

	clearTable(out_cards); // テーブルをクリア
	for (j = 13; j > 0 && find_flag == 0; j--)
	{ // 高い方からさがし
		for (i = 0; i < 4 && find_flag == 0; i++)
		{
			if (my_cards[i][j] == 1)
			{									  // カードを見つけたら
				find_flag = 1;					  // フラグを立て
				out_cards[i][j] = my_cards[i][j]; // out_cardsにのせ,ループを抜ける。
			}
		}
	}
	if (find_flag == 0 && joker_flag == 1)
	{						 // 見つからなかったとき
		out_cards[0][0] = 2; // ジョーカーをのせる
	}
}

void change(int out_cards[8][15], int my_cards[8][15], int num_of_change)
{
	/*
	  カード交換時のアルゴリズム
	  大富豪あるいは富豪が、大貧民あるいは貧民にカードを渡す時のカードを
	  カードテーブルmy_cardsと交換枚数num_of_changeに応じて、
	  低いほうから選びカードテーブルout_cardsにのせる
	*/
	int count = 0;
	int one_card[8][15];

	clearTable(out_cards);
	while (count < num_of_change)
	{
		lowSolo(one_card, my_cards, 0);
		cardsDiff(my_cards, one_card);
		cardsOr(out_cards, one_card);
		count++;
	}
}

void lead(int out_cards[8][15], int my_cards[8][15])
{
	/*
	   新しくカードを提出するときの選択ルーチン
	   カードテーブルmy_cardsから階段=>ペア=>一枚の順で枚数の多いほうから走査し,
	   低いカードからみて、はじめて見つけたものを out_cardsにのせる。
	 */
	int group[8][15];	  // 枚数組を調べるためのテーブル
	int sequence[8][15];  // 階段を調べるためのテーブル
	int temp[8][15];	  // 一時使用用のテーブル
	int i, find_flag = 0; // 手札が発見したか否かのフラグ

	clearTable(group);
	clearTable(sequence);
	clearTable(temp);
	if (state.joker == 1)
	{										  // ジョーカーがあるとき,ジョーカーを考慮し,
		makeJGroupTable(group, my_cards);	  // 階段と枚数組があるかを調べ,
		makeJKaidanTable(sequence, my_cards); // テーブルに格納する
	}
	else
	{
		makeGroupTable(group, my_cards);	 // ジョーカーがないときの階段と枚数組の
		makeKaidanTable(sequence, my_cards); // 状況をテーブルに格納する
	}

	for (i = 15; i >= 3 && find_flag == 0; i--)
	{										   // 枚数の大きい方から,見つかるまで
		find_flag = nCards(temp, sequence, i); // 階段があるかをしらべ,

		if (find_flag == 1)
		{											// 見つかったとき
			lowSequence(out_cards, my_cards, temp); // そのなかで最も低いものをout_cards
		} // にのせる
	}
	for (i = 5; i >= 2 && find_flag == 0; i--)
	{										// 枚数の大きい方から,見つかるまで
		find_flag = nCards(temp, group, i); // 枚数組があるかを調べ,
		if (find_flag == 1)
		{										 // 見つかったとき
			lowGroup(out_cards, my_cards, temp); // そのなかで最も低いものをout_cards
		} // のせる
	}
	if (find_flag == 0)
	{											   // まだ見つからないとき
		lowSolo(out_cards, my_cards, state.joker); // 最も低いカードをout_cardsにのせる。
	}
}

void leadRev(int out_cards[8][15], int my_cards[8][15])
{
	/*
	  革命時用の新しくカードを提出するときの選択ルーチン
	  カードテーブルmy_cardsから階段=>ペア=>一枚の順で枚数の多いほうから走査し,
	  高いカードからみて、はじめて見つけたものを out_cardsにのせる。
	*/
	int group[8][15];	  // 枚数組を調べるためのテーブル
	int sequence[8][15];  // 階段を調べるためのテーブル
	int temp[8][15];	  // 一時使用用のテーブル
	int i, find_flag = 0; // 手札が発見したか否かのフラグ
	// clearTable(group);
	// clearTable(sequence);
	// clearTable(temp);
	if (state.joker == 1)
	{										  // ジョーカーがあるとき,ジョーカーを考慮し,
		makeJGroupTable(group, my_cards);	  // 階段と枚数組があるかを調べ,
		makeJKaidanTable(sequence, my_cards); // テーブルに格納する
	}
	else
	{
		makeGroupTable(group, my_cards);	 // ジョーカーがないときの階段と枚数組の
		makeKaidanTable(sequence, my_cards); // 状況をテーブルに格納する
	}
	for (i = 15; i >= 3 && find_flag == 0; i--)
	{ // 枚数の大きい方から,見つかるまで

		find_flag = nCards(temp, sequence, i); // 階段組があるかを調べ,

		if (find_flag == 1)
		{										  // 見つかったとき
			highGroup(out_cards, my_cards, temp); // そのなかで最も高いものをout_cards
		} // にのせる
	}
	for (i = 5; i >= 2 && find_flag == 0; i--)
	{										// 枚数の大きい方から,見つかるまで
		find_flag = nCards(temp, group, i); // 枚数組があるかを調べ,
		if (find_flag == 1)
		{										  // 見つかったとき
			highGroup(out_cards, my_cards, temp); // そのなかで最も高いものをout_cards
		} // にのせる
	}
	if (find_flag == 0)
	{												// まだ見つからないとき
		highSolo(out_cards, my_cards, state.joker); // 最も高いカードをout_cardsにのせる
	}
}

void followSolo(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  他のプレーヤーに続いてカードを一枚で出すときのルーチン
	  joker_flagが1の時ジョーカーを使おうとする
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	int group[8][15];	 // 枚数組を調べるためのテーブル
	int sequence[8][15]; // 階段を調べるためのテーブル
	int temp[8][15];	 // 一時使用用のテーブル

	makeGroupTable(group, my_cards);	 // 枚数組を書き出す
	makeKaidanTable(sequence, my_cards); // 階段を書き出す

	removeSequence(temp, my_cards, sequence); // 階段を除去
	removeGroup(out_cards, temp, group);	  // 枚数組を除去

	highCards(temp, out_cards, state.ord); // 場のカードより弱いカードを除去

	if (state.lock == 1)
	{
		lockCards(temp, state.suit); // ロックされているとき出せないカードを除去
	}
	lowSolo(out_cards, temp, state.joker); // 残ったカードから弱いカードを抜き出す
}

void followGroup(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  他のプレーヤーに続いてカードを枚数組で出すときのルーチン
	  joker_flagが1の時ジョーカーを使おうとする
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	int group[8][15];
	int ngroup[8][15];
	int temp[8][15];

	highCards(temp, my_cards, state.ord); // 場より強いカードを残す
	if (state.lock == 1)
	{								 // ロックされているとき
		lockCards(temp, state.suit); // 出せないカードを除去
	}
	makeGroupTable(group, temp); // 残ったものから枚数組を書き出す
	if (nCards(ngroup, group, state.qty) == 0 && state.joker == 1)
	{
		// 場と同じ枚数の階段が無いときジョーカーを使って探す
		makeJGroupTable(group, temp);
		nCards(ngroup, group, state.qty); // 場と同じ枚数の組のみのこす。
	}
	lowGroup(out_cards, my_cards, ngroup); // 一番弱い組を抜き出す
}

void followSequence(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  他のプレーヤーに続いてカードを階段で出すときのルーチン
	  joker_flagが1の時ジョーカーを使おうとする
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	int seq[8][15];
	int nseq[8][15];
	int temp[8][15];

	highCards(temp, my_cards, state.ord); // 場より強いカードを残す
	if (state.lock == 1)
	{								 // ロックされているとき
		lockCards(temp, state.suit); // 出せないカードを除去
	}
	makeKaidanTable(seq, temp); // 階段を書き出す
	if (nCards(nseq, seq, state.qty) == 0 && state.joker == 1)
	{
		// 場と同じ枚数の階段が無いときジョーカーを使って探す
		makeJKaidanTable(seq, temp);
		nCards(nseq, seq, state.qty); // 場と同じ枚数の組のみのこす。
	}
	lowSequence(out_cards, my_cards, nseq); // 一番弱い階段を
}

void followSoloRev(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  革命状態のときに他のプレーヤーに続いてカードを一枚で出すときのルーチン
	  joker_flagが1の時ジョーカーを使おうとする
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	int group[8][15];
	int sequence[8][15];
	int temp[8][15];

	makeGroupTable(group, my_cards);	 // 枚数組を書き出す
	makeKaidanTable(sequence, my_cards); // 階段を書き出す

	removeSequence(temp, my_cards, sequence); // 階段を除去
	removeGroup(out_cards, temp, group);	  // 枚数組を除去
	lowCards(temp, out_cards, state.ord);	  // 場のカードより強いカードを除去
	if (state.lock == 1)
		if (state.lock == 1)
		{
			lockCards(temp, state.suit); // ロックされているとき出せないカードを除去
		}
	highSolo(out_cards, temp, state.joker); // 残ったカードから強いカードを抜き出す
}

void followGroupRev(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  革命状態のときに他のプレーヤーに続いてカードを枚数組で出すときのルーチン
	  joker_flagが1の時ジョーカーを使おうとする
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	int group[8][15];
	int ngroup[8][15];
	int temp[8][15];

	lowCards(temp, my_cards, state.ord); // 場より弱いカードを残す
	if (state.lock == 1)
	{								 // ロックされているとき
		lockCards(temp, state.suit); // 出せないカードを除去
	}
	makeGroupTable(group, temp); // 枚数組を書き出す
	if (nCards(ngroup, group, state.qty) == 0 && state.joker == 1)
	{
		// 場と同じ枚数の組が無いときジョーカーを使って探す
		makeJGroupTable(group, temp);
		nCards(ngroup, group, state.qty); // 場と同じ枚数の組のみのこす。
	}
	highGroup(out_cards, my_cards, ngroup); // 残ったものから一番強い組を抜き出す
}

void followSequenceRev(int out_cards[8][15], int my_cards[8][15], int joker_flag)
{
	/*
	  革命状態のときに他のプレーヤーに続いてカードを階段で出すときのルーチン
	  joker_flagが1の時ジョーカーを使おうとする
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	int seq[8][15];
	int nseq[8][15];
	int temp[8][15];

	lowCards(temp, my_cards, state.ord); // 場より弱いカードを残す
	if (state.lock == 1)
	{								 // ロックされているとき
		lockCards(temp, state.suit); // 出せないカードを除去
	}
	makeKaidanTable(seq, temp); // 階段を書き出す
	if (nCards(nseq, seq, state.qty) == 0 && state.joker == 1)
	{
		// 場と同じ枚数の階段が無いときジョーカーを使って探す
		makeJKaidanTable(seq, temp);
		nCards(nseq, seq, state.qty); // 場と同じ枚数の階段のみのこす。
	}
	highSequence(out_cards, my_cards, nseq); // 残ったものから一番強い組を抜き出す
}

void follow(int out_cards[8][15], int my_cards[8][15])
{
	/*
	  他のプレーヤーに続いてカードを出すときのルーチン
	  場の状態stateに応じて一枚、枚数組、階段の場合に分けて
	  対応すれる関数を呼び出す
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	clearTable(out_cards);
	if (state.qty == 1)
	{
		followSolo(out_cards, my_cards, state.joker); // 一枚のとき
	}
	else
	{
		if (state.sequence == 0)
		{
			followGroup(out_cards, my_cards, state.joker); // 枚数組のとき
		}
		else
		{
			followSequence(out_cards, my_cards, state.joker); // 階段のとき
		}
	}
}

void followRev(int out_cards[8][15], int my_cards[8][15])
{
	/*
	  他のプレーヤーに続いてカードを出すときのルーチン
	  場の状態stateに応じて一枚、枚数組、階段の場合に分けて
	  対応すれる関数を呼び出す
	  提出するカードはカードテーブルout_cardsに格納される
	*/
	clearTable(out_cards);
	if (state.qty == 1)
	{
		followSoloRev(out_cards, my_cards, state.joker); // 一枚のとき
	}
	else
	{
		if (state.sequence == 0)
		{
			followGroupRev(out_cards, my_cards, state.joker); // 枚数組のとき
		}
		else
		{
			followSequenceRev(out_cards, my_cards, state.joker); // 階段のとき
		}
	}
}

int cmpCards(int cards1[8][15], int cards2[8][15])
{
	/*
	   カードテーブルcards1、cards2のカード部分を比較し、
	   異なっていれば1、一致していれば0を返す
	 */
	int i, j, flag = 0;

	for (i = 0; i < 5; i++)
		for (j = 0; j < 15; j++)
			if (cards1[i][j] != cards2[i][j])
				flag = 1;

	return flag;
}

int cmpState(struct state_type *state1, struct state_type *state2)
{
	/*
	  状態を格納するstate1とstate2を比較し、一致すれば0を、
	  異なっていればそれ以外を返す
	*/
	int i, flag = 0;
	if (state1->ord != state2->ord)
		flag += 1;
	if (state1->qty != state2->qty)
		flag += 2;
	if (state1->sequence != state2->sequence)
		flag += 4;
	for (i = 0; i < 5; i++)
		if (state1->suit[i] != state2->suit[i])
			flag += 8;
	if (state1->onset != state2->onset)
		flag += 16;
	return flag;
}

int getLastPlayerNum(int ba_cards[8][15])
{
	/*
	  最後パス以外のカード提出をしたプレーヤーの番号を返す。
	  この関数を正常に動作させるためには、
	  サーバから場に出たカードをもらう度に
	  この関数を呼び出す必要がある。
	*/
	static struct state_type last_state;
	static int last_player_num = -1;

	if (g_logging == 1)
	{ // ���O�̕\��
		printf("Now state \n");
		showState(&state);
		printf(" Last state \n");
		showState(&last_state);
	}

	if (cmpState(&last_state, &state) != 0)
	{									  // 場の状態に変化が起きたら
		last_player_num = ba_cards[5][3]; // 最後のプレーヤと
		last_state = state;				  // 最新の状態を更新する
	}

	if (g_logging == 1)
	{//ログの表示
		printf("last player num : %d\n", last_player_num);
	}

	return last_player_num;
}
