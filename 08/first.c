#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define MAX_LEN 200

struct grid_s {
   char grid[MAX_LEN][MAX_LEN];
   int width;
   int height;
};
typedef struct grid_s grid_t;


void read_file(FILE *, grid_t *);
int count_visible_trees_n_get_best_score(grid_t *, int *);
bool is_visible(grid_t *, int, int);
int get_scenic_score(grid_t *, int, int);


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

   grid_t grid;
   read_file(fp, &grid);

   int scenic_score;
   int visible_tree_count = count_visible_trees_n_get_best_score(&grid, &scenic_score);
   printf("visible trees (first part): %d\n", visible_tree_count);
   printf("best scenic score (second part): %d\n", scenic_score);

   fclose(fp);

   return 0;

}


void read_file(FILE *fp, grid_t *grid) {
   char buf[MAX_LEN + 2];
   while (fgets(buf, MAX_LEN + 2, fp) != NULL) {
      if (buf[strlen(buf)-1] == '\n')
         buf[strlen(buf)-1] = '\0';
      if (grid->width == 0)
         grid->width = strlen(buf);
      strcpy(grid->grid[grid->height], buf);
      for (int i=0; i<grid->width; i++) {
         grid->grid[grid->height][i] -= 48;
      }
      grid->height++;
   }
}


int count_visible_trees_n_get_best_score(grid_t *grid, int *scenic_score) {
   int counter = 0;
   *scenic_score = 0;
   for (int i=0; i<grid->height; i++) {
      for (int j=0; j<grid->width; j++) {
         if (is_visible(grid, i, j))
            counter++;
         int score = get_scenic_score(grid, i, j);
         if (score > *scenic_score)
            *scenic_score = score;
      }
   }
   return counter;
}


bool is_visible(grid_t *grid, int row, int col) {
   if (row == 0 || col == 0 || row == grid->height-1 || col == grid->width-1)
      return true;

   for (int i=row-1; i>=0; i--) {
      if (grid->grid[i][col] >= grid->grid[row][col])
         break;
      if (i == 0)
         return true;
   }
   for (int i=row+1; i<grid->height; i++) {
      if (grid->grid[i][col] >= grid->grid[row][col])
         break;
      if (i == grid->height-1)
         return true;
   }
   for (int i=col-1; i>=0; i--) {
      if (grid->grid[row][i] >= grid->grid[row][col])
         break;
      if (i == 0)
         return true;
   }
   for (int i=col+1; i<grid->width; i++) {
      if (grid->grid[row][i] >= grid->grid[row][col])
         break;
      if (i == grid->width-1)
         return true;
   }
   return false;
}


int get_scenic_score(grid_t *grid, int row, int col) {
   int left = 0;
   int right = 0;
   int up = 0;
   int down = 0;
   for (int i=row-1; i>=0; i--) {
      up++;
      if (grid->grid[i][col] >= grid->grid[row][col])
         break;
   }
   for (int i=row+1; i<grid->height; i++) {
      down++;
      if (grid->grid[i][col] >= grid->grid[row][col])
         break;
   }
   for (int i=col-1; i>=0; i--) {
      left++;
      if (grid->grid[row][i] >= grid->grid[row][col])
         break;
   }
   for (int i=col+1; i<grid->width; i++) {
      right++;
      if (grid->grid[row][i] >= grid->grid[row][col])
         break;
   }
   return left * right * up * down;
}
