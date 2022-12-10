#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_LEN 20
#define MAX_POSITIONS 100000

struct pos_s {
   int x;
   int y;
};
typedef struct pos_s pos_t;

struct positions_s {
   pos_t positions[MAX_POSITIONS];
   int size;
};
typedef struct positions_s positions_t;

pos_t move_head(char, pos_t);
pos_t move_tail(pos_t, pos_t);
void save_pos(pos_t, positions_t *);


int main(int argc, char **argv) {

   if (argc != 2) {
      printf("missing filename\n");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("unable to open %s\n", filename);
      exit(1);
   }

   char buf[BUF_LEN];

   // first part
   pos_t head_pos = {};
   pos_t tail_pos = {};
   positions_t positions_part1 = {};
   save_pos(tail_pos, &positions_part1);

   // second part
   pos_t long_tail[9] = {};
   positions_t positions_part2 = {};
   save_pos(long_tail[8], &positions_part2);

   while (fgets(buf, BUF_LEN, fp) != NULL) {
      char move = buf[0];
      int steps = atoi(buf + 2);
      for (int i=0; i<steps; i++) {
         head_pos = move_head(move, head_pos);

         // first part
         tail_pos = move_tail(head_pos, tail_pos);
         save_pos(tail_pos, &positions_part1);

         // second part
         for (int j=0; j<9; j++) {
            long_tail[j] = move_tail(j == 0 ? head_pos : long_tail[j-1], long_tail[j]);
         }
         save_pos(long_tail[8], &positions_part2);
      }
   }

   printf("visited positions (first part): %d\n", positions_part1.size);
   printf("visited positions (second part): %d\n", positions_part2.size);

   fclose(fp);

}


pos_t move_head(char move, pos_t current_pos) {
   pos_t new_pos = {current_pos.x, current_pos.y};
   switch (move) {
      case 'U': new_pos.y++; break;
      case 'D': new_pos.y--; break;
      case 'L': new_pos.x--; break;
      case 'R': new_pos.x++; break;
      default:
         printf("unknown move: %c", move);
         exit(1);
   }
   return new_pos;
}


pos_t move_tail(pos_t head, pos_t tail) {
   if (abs(head.x - tail.x) > 1 || abs(head.y - tail.y) > 1) {
      int move_x = head.x > tail.x ? 1 : head.x < tail.x ? -1 : 0;
      int move_y = head.y > tail.y ? 1 : head.y < tail.y ? -1 : 0;
      pos_t new_pos = {tail.x + move_x, tail.y + move_y};
      return new_pos;
   }
   return tail;
}


void save_pos(pos_t pos, positions_t *positions) {
   for (int i=0; i<positions->size; i++) {
      if (positions->positions[i].x == pos.x && positions->positions[i].y == pos.y)
         return;
   }
   positions->positions[positions->size].x = pos.x;
   positions->positions[positions->size].y = pos.y;
   positions->size++;
}
