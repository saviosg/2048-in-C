#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
void move_left();
void move_right();
void move_up();
void move_down();
int is_gameover();
int* cells;
int game_over;
void rotate(int count);
void start_map();
void start_game();
int main()
{
  srand(time(0));
  initscr();
  start_color();
  raw();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();

  init_pair(0, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_BLUE, COLOR_WHITE);
  init_pair(4, COLOR_WHITE, COLOR_BLUE);
  init_pair(8, COLOR_WHITE, COLOR_RED);
  init_pair(16, COLOR_WHITE, COLOR_YELLOW);
  init_pair(32, COLOR_WHITE, COLOR_CYAN);
  init_pair(64, COLOR_WHITE, COLOR_MAGENTA);
  init_pair(128, COLOR_WHITE, COLOR_GREEN);
  init_pair(129, COLOR_WHITE, COLOR_RED);//256
  init_pair(130, COLOR_CYAN, COLOR_BLUE);//512
	init_pair(131, COLOR_RED, COLOR_YELLOW);//1024
  init_pair(132, COLOR_YELLOW, COLOR_GREEN);//2048

  start_game();
}

void start_game()
{
  int ch;
  cells = NULL;
  start_map();
  while (1) {
    for (int i = 0; i <= 15; i++) {
      int j = i/4;
      int k = i%4;
      int color_code;
      switch (cells[i]) {
        case 256:
          color_code = 129;
          break;
        case 512:
          color_code = 130;
          break;
        case 1024:
          color_code = 131;
          break;
        case 2048:
          color_code = 132;
          break;
        default:
          color_code = cells[i];
          break;
      }
      attron(COLOR_PAIR(color_code) | A_BOLD);

      for (int row = 0; row <= 4; row++) {
        mvprintw(((j+1)*1)+j*5+row, ((k+1)*1)+(k*6), "      ", cells[i]);
      }
      mvprintw(((j+1)*1)+j*5+2, ((k+1)*1)+(k*6), " %d", cells[i]);
      attroff(COLOR_PAIR(color_code));
    }

    ch = getch();
    switch (ch) {
      case KEY_LEFT:
      case 97:
        move_left();
        break;
      case KEY_RIGHT:
      case 100:
        move_right();
        break;
      case KEY_UP:
      case 119:
        move_up();
        break;
      case KEY_DOWN:
      case 115:
        move_down();
        break;
      case 113:
        endwin();
        return;
      case 114:
        start_map();
        break;
      case 116:
        rotate(1);
        break;
    }
    if (is_gameover()) {
      mvprintw(29, 0, "GAME OVER, PRESS R TO RESTART");
    }
  }
}

void start_map()
{
  int first_cell = rand () % 16;
  int second_cell = rand() % 16;
  if (first_cell == second_cell) {
    second_cell++;
  }
  free(cells);
  cells = calloc(16, sizeof(int));
  cells[first_cell] = 2;
  cells[second_cell] = 2;
}

int is_gameover()
{ 
  for (int current_offset = 0; current_offset <= 15; current_offset++) {
    if (!cells[current_offset]) {
      return 0;
    }
  }
  for (int current_offset = 0; current_offset <= 15; current_offset++) {
    if (current_offset % 4 < 3) {
      if (cells[current_offset] == cells[current_offset+1]) {
        return 0;
      }
    }
    else if (current_offset % 4 > 0) {
      if (cells[current_offset] == cells[current_offset-1]) {
        return 0;
      }
    }
    if (current_offset < 12){
      if (cells[current_offset] == cells[current_offset+4]) {
        return 0;
      }
    }
    else if (current_offset > 4) {
      if (cells[current_offset] == cells[current_offset-4]) {
        return 0;
      }
    }
  }
  return 1;
}

int rotated_index(int src_index, int count) {
  for (int i = 0; i < count; i++) {
    src_index = 4*(3-(src_index%4))+(src_index/4);
  }
  return src_index;
}

void slide()
{
  int merged;
  int moved = merged = 0;
  for (int current_offset = 0; current_offset <= 12; current_offset+=4) {
    // merge cells loop (left-right) below
    // we finish merging at 3, 4 is the last cell
    for (int merger = current_offset; merger <= current_offset + 2; merger++) {
      if (!cells[merger]) {
        continue;
      }
      for (int mergee = merger + 1; mergee < current_offset + 4; mergee++) {
        if (cells[merger] == cells[mergee]) {
          cells[merger] += cells[mergee];
          cells[mergee] = 0;
          merger = mergee;
          merged = 1;
          break;
        }
        else if (cells[mergee]) {
          merger = mergee - 1;
          break;
        }
      }
    }
    // move cells loop (left-right) below
    for (int recv_offset = current_offset; recv_offset <= current_offset + 2; recv_offset++) {
      if (!cells[recv_offset]) {
        for (int send_offset = recv_offset + 1; send_offset <= current_offset + 3; send_offset++) {
          if (cells[send_offset]) {
            cells[recv_offset] = cells[send_offset];
            cells[send_offset] = 0;
            moved = 1;
            break;
          }
        }
      }
    }
  }
  if (!moved && !merged) {
    return;
  }
  int random_offset = rand() % 16;
  for (int random_cell = random_offset, limit = 15; random_cell <= limit; random_cell++) {
    if (!cells[random_cell]) {
      cells[random_cell] = (rand() % 2) ? 2 : 4 ;
      break;
    }
    if (random_cell == limit) {
      if (!random_offset) {
        break;
      }
      limit = random_offset - 1;
      random_cell = rand() % random_offset;
    }
  }
}

void rotate(int count)
{
  int* new_cells = (int*) malloc(sizeof(int)*16);
  
  /*
  for (int i = 0; i <= 12; i+=4) {
    for (int j = 0; j <= 3; j++) {
      int new_index = 4*(3-j)+(i/4);
      int new_offset = new_index / 4;
      int new_column = new_index % 4;
      for (int k = 1; k < count; k++) {
        new_index = 4*(3-new_column)+new_offset;
        new_offset = new_index / 4;
        new_column = new_index % 4;
      }
      new_cells[new_index] = cells[i+j];
    }
  }*/

  for (int i = 0; i <= 15; i++) {
    new_cells[rotated_index(i, count)] = cells[i];
  }
  
  free(cells);
  cells = new_cells;
}

void move_left()
{
  slide(0);
}

void move_right()
{
  rotate(2);
  slide(1);
  rotate(2);
}

void move_up()
{
  rotate(1);
  slide(2);
  rotate(3);
}

void move_down()
{
  rotate(3);
  slide(3);
  rotate(1);
}
