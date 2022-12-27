#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define BUF_LEN 1024
#define MAX_STEP_SIZE 5000
#define MAX_POINT_SIZE 20000


struct point_s {
   int x;
   int y;
   struct point_s *right;
   struct point_s *down;
   struct point_s *left;
   struct point_s *up;
   bool is_wall;
};
typedef struct point_s point_t;

typedef int step_t;

enum facing_e {Right = 0, Down = 1, Left = 2, Up = 3};
typedef enum facing_e facing_t;


void parse_file(FILE *, point_t[MAX_POINT_SIZE], point_t **, step_t[MAX_STEP_SIZE], size_t *);
int play(point_t *root, size_t step_size, step_t steps[step_size]);


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

   point_t points[MAX_POINT_SIZE];
   point_t *root;
   step_t steps[MAX_STEP_SIZE] = {0};
   size_t step_size = 0;
   parse_file(fp, points, &root, steps, &step_size);

   int password = play(root, step_size, steps);
   printf("password is (first part): %d\n", password);

   return 0;
}


void parse_file(FILE *fp, point_t points[MAX_STEP_SIZE], point_t **root, step_t steps[MAX_STEP_SIZE], size_t *step_size) {
   char buf[BUF_LEN];
   int y = 0;
   size_t point_size = 0;

   size_t col_count = 0;

   while (fgets(buf, BUF_LEN, fp) != NULL) {
      int len = strlen(buf);
      buf[--len] = '\0';
      if (len == 0)
         break;

      point_t *first = NULL;
      point_t *last = NULL;

      for (int x=0; x<len; x++) {
         if (buf[x] == '.' || buf[x] == '#') {
            points[point_size] = (point_t){ x, y, NULL, NULL, NULL, NULL, buf[x] == '#' };
            point_t *cur = &points[point_size];
            point_size++;

            if (first == NULL)
               first = cur;
            if (last != NULL && !last->is_wall && !cur->is_wall) {
               cur->left = last;
               last->right = cur;
            }
            last = cur;
         }

         if (x + 1 > col_count)
            col_count = x + 1;

      }
      if (!last->is_wall && !first->is_wall) {
         last->right = first;
         first->left = last;
      }

      y++;
   }

   *root = &points[0];

   for (int x=0; x<col_count; x++) {
      point_t *first = NULL;
      point_t *last = NULL;
      point_t *cur = NULL;
      for (int i=0; i<point_size; i++) {
         if (points[i].x == x) {
            cur = &points[i];
            if (first == NULL)
               first = cur;

            if (last != NULL && !last->is_wall && !cur->is_wall) {
               cur->up = last;
               last->down = cur;
            }
            last = cur;
         }
      }

      if (!first->is_wall && !last->is_wall) {
         first->up = last;
         last->down = first;
      }
   }

   int move;
   char direction;
   while (1) {
      if (fscanf(fp, "%d", &move) == EOF)
         break;
      steps[(*step_size)++] = move;
      if (fscanf(fp, "%c", &direction) == EOF || direction == '\n')
         break;
      if (direction != 'R' && direction != 'L') {
         printf("unknown direction '%c'\n", direction);
         exit(1);
      }
      steps[(*step_size)++] = direction == 'R' ? -1 : -2;
   }
}


int play(point_t *root, size_t step_size, step_t steps[step_size]) {
   facing_t facing = Right;
   point_t *cur = root;

   for (int i=0; i<step_size; i++) {
      step_t step = steps[i];

      // turning
      if (step < 0) {
         printf("turning: %d\n", step);
         if (step == -1)
            facing = (facing + 1) % 4;
         else
            facing = (facing - 1) % 4;

      // move in line
      } else {
         printf("move: %d, facing: %d (%d, %d)\n", step, facing, cur->x, cur->y);

         for (int m=0; m<step; m++) {
            switch (facing) {
               case Right:
                  if (cur->right != NULL)
                     cur = cur->right;
                  break;
               case Down:
                  if (cur->down != NULL)
                     cur = cur->down;
                  break;
               case Left:
                  if (cur->left != NULL)
                     cur = cur->left;
                  break;
               case Up:
                  if (cur->up != NULL)
                     cur = cur->up;
                  break;
            }
         }
         printf("x, y=     (%d, %d)\n", cur->x, cur->y);
      }
   }

   int password = 1000 * (cur->y + 1) + 4 * (cur->x + 1) + facing;
   return password;
}

