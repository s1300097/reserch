#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "daihinmin.h"
#include "connection.h"

const int g_logging = 0;

#define MAX_MOVES 1024

struct move_list
{
  int moves[MAX_MOVES][8][15];
  int count;
};

static int popcount4(int value)
{
  int c = 0;
  for (int i = 0; i < 4; i++)
  {
    if (value & (1 << i))
      c++;
  }
  return c;
}

static void init_move_list(struct move_list *list)
{
  list->count = 0;
  for (int i = 0; i < MAX_MOVES; i++)
  {
    clearTable(list->moves[i]);
  }
}

static void log_moves(const struct move_list *list)
{
  if (g_logging == 0)
    return;

  for (int i = 0; i < list->count; i++)
  {
    printf("move #%d\n", i);
    outputTable((int (*)[15])list->moves[i]);
  }
}

static int usesAvailableCards(int move[8][15], int hand[8][15], int has_joker)
{
  int joker_used = 0;
  for (int s = 0; s < 4; s++)
  {
    for (int r = 0; r < 15; r++)
    {
      if (move[s][r] == 0)
        continue;
      if (move[s][r] == 1)
      {
        if (hand[s][r] == 0)
          return 0;
      }
      else if (move[s][r] == 2)
      {
        joker_used++;
      }
      else
      {
        return 0;
      }
    }
  }
  if (joker_used > 0 && has_joker == 0)
    return 0;
  if (joker_used > 1)
    return 0;
  return 1;
}

static void analyzeMoveState(int move[8][15], struct state_type *out_state)
{
  struct state_type backup = state;
  getField(move);
  *out_state = state;
  state = backup;
}

static int beats_current_field(const struct state_type *candidate, const struct state_type *current)
{
  if (current->onset == 1)
    return 1; // new field, anything goes

  if (candidate->qty != current->qty)
    return 0;
  if (candidate->sequence != current->sequence)
    return 0;

  if (current->lock == 1)
  {
    for (int i = 0; i < 4; i++)
    {
      if (candidate->suit[i] == 1 && current->suit[i] == 0)
        return 0;
    }
  }

  if (current->rev == 0)
  {
    return candidate->ord > current->ord;
  }
  else
  {
    return candidate->ord < current->ord;
  }
}

static int is_move_valid(int move[8][15], int hand[8][15])
{
  struct state_type move_state;
  struct state_type current = state;

  if (beEmptyCards(move) == 1)
    return 0;

  if (usesAvailableCards(move, hand, current.joker) == 0)
    return 0;

  analyzeMoveState(move, &move_state);
  if (move_state.qty == 0)
    return 0;

  return beats_current_field(&move_state, &current);
}

static void add_move(struct move_list *list, int move[8][15])
{
  for (int i = 0; i < list->count; i++)
  {
    if (cmpCards(list->moves[i], move) == 0)
      return;
  }
  if (list->count >= MAX_MOVES)
    return;
  copyTable(list->moves[list->count], move);
  list->count++;
}

static void try_add_move(struct move_list *list, int move[8][15], int hand[8][15])
{
  if (is_move_valid(move, hand))
  {
    add_move(list, move);
  }
}

static void collect_single_moves(struct move_list *list, int hand[8][15])
{
  int move[8][15];
  for (int s = 0; s < 4; s++)
  {
    for (int r = 0; r < 14; r++)
    {
      if (hand[s][r] > 0)
      {
        clearTable(move);
        move[s][r] = 1;
        try_add_move(list, move, hand);
      }
    }
  }

  if (state.joker == 1)
  {
    clearTable(move);
    move[0][14] = 2; // joker as strong card
    try_add_move(list, move, hand);
  }
}

static void collect_group_moves(struct move_list *list, int hand[8][15], int exact_qty)
{
  int move[8][15];
  for (int rank = 1; rank < 14; rank++)
  {
    int have_mask = 0;
    for (int suit = 0; suit < 4; suit++)
    {
      if (hand[suit][rank] > 0)
        have_mask |= (1 << suit);
    }

    for (int subset = 1; subset < 13; subset++)
    {
      if ((subset & ~have_mask) != 0)
        continue;
      int count = popcount4(subset);
      if (count >= 2 && (exact_qty == 0 || count == exact_qty))
      {
        clearTable(move);
        for (int suit = 0; suit < 4; suit++)
        {
          if (subset & (1 << suit))
            move[suit][rank] = 1;
        }
        try_add_move(list, move, hand);
      }
      if (state.joker == 1 && count >= 1)
      {
        int total_with_joker = count + 1;
        if (exact_qty != 0 && total_with_joker != exact_qty)
          continue;
        clearTable(move);
        for (int suit = 0; suit < 4; suit++)
        {
          if (subset & (1 << suit))
            move[suit][rank] = 1;
        }
        int joker_pos = 0;
        for (int suit = 0; suit < 4; suit++)
        {
          if ((subset & (1 << suit)) == 0)
          {
            joker_pos = suit;
            break;
          }
        }
        move[joker_pos][rank] = 2;
        try_add_move(list, move, hand);
      }
    }
  }
}

static void collect_sequence_without_joker(struct move_list *list, int hand[8][15], int exact_len)
{
  int move[8][15];
  for (int suit = 0; suit < 4; suit++)
  {
    int rank = 0;
    while (rank < 14)
    {
      while (rank < 14 && hand[suit][rank] == 0)
      {
        rank++;
      }
      if (rank >= 14)
        break;
      int start = rank;
      while (rank < 14 && hand[suit][rank] == 1)
      {
        rank++;
      }
      int end = rank - 1;
      if (end - start + 1 >= 3)
      {
        for (int len = 3; len <= end - start + 1; len++)
        {
          if (exact_len != 0 && len != exact_len)
            continue;
          for (int s = start; s + len - 1 <= end; s++)
          {
            clearTable(move);
            for (int r = s; r < s + len; r++)
            {
              move[suit][r] = 1;
            }
            try_add_move(list, move, hand);
          }
        }
      }
    }
  }
}

static void collect_sequence_with_joker(struct move_list *list, int hand[8][15], int exact_len)
{
  if (state.joker == 0)
    return;

  int move[8][15];
  for (int suit = 0; suit < 4; suit++)
  {
    for (int start = 0; start < 14; start++)
    {
      int missing_used = 0;
      int missing_rank = -1;
      int length = 0;
      for (int rank = start; rank < 14; rank++)
      {
        if (hand[suit][rank] == 1)
        {
          length++;
        }
        else if (missing_used == 0)
        {
          missing_used = 1;
          missing_rank = rank;
          length++;
        }
        else
        {
          break;
        }

        if (length >= 3 && missing_used == 1)
        {
          if (exact_len != 0 && length != exact_len)
            continue;
          clearTable(move);
          for (int r = start; r <= rank; r++)
          {
            if (hand[suit][r] == 1)
            {
              move[suit][r] = 1;
            }
          }
          move[suit][missing_rank] = 2;
          try_add_move(list, move, hand);
        }
      }
    }
  }
}

static void collect_playable_moves(struct move_list *list, int hand[8][15])
{
  int move[8][15];
  clearTable(move);
  add_move(list, move);
  // 場に何も無いときは全ての手を候補にする
  if (state.onset == 1)
  {
    collect_single_moves(list, hand);
    collect_group_moves(list, hand, 0);
    collect_sequence_without_joker(list, hand, 0);
    collect_sequence_with_joker(list, hand, 0);
    return;
  }

  // 以降は場の状態に合わせて候補種別を絞る
  if (state.qty == 1)
  {
    collect_single_moves(list, hand);
    return;
  }

  if (state.sequence == 0)
  {
    collect_group_moves(list, hand, state.qty);
  }
  else
  {
    collect_sequence_without_joker(list, hand, state.qty);
    collect_sequence_with_joker(list, hand, state.qty);
  }
}

static void choose_random_move(int out_cards[8][15], int hand[8][15])
{
  struct move_list list;
  init_move_list(&list);
  collect_playable_moves(&list, hand);

  if (list.count == 0)
  {
    clearTable(out_cards); // pass
    return;
  }
  else if(g_logging == 1)
  {
    showState(&state);
    printf("found %d possible moves\n", list.count);
    log_moves(&list);
  }

  int idx = rand() % list.count;
  copyTable(out_cards, list.moves[idx]);
}

int main(int argc, char *argv[])
{

  int my_playernum;
  int whole_gameend_flag = 0;
  int one_gameend_flag = 0;
  int accept_flag = 0;
  int game_count = 0;

  int own_cards_buf[8][15];
  int own_cards[8][15];
  int ba_cards_buf[8][15];
  int ba_cards[8][15];

  srand((unsigned int)(time(NULL) ^ getpid()));

  // parse command line and connect settings
  checkArg(argc, argv);

  // join game
  my_playernum = entryToGame();

  while (whole_gameend_flag == 0)
  {
    one_gameend_flag = 0;

    game_count = startGame(own_cards_buf);
    copyTable(own_cards, own_cards_buf);

    if (own_cards[5][0] == 0)
    {
      printf("not card-change turn?\n");
      exit(1);
    }
    else
    {
      if (own_cards[5][1] > 0 && own_cards[5][1] < 100)
      {
        int change_qty = own_cards[5][1];
        int select_cards[8][15] = {{0}};

        change(select_cards, own_cards, change_qty);

        sendChangingCards(select_cards);
      }
      else
      {
        // nothing to exchange
      }
    }

    while (one_gameend_flag == 0)
    {
      int select_cards[8][15] = {{0}};

      if (receiveCards(own_cards_buf) == 1)
      {
        clearCards(select_cards);
        copyTable(own_cards, own_cards_buf);

        choose_random_move(select_cards, own_cards);

        accept_flag = sendCards(select_cards);
        (void)accept_flag;
      }
      else
      {
        // not my turn
      }

      lookField(ba_cards_buf);
      copyTable(ba_cards, ba_cards_buf);

      switch (beGameEnd())
      {
      case 0:
        one_gameend_flag = 0;
        whole_gameend_flag = 0;
        break;
      case 1:
        one_gameend_flag = 1;
        whole_gameend_flag = 0;
        if (g_logging == 1)
        {
          printf("game #%d was finished.\n", game_count);
        }
        break;
      default:
        one_gameend_flag = 1;
        whole_gameend_flag = 1;
        if (g_logging == 1)
        {
          printf("All game was finished(Total %d games.)\n", game_count);
        }
        break;
      }
    }
  }
  if (closeSocket() != 0)
  {
    printf("failed to close socket\n");
    exit(1);
  }
  exit(0);
}
