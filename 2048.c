#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PDC_WIDE TRUE
#include "PDCursesMod/curses.h"

#define BOARD_Y_OFFSET 8
#define HEADER_X_OFFSET 2
#define CELL_WIDTH 6
#define CELL_HEIGHT 3
#define CELL_VERTICAL_MARGIN 1
#define CELL_HORIZONTAL_MARGIN 2

void move_left();
void move_right();
void move_up();
void move_down();
int is_gameover();
void slide(int direction);
void rotate(int count);
void start_map();
void start_game();

int* cells;
int game_over;
int score;

int main()
{
  srand(time(0));
  resize_term(24, 34);
  initscr();
  start_color();
  raw();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);
  PDC_set_title("2048");

  init_color(40, 933, 894, 854);
  init_color(41, 929, 878, 784);
  init_color(42, 949, 694, 474);
  init_color(43, 960, 584, 388);
  init_color(44, 964, 486, 372);
  init_color(45, 964, 368, 231);
  init_color(46, 929, 811, 447);
  init_color(47, 929, 800, 380);
  init_color(48, 929, 784, 313);
  init_color(49, 929, 772, 247);
  init_color(50, 929, 760, 180);

  init_color(51, 467, 431, 396); //DARK TILE COLOR
  init_pair(2, 51, 40);
  init_pair(4, 51, 41);
  
  init_color(53, 804, 757, 706); //EMPTY TILE
  init_pair(1, 53, 53);

  init_color(54, 733, 678, 627); //BOARD BACKGROUND
  init_pair(3, 54, 54);
  
  init_color(55, 980, 972, 937); //SCREEN BACKGROUND
  init_color(52, 976, 965, 949); //LIGHT TILE COLOR
  init_pair(8, 52, 42);
  init_pair(16, 52, 43);
  init_pair(32, 52, 44);
  init_pair(64, 52, 45);
  init_pair(128, 52, 46);
  init_pair(129, 52, 47);//256
  init_pair(130, 52, 48);//512
	init_pair(131, 52, 49);//1024
  init_pair(132, 52, 50);//2048
  init_pair(133, COLOR_RED, 55);//BAD
  init_pair(134, COLOR_GREEN, 55);//GOOD
  assume_default_colors(51, 55);
  start_game();
}

void start_game()
{
  int ch;
  score = 0;
  cells = NULL;
  start_map();
  
  mvprintw(3, 1, "(WASD/ARROWS) MOVE  (Q) QUIT");
  mvprintw(4, 1, "(R) RESTART         (T) ROTATE");
  while (1) {
    attron(COLOR_PAIR(3));
    for (int i = 7; i <= 7+4*3+4; i++) {
      mvprintw(i, 0, "                                  ");
    }
      attroff(COLOR_PAIR(3));
    for (int i = 0; i <= 15; i++) {
      int row = i/4;
      int column = i%4;
      int color_code;
      switch (cells[i]) {
        case 0:
          color_code = 1;
          break;
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

      attron(COLOR_PAIR(color_code));

      for (int j = 0; j < CELL_HEIGHT; j++) {
        mvprintw(BOARD_Y_OFFSET+(CELL_HEIGHT*row)+(CELL_VERTICAL_MARGIN*row)+j, CELL_WIDTH*column+(CELL_HORIZONTAL_MARGIN*(column+1)), "      ", cells[j]);
      }
      mvprintw(BOARD_Y_OFFSET+(CELL_HEIGHT*row)+(CELL_VERTICAL_MARGIN*row)+1, CELL_WIDTH*column+(CELL_HORIZONTAL_MARGIN*(column+1)), " %d", cells[i]); 
      attroff(COLOR_PAIR(color_code));
      mvprintw(1, 1, "SCORE: %d  ", score);
    }

    ch = getch();
    switch (ch) {
      case KEY_LEFT:
      case 97:
        slide(0);
        break;
      case KEY_RIGHT:
      case 100:
        slide(2);
        break;
      case KEY_UP:
      case 119:
        slide(3);
        break;
      case KEY_DOWN:
      case 115:
        slide(1);
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
      attron(COLOR_PAIR(133));
      mvprintw(5, 1, "GAME OVER, PRESS R TO RESTART");
      attroff(COLOR_PAIR(133));
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
  score = 0;
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

void slide(int direction)
{
  int merged;
  int moved = merged = 0;
  for (int current_offset = 0; current_offset <= 12; current_offset+=4) {
    // merge cells loop (left-right) below
     for (int merger = current_offset; merger <= current_offset+2; merger++) {
      if (!cells[rotated_index(merger, direction)]) {
        continue;
      }
      for (int mergee = merger+1; mergee <= current_offset+3; mergee++) {
        if (cells[rotated_index(merger, direction)] == cells[rotated_index(mergee, direction)]) {
          score += cells[rotated_index(merger, direction)] *= 2;
          cells[rotated_index(mergee, direction)] = 0;
          merged = 1;
          if (cells[rotated_index(merger, direction)] == 2048) {
            attron(COLOR_PAIR(134));
            mvprintw(5, 1, "CONGRATULATIONS");
            attroff(COLOR_PAIR(134));
          }
          merger = mergee;
          break;
        }
        else if (cells[rotated_index(mergee,direction)]) {
          merger = mergee-1;
          break;
        }
      }
    }
    // move cells loop (left-right) below
    for (int recv_offset = current_offset; recv_offset <= current_offset+2; recv_offset++) {
      if (!cells[rotated_index(recv_offset, direction)]) {
        for (int send_offset = recv_offset+1; send_offset <= current_offset+3; send_offset++) {
          if (cells[rotated_index(send_offset, direction)]) {
            cells[rotated_index(recv_offset, direction)] = cells[rotated_index(send_offset, direction)];
            cells[rotated_index(send_offset, direction)] = 0;
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
  int random_offset = rand() % 10;
  if (random_offset < 4) {
    random_offset = 0;
  }
  else if (random_offset < 7) {
    random_offset = 4;
  }
  else if (random_offset < 9) {
    random_offset = 8;
  }
  else {
    random_offset = 12;
  }
  random_offset += rand() % 4;
  for (int i = random_offset; i <= 15; i++){
    if (!cells[rotated_index(i, direction+2)]) {
      cells[rotated_index(i, direction+2)] = (rand() % 2) ? 2 : 4 ;
      break;
    }
    else if (i == 15) {
      i = -1;
    }
  }
}

void rotate(int count)
{
  int* new_cells = (int*) malloc(sizeof(int)*16);

  for (int i = 0; i <= 15; i++) {
    new_cells[rotated_index(i, count)] = cells[i];
  }
  
  free(cells);
  cells = new_cells;
}
