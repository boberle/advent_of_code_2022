#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define BUF_LEN 1024
#define MAX_HEIGHT 500
#define MAX_WIDTH 500


struct point_s {
   int x;
   int y;
};
typedef struct point_s point_t;

enum direction_e {North, South, West, East};
typedef enum direction_e direction_t;


void parse_file(FILE *, bool[MAX_HEIGHT][MAX_WIDTH], size_t *width, size_t *height);
int play(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int rounds);
unsigned long play2(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH]);
bool is_elf_alone(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int x, int y);
int propose_move(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int move_counts[h][w], point_t moves[h][w], direction_t first_dir);
void move(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int move_counts[h][w], point_t moves[h][w]);
int count_empty_tiles(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH]);
void print_map(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH]);


int main(int argc, char **argv) {
   
   if (argc != 2) {
      puts("missing filename");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("unable to open '%s'\n", filename);
      exit(1);
   }

   size_t width, height;
   bool map[MAX_HEIGHT][MAX_WIDTH] = {false};
   parse_file(fp, map, &width, &height);

   printf("map size: %ld x %ld\n", width, height);

   //int empty_tiles = play(width, height, map, 10);
   //printf("empty tiles: %d (first part)\n", empty_tiles);

   unsigned long rounds = play2(width, height, map);
   printf("rounds: %lu (second part)\n", rounds);

   fclose(fp);

   return 0;

}



void parse_file(FILE *fp, bool map[MAX_HEIGHT][MAX_WIDTH], size_t *width, size_t *height) {
   *height = 0;

   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != NULL) {
      int len = strlen(buf) - 1;
      for (int i=0; i<len; i++) {
         map[*height + 150][i + 150] = buf[i] == '#';
      }
      *width = len;
      (*height)++;
   }
   *width += 300;
   *height += 300;
}


int play(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int rounds) {
   direction_t dir = North;
   //print_map(w, h, map);
   for (int i=0; i<rounds; i++) {
      int move_counts[h][w];
      memset(move_counts, 0, sizeof(int) * w * h);
      point_t moves[h][w];
      propose_move(w, h, map, move_counts, moves, dir);
      move(w, h, map, move_counts, moves);
      dir = (dir + 1) % 4;
      //printf("=== %d ===\n", i);
      //print_map(w, h, map);
   }
   int empty_tiles = count_empty_tiles(w, h, map);
   return empty_tiles;
}


unsigned long play2(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH]) {
   direction_t dir = North;
   unsigned long counter = 1;
   while (1) {
      int move_counts[h][w];
      memset(move_counts, 0, sizeof(int) * w * h);
      point_t moves[h][w];
      int move_count = propose_move(w, h, map, move_counts, moves, dir);
      if (!move_count)
         break;
      move(w, h, map, move_counts, moves);
      dir = (dir + 1) % 4;
      counter++;
   }
   return counter;
}


bool is_elf_alone(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int x, int y) {
   w--;
   h--;
   return !(
      (x > 0 && y > 0 && map[y-1][x-1])
      || (y > 0 && map[y-1][x])
      || (y > 0 && x < w && map[y-1][x+1])
      || (x < w && map[y][x+1])
      || (y < h && x < w && map[y+1][x+1])
      || (y < h && map[y+1][x])
      || (y < h && x > 0 && map[y+1][x-1])
      || (x > 0 && map[y][x-1])
   );
}


int propose_move(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int move_counts[h][w], point_t moves[h][w], direction_t first_dir) {
   int max_x = w - 1;
   int max_y = h - 1;

   int total_move = 0;

   for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {

         if (!map[y][x] || is_elf_alone(w, h, map, x, y))
            continue;

         for (int i=0; i<4; i++) {
            direction_t dir = (first_dir + i) % 4;
            point_t proposed = {-1, -1};

            switch (dir) {
               case North:
                  if (y > 0 && !map[y-1][x] && (x == 0 || !map[y-1][x-1]) && (x == max_x || !map[y-1][x+1]))
                     proposed = (point_t){x, y-1};
                  break;
               case East:
                  if (x < max_x && !map[y][x+1] && (y == 0 || !map[y-1][x+1]) && (y == max_y || !map[y+1][x+1]))
                     proposed = (point_t){x+1, y};
                  break;
               case South:
                  if (y < max_y && !map[y+1][x] && (x == 0 || !map[y+1][x-1]) && (x == max_x || !map[y+1][x+1]))
                     proposed = (point_t){x, y+1};
                  break;
               case West:
                  if (x > 0 && !map[y][x-1] && (y == 0 || !map[y-1][x-1]) && (y == max_y || !map[y+1][x-1]))
                     proposed = (point_t){x-1, y};
                  break;
            }

            if (proposed.x != -1) {
               move_counts[proposed.y][proposed.x]++;
               moves[proposed.y][proposed.x] = (point_t){x, y};
               total_move++;
               break;
            }

         }
      }
   }
   return total_move;
}


void move(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH], int move_counts[h][w], point_t moves[h][w]) {
   // check
   for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {
         if (move_counts[y][x] == 1) {
            if (map[y][x]) {
               printf("already an elf at (%d,%d)\n", x, y);
               exit(1);
            }
         }
      }
   }

   for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {
         if (move_counts[y][x] == 1) {
            int src_x = moves[y][x].x;
            int src_y = moves[y][x].y;
            map[y][x] = true;
            map[src_y][src_x] = false;
         }
      }
   }
}


int count_empty_tiles(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH]) {
   int max_x = 0, max_y = 0, min_x = w, min_y = h;
   for (int x=0; x<w; x++) {
      for (int y=0; y<h; y++) {
         if (map[y][x]) {
            if (x < min_x)
               min_x = x;
            if (y < min_y)
               min_y = y;
            if (x > max_x)
               max_x = x;
            if (y > max_y)
               max_y = y;
         }
      }
   }

   int total = 0;
   for (int x=min_x; x<=max_x; x++) {
      for (int y=min_y; y<=max_y; y++) {
         if (!map[y][x])
            total++;
      }
   }

   return total;
}


void print_map(size_t w, size_t h, bool map[MAX_HEIGHT][MAX_WIDTH]) {
   for (int y=0; y<h; y++) {
      for (int x=0; x<w; x++) {
         printf("%c", map[y][x] ? '#' : '.');
      }
      printf("\n");
   }
}
