#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define BUF_LEN 1024
#define MAX_STEP_SIZE 5000
#define MAX_POINT_SIZE 20000
#define MAX_MAP_WIDTH 200
#define MAX_MAP_HEIGHT 250


//#define TEST


struct point_s {
   int x;
   int y;
   struct point_s *right;
   struct point_s *down;
   struct point_s *left;
   struct point_s *up;
   bool is_wall;
   unsigned char face;
};
typedef struct point_s point_t;

typedef int step_t;

enum facing_e {Right = 0, Down = 1, Left = 2, Up = 3};
typedef enum facing_e facing_t;


struct connection_s {
   int a_start_x;
   int a_start_y;
   int a_end_y;
   int a_end_x;
   int b_start_x;
   int b_start_y;
   int b_end_x;
   int b_end_y;
   facing_t a_facing;
   facing_t b_facing;
   unsigned char a_face;
   unsigned char b_face;
};
typedef struct connection_s connection_t;


void parse_file(FILE *, point_t[MAX_POINT_SIZE], point_t **, step_t[MAX_STEP_SIZE], size_t *);
int play(point_t *root, size_t step_size, step_t steps[step_size]);
void set_connection(connection_t *, int, int, int, int, int, int, int, int, facing_t, facing_t, unsigned char, unsigned char);
void set_connections(connection_t[20], size_t *);
static inline facing_t change_facing(point_t *, point_t *, facing_t);


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
   printf("password is (second part): %d\n", password);

   return 0;
}


void parse_file(FILE *fp, point_t points[MAX_POINT_SIZE], point_t **root, step_t steps[MAX_STEP_SIZE], size_t *step_size) {
   char buf[BUF_LEN];
   int y = 0;
   size_t point_size = 0;

   size_t col_count = 0;

   point_t *map[MAX_MAP_HEIGHT][MAX_MAP_WIDTH] = {NULL};

   while (fgets(buf, BUF_LEN, fp) != NULL) {
      int len = strlen(buf);
      buf[--len] = '\0';
      if (len == 0)
         break;

      point_t *last = NULL;
      for (int x=0; x<len; x++) {
         if (buf[x] == '.' || buf[x] == '#') {
            points[point_size] = (point_t){ x, y, NULL, NULL, NULL, NULL, buf[x] == '#', 255 };
            point_t *cur = &points[point_size];
            point_size++;

            map[y][x] = cur;

            if (last != NULL && !last->is_wall && !cur->is_wall) {
               cur->left = last;
               last->right = cur;
            }
            last = cur;
         }
         if (x + 1 > col_count)
            col_count = x + 1;
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

   connection_t connections[20];
   size_t connection_size;
   set_connections(connections, &connection_size);

   for (size_t i=0; i<connection_size; i++) {
      int b_start_x = connections[i].b_start_x;
      int b_start_y = connections[i].b_start_y;
      int b_end_x = connections[i].b_end_x;
      int b_end_y = connections[i].b_end_y;

      size_t edge_size = abs(connections[i].a_start_x - connections[i].a_end_x) + abs(connections[i].a_start_y - connections[i].a_end_y) + 1;
      point_t *edge_a[edge_size];
      point_t *edge_b[edge_size];

      size_t c = 0;
      int start_x = connections[i].a_start_x;
      int start_y = connections[i].a_start_y;
      int end_y = connections[i].a_end_y;
      int end_x = connections[i].a_end_x;

      for (int x=start_x; start_x < end_x ? x <= end_x : x >= end_x; x += start_x < end_x ? 1 : -1) {
         for (int y=start_y; start_y < end_y ? y <= end_y : y >= end_y; y += start_y < end_y ? 1 : -1) {
            edge_a[c++] = map[y][x];
            if (map[y][x] == NULL) {
               fprintf(stderr, "node on edge not defined");
               exit(1);
            }
         }
      }
      if (c != edge_size) {
         fprintf(stderr, "incomplete edge: %ld %ld\n", c, edge_size);
         exit(1);
      }

      c = 0;
      start_x = connections[i].b_start_x;
      start_y = connections[i].b_start_y;
      end_y = connections[i].b_end_y;
      end_x = connections[i].b_end_x;
      for (int x=start_x; start_x < end_x ? x <= end_x : x >= end_x; x += start_x < end_x ? 1 : -1) {
         for (int y=start_y; start_y < end_y ? y <= end_y : y >= end_y; y += start_y < end_y ? 1 : -1) {
            edge_b[c++] = map[y][x];
            if (map[y][x] == NULL) {
               fprintf(stderr, "node on edge not defined");
               exit(1);
            }
         }
      }
      if (c != edge_size) {
         fprintf(stderr, "incomplete edge: %ld %ld\n", c, edge_size);
         exit(1);
      }

      for (int c=0; c<edge_size; c++) {

         point_t **a;
         switch (connections[i].a_facing) {
            case Up: a = &edge_a[c]->up; break;
            case Down: a = &edge_a[c]->down; break;
            case Left: a = &edge_a[c]->left; break;
            case Right: a = &edge_a[c]->right; break;
         }
         point_t **b;
         switch (connections[i].b_facing) {
            case Up: b = &edge_b[c]->up; break;
            case Down: b = &edge_b[c]->down; break;
            case Left: b = &edge_b[c]->left; break;
            case Right: b = &edge_b[c]->right; break;
         }

         if (!edge_a[c]->is_wall && !edge_b[c]->is_wall) {
            *a = edge_b[c];
            edge_a[c]->face = connections[i].a_face;

            *b = edge_a[c];
            edge_b[c]->face = connections[i].b_face;
         } else {
            *a = NULL;
            *b = NULL;
         }
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


void set_connection(connection_t *connection, 
      int a_start_x,
      int a_start_y,
      int a_end_x,
      int a_end_y,
      int b_start_x,
      int b_start_y,
      int b_end_x,
      int b_end_y,
      facing_t a_facing,
      facing_t b_facing,
      unsigned char a_face,
      unsigned char b_face
      ) {
   connection->a_start_x = a_start_x;
   connection->a_start_y = a_start_y;
   connection->a_end_x = a_end_x;
   connection->a_end_y = a_end_y;
   connection->b_start_x = b_start_x;
   connection->b_start_y = b_start_y;
   connection->b_end_x = b_end_x;
   connection->b_end_y = b_end_y;
   connection->a_facing = a_facing;
   connection->b_facing = b_facing;
   connection->a_face = a_face;
   connection->b_face = b_face;
}


void set_connections(connection_t connections[20], size_t *size) {

   #ifdef TEST
   //set_connection(&connections[0], 3, 4, 3, 7, 4, 4, 4, 7, Right, Left, 2, 3);
   //set_connection(&connections[1], 7, 4, 7, 7, 8, 4, 8, 7, Right, Left, 3, 4);
   //set_connection(&connections[2], 8, 3, 11, 3, 8, 4, 11, 4, Down, Up, 1, 4);
   //set_connection(&connections[3], 8, 7, 11, 7, 8, 8, 11, 8, Down, Up, 4, 5);
   //set_connection(&connections[4], 11, 8, 11, 11, 12, 8, 12, 11, Right, Left, 5, 6);

   //set_connection(&connections[5], 4, 4, 7, 4, 8, 0, 8, 3, Up, Left, 3, 1);
   //set_connection(&connections[6], 3, 4, 0, 4, 8, 0, 11, 0, Up, Up, 2, 1);
   //set_connection(&connections[7], 4, 7, 7, 7, 11, 8, 8, 8, Down, Left, 3, 5);
   //set_connection(&connections[8], 3, 7, 0, 7, 8, 11, 11, 11, Down, Down, 2, 5);
   //set_connection(&connections[9], 11, 4, 11, 7, 15, 8, 12, 8, Right, Up, 4, 6);
   //set_connection(&connections[10], 11, 3, 11, 0, 15, 8, 15, 11, Right, Right, 1, 6);
   //set_connection(&connections[11], 0, 7, 0, 4, 12, 11, 15, 11, Left, Down, 2, 6);

   //*size = 12;

   // in fact, the following connections are enough:

   set_connection(&connections[0], 4, 4, 7, 4, 8, 0, 8, 3, Up, Left, 3, 1);
   set_connection(&connections[1], 3, 4, 0, 4, 8, 0, 11, 0, Up, Up, 2, 1);
   set_connection(&connections[2], 4, 7, 7, 7, 11, 8, 8, 8, Down, Left, 3, 5);
   set_connection(&connections[3], 3, 7, 0, 7, 8, 11, 11, 11, Down, Down, 2, 5);
   set_connection(&connections[4], 11, 4, 11, 7, 15, 8, 12, 8, Right, Up, 4, 6);
   set_connection(&connections[5], 11, 3, 11, 0, 15, 8, 15, 11, Right, Right, 1, 6);
   set_connection(&connections[6], 0, 7, 0, 4, 12, 11, 15, 11, Left, Down, 2, 6);

   *size = 7;

   #else
   //set_connection(&connections[0], 50, 49, 99, 49, 50, 50, 99, 50, Down, Up, 1, 3);
   //set_connection(&connections[1], 50, 99, 99, 99, 50, 100, 99, 100, Down, Up, 3, 4);
   //set_connection(&connections[2], 99, 0, 99, 49, 100, 0, 100, 49, Right, Left, 1, 2);
   //set_connection(&connections[3], 49, 100, 49, 149, 50, 100, 50, 149, Right, Left, 5, 4);
   //set_connection(&connections[4], 0, 149, 49, 149, 0, 150, 49, 150, Down, Up, 5, 6);

   //set_connection(&connections[5], 50, 50, 50, 99, 0, 100, 49, 100, Left, Up, 3, 5);
   //set_connection(&connections[6], 50, 0, 50, 49, 0, 149, 0, 100, Left, Left, 1, 5);
   //set_connection(&connections[7], 100, 49, 149, 49, 99, 50, 99, 99, Down, Right, 2, 3);
   //set_connection(&connections[8], 149, 0, 149, 49, 99, 149, 99, 100, Right, Right, 2, 4);
   //set_connection(&connections[9], 50, 149, 99, 149, 49, 150, 49, 199, Down, Right, 4, 6);
   //set_connection(&connections[10], 100, 0, 149, 0, 0, 199, 49, 199, Up, Down, 2, 6);
   //set_connection(&connections[11], 50, 0, 99, 0, 0, 150, 0, 199, Up, Left, 1, 6);

   //*size = 12;

   // in fact, the following connections are enough:

   set_connection(&connections[0], 50, 50, 50, 99, 0, 100, 49, 100, Left, Up, 3, 5);
   set_connection(&connections[1], 50, 0, 50, 49, 0, 149, 0, 100, Left, Left, 1, 5);
   set_connection(&connections[2], 100, 49, 149, 49, 99, 50, 99, 99, Down, Right, 2, 3);
   set_connection(&connections[3], 149, 0, 149, 49, 99, 149, 99, 100, Right, Right, 2, 4);
   set_connection(&connections[4], 50, 149, 99, 149, 49, 150, 49, 199, Down, Right, 4, 6);
   set_connection(&connections[5], 100, 0, 149, 0, 0, 199, 49, 199, Up, Down, 2, 6);
   set_connection(&connections[6], 50, 0, 99, 0, 0, 150, 0, 199, Up, Left, 1, 6);

   *size = 7;
   #endif
}


int play(point_t *root, size_t step_size, step_t steps[step_size]) {
   facing_t facing = Right;
   point_t *cur = root;

   for (int i=0; i<step_size; i++) {
      step_t step = steps[i];

      // turning
      if (step < 0) {
         //printf("turning: %d\n", step);
         if (step == -1)
            facing = (facing + 1) % 4;
         else
            facing = (facing - 1) % 4;

      // move in line
      } else {
         //printf("move: %d, facing: %d (%d, %d)\n", step, facing, cur->x, cur->y);

         for (int m=0; m<step; m++) {
            point_t *last = cur;
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
            if (last != cur)
               facing = change_facing(last, cur, facing);
         }
         //printf("x, y=     (%d, %d)\n", cur->x, cur->y);
      }
   }

   int password = 1000 * (cur->y + 1) + 4 * (cur->x + 1) + facing;
   return password;
}


static inline facing_t change_facing(point_t *a, point_t *b, facing_t f) {
   if (a->face == 255 || b->face == 255)
      return f;
   if (a->face == b->face)
      return f;
   #ifdef TEST
   static facing_t facings[6][6] = {
      {-1, Down, Down, Down, -1, Left},
      {Down, -1, Right, -1, Up, Up},
      {Right, Left, -1, Right, Right, -1},
      {Up, -1, Left, -1, Down, Down},
      {-1, Up, Up, Up, -1, Right},
      {Left, Right, -1, Left, Left, -1},
   };
   #else
   static facing_t facings[6][6] = {
      {-1, Right, Down, -1, Right, Right},
      {Left, -1, Left, Left, -1, Up},
      {Up, Up, -1, Down, Down, -1},
      {-1, Left, Up, -1, Left, Left},
      {Right, -1, Right, Right, -1, Down},
      {Down, Down, -1, Up, Up, -1},
   };
   #endif
   facing_t new_f = facings[a->face - 1][b->face - 1];
   if (new_f == -1) {
      printf("incompatible faces from %d to %d\n", a->face, b->face);
      exit(1);
   }
   return new_f;
}
