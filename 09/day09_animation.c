#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define BUF_LEN 20
#define MAX_POSITIONS 100000

// e
//#define IMG_WIDTH 380
//#define IMG_HEIGHT 220
//#define START_X 70
//#define START_Y 110

// g
#define IMG_WIDTH 170
#define IMG_HEIGHT 200
#define START_X 120
#define START_Y 70

#define WRITE_IMG 0

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
void write_image(int, pos_t, int size, pos_t[size], positions_t *);


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
   pos_t head_pos = {START_X, START_Y};
   pos_t tail_pos = {START_X, START_Y};
   positions_t positions_part1 = {};
   save_pos(tail_pos, &positions_part1);

   // second part
   pos_t long_tail[9] = {};
   for (int i=0; i<9; i++) {
      long_tail[i].x = START_X;
      long_tail[i].y = START_Y;
   }
   positions_t positions_part2 = {};
   save_pos(long_tail[8], &positions_part2);

   int min_x = INT_MAX, max_x = INT_MIN;
   int min_y = INT_MAX, max_y = INT_MIN;

   int img_index = 0;
   while (fgets(buf, BUF_LEN, fp) != NULL) {
      char move = buf[0];
      int steps = atoi(buf + 2);
      for (int i=0; i<steps; i++) {
         head_pos = move_head(move, head_pos);

         // first part
         tail_pos = move_tail(head_pos, tail_pos);
         save_pos(tail_pos, &positions_part1);

         // second part
         if (head_pos.x < min_x) min_x = head_pos.x;
         if (head_pos.x > max_x) max_x = head_pos.x;
         if (head_pos.y < min_y) min_y = head_pos.y;
         if (head_pos.y > max_y) max_y = head_pos.y;
         for (int j=0; j<9; j++) {
            long_tail[j] = move_tail(j == 0 ? head_pos : long_tail[j-1], long_tail[j]);
            if (long_tail[j].x < min_x) min_x = long_tail[j].x;
            if (long_tail[j].x > max_x) max_x = long_tail[j].x;
            if (long_tail[j].y < min_y) min_y = long_tail[j].y;
            if (long_tail[j].y > max_y) max_y = long_tail[j].y;
         }
         save_pos(long_tail[8], &positions_part2);

         // img
         if (WRITE_IMG)
            write_image(img_index++, head_pos, 9, long_tail, &positions_part2);
      }
   }

   fclose(fp);

   printf("min x: %d, max x: %d, min y: %d, max y %d\n", min_x, max_x, min_y, max_y);
   printf("visited positions (first part): %d\n", positions_part1.size);
   printf("visited positions (second part): %d\n", positions_part2.size);

}


pos_t move_head(char move, pos_t current_pos) {
   pos_t new_pos = {current_pos.x, current_pos.y};
   switch (move) {
      case 'U': new_pos.y--; break;
      case 'D': new_pos.y++; break;
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


void check_pixel_pos(pos_t pos) {
   if (pos.x < 0 || pos.y < 0 || pos.x > IMG_WIDTH || pos.y > IMG_HEIGHT) {
      printf("invalid pos: (%d, %d)\n", pos.x, pos.y);
      exit(1);
   }
}


void write_image(int index, pos_t head, int size, pos_t long_tail[size], positions_t *positions) {

   const unsigned char head_color[4] = "\xff\x00\x00";
   const unsigned char tail_color[4] = "\x0a\x56\xcf";
   const unsigned char link_color[4] = "\xba\xba\xba";
   const unsigned char printed_color[4] = "\x94\xbd\xff";
   const unsigned char bg_color[4] = "\xff\xff\xff";

   char filename[50];
   sprintf(filename, "images/img-%05d.ppm", index);
   FILE *fp = fopen(filename, "w");
   if (fp == NULL) {
      printf("unable to open '%s'\n", filename);
      exit(1);
   }

   fprintf(fp, "P6\n%d %d\n255\n", IMG_WIDTH, IMG_HEIGHT);

   char img[IMG_WIDTH * IMG_HEIGHT * 3];
   memset(img, '\xff', IMG_WIDTH * IMG_HEIGHT * 3);

   check_pixel_pos(head);
   memcpy(img + ((head.y * IMG_WIDTH + head.x) * 3), head_color, 3);

   for (int k=0; k<size-1; k++) {
      check_pixel_pos(long_tail[k]);
      memcpy(img + ((long_tail[k].y * IMG_WIDTH + long_tail[k].x) * 3), link_color, 3);
   }

   check_pixel_pos(long_tail[size-1]);
   memcpy(img + ((long_tail[size-1].y * IMG_WIDTH + long_tail[size-1].x) * 3), tail_color, 3);

   for (int k=0; k<positions->size; k++) {
      check_pixel_pos(positions->positions[k]);
      memcpy(img + ((positions->positions[k].y * IMG_WIDTH + positions->positions[k].x) * 3), printed_color, 3);
   }

   fwrite(img, 1, IMG_WIDTH * IMG_HEIGHT * 3, fp);
   fclose(fp);
}
