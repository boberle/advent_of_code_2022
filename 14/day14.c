#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUF_LEN 1024
#define MAX 1000


void place_rocks(FILE *, char[MAX][MAX], int *);
void place_floor(char[MAX][MAX], int);
int pour_sand(char[MAX][MAX], int);
void export_grid(char[MAX][MAX]);


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

   char map1[MAX][MAX] = {0};
   int height;
   place_rocks(fp, map1, &height);

   char map2[MAX][MAX];
   memcpy(map2, map1, MAX * MAX);

   int ans1 = pour_sand(map1, height);
   printf("total (first part): %d\n", ans1);

   height += 2;
   place_floor(map2, height);
   int ans2 = pour_sand(map2, height);
   printf("total (second part): %d\n", ans2);

   fclose(fp);

   return 0;
}



void place_rocks(FILE *fp, char map[MAX][MAX], int *height) {

   *height = 0;

   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != NULL) {
      int src_x = -1, src_y = -1;
      int dest_x, dest_y;
      char *p = buf;

      while (sscanf(p, "%d,%d", &dest_x, &dest_y)) {

         if (src_x >= 0) {
            if (src_x == dest_x) {
               int start = src_y <= dest_y ? src_y : dest_y;
               int end = src_y > dest_y ? src_y : dest_y;
               if (end > *height)
                  *height = end;
               for (int y=start; y<=end; y++)
                  map[y][src_x] = 'x';
            } else if (src_y == dest_y) {
               int start = src_x <= dest_x ? src_x : dest_x;
               int end = src_x > dest_x ? src_x : dest_x;
               for (int x=start; x<=end; x++)
                  map[src_y][x] = 'x';

            }
         }

         src_x = dest_x;
         src_y = dest_y;

         char *c = strchr(p, '>');
         if (c == NULL)
            break;
         p = c + 2;
      }
   }

}


void place_floor(char map[MAX][MAX], int height) {
   for (int x=0; x<MAX; x++) {
      map[height][x] = 'x';
   }
}


int pour_sand(char map[MAX][MAX], int height) {

   int total_sand = 0;

   while (1) {
      int x = 500, y = 0;

      if (map[y][x] == 'o')
         break;

      while (1) {
         if (y >= height)
            break;

         if (!map[y+1][x]) {
            y++;
         } else if (!map[y+1][x-1]) {
            y++;
            x--;
         } else if (!map[y+1][x+1]) {
            y++;
            x++;
         } else {
            map[y][x] = 'o';
            total_sand++;
            break;
         }
      }
      if (y >= height)
         break;
   }

   return total_sand;
}


void export_grid(char map[MAX][MAX]) {
   for (int j=0; j<MAX; j++) {
      for (int i=0; i<MAX; i++) {
         if (map[j][i])
            printf("%c", map[j][i]);
         else
            printf(" ");
      }
      printf("\n");
   }
}
