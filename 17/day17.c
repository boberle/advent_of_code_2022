#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


struct point_s {
   int x;
   int y;
};
typedef struct point_s point_t;


struct rock_s {
   point_t mat[5];
   size_t size;
};
typedef struct rock_s rock_t;


struct map_s {
   point_t mat[50000];
   size_t size;
   rock_t rock;
   int floor;
};
typedef struct map_s map_t;


struct state_s {
   char jet_pattern[12000];
   char *next_jet;
   unsigned char next_rock;
};
typedef struct state_s state_t;



void appear_rock(map_t *map, state_t *state);
void move_rock_gas(map_t *map, state_t *state);
bool move_rock_down(map_t *map, state_t *state);
bool can_move_rock(map_t *map, int x, int y);
void rock_factory(rock_t *rock, int type);
void move_rock(rock_t *rock, int x, int y);
int cmp(point_t *a, point_t *b);
void print(map_t *map);
void find_period(int size, int arr[size], int *init_size, int *period_size);
unsigned long long sum(int size, int arr[size]);



int main(int argc, char **argv) {

   //int foo[20] = {10, 11,12, 1, 2, 1, 2, 3, 4, 4, 1, 2, 3, 4, 4};
   //int size = 15;
   //int init_size, period_size;
   //find_period(size, foo, &init_size, &period_size);
   //printf("%d, %d\n", init_size, period_size);
   //exit(1);

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

   map_t map = {0};
   map.floor = -1;
   state_t state = {0};

   if (fgets(state.jet_pattern, 12000, fp) == NULL) {
      puts("unable to read file");
      exit(1);
   }

   if (state.jet_pattern[strlen(state.jet_pattern)-1] == '\n')
      state.jet_pattern[strlen(state.jet_pattern)-1] = '\0';

   state.next_jet = state.jet_pattern;

   fclose(fp);

   for (int i=0; i<2022; i++) {
      appear_rock(&map, &state);
      //print(&map);
      do {
         move_rock_gas(&map, &state);
      } while(move_rock_down(&map, &state));
   }

   printf("floor is %d, so rocks are %d units tall (first part)\n", map.floor, map.floor+1);


   map_t map2 = {0};
   map2.floor = -1;
   state_t state2 = {0};
   strcpy(state2.jet_pattern, state.jet_pattern);
   state2.next_jet = state2.jet_pattern;

   int steps[5000];
   int last = map2.floor;
   int step_size = 0;
   int init_size, period_size, test_init_size, last_period_size;
   while (1) {
      appear_rock(&map2, &state2);
      steps[step_size++] = map2.floor - last;
      find_period(step_size, steps, &test_init_size, &period_size);
      if (period_size && last_period_size == period_size)
         break;
      last_period_size = period_size;
      init_size = test_init_size;
      last = map2.floor;
      do {
         move_rock_gas(&map2, &state2);
      } while(move_rock_down(&map2, &state2));
   }

   //printf("%d, %d\n", init_size, period_size);

   const unsigned long long n = 1000000000000llu;
   //const unsigned long long n = 2022llu;
   unsigned long long a = sum(init_size, steps);
   unsigned long long b = ((n - init_size) / period_size) * sum(period_size, steps + init_size);
   unsigned long long c = sum(((n - init_size) % period_size) + 1, steps + init_size);
   printf("rocks are %llu units tall (second part)\n", a + b + c);

   return 0;
}


unsigned long long sum(int size, int arr[size]) {
   unsigned long long total = 0;
   for (int i=0; i<size; i++)
      total += arr[i];
   return total;
}


void find_period(int size, int arr[size], int *init_size, int *period_size) {
   *period_size = 0;
   *init_size = 0;
   for (int i=30; i<size/2; i++) {
      //printf("%d - %d... ", size-2*i, size-i);
      if (memcmp(arr+size-2*i, arr+size-i, sizeof(int) * i) == 0) {
         //printf("yes\n");
         *period_size = i;
         *init_size = size - 2 * i;
      }
      //else
         //printf("no\n");
   }
   //puts("quit");
}


void appear_rock(map_t *map, state_t *state) {
   rock_factory(&map->rock, state->next_rock++);
   state->next_rock %= 5;
   move_rock(&map->rock, 2, map->floor + 4);
}


// TODO
int foo = 0;
int bar = 0;


void move_rock_gas(map_t *map, state_t *state) {
   if (*state->next_jet == '>') {
      if (can_move_rock(map, 1, 0))
         move_rock(&map->rock, 1, 0);
   } else if (*state->next_jet == '<') {
      if (can_move_rock(map, -1, 0))
         move_rock(&map->rock, -1, 0);
   } else {
      printf("invalid jet direction: %c\n", *state->next_jet);
      exit(1);
   }

   bar++; // TODO
   if (!*(++state->next_jet)) {
      // TODO
      //printf("floor: %d - %d - %d\n", map->floor, map->floor - foo, bar);
      foo = map->floor;
      bar = 0;
      state->next_jet = state->jet_pattern;
   }
}


bool move_rock_down(map_t *map, state_t *state) {
   if (can_move_rock(map, 0, -1)) {
      move_rock(&map->rock, 0, -1);
      return true;
   } else {
      int max_y = map->floor;
      for (int i=0; i<map->rock.size; i++) {
         map->mat[map->size++] = map->rock.mat[i];
         if (map->rock.mat[i].y > max_y)
            max_y = map->rock.mat[i].y;
      }
      map->floor = max_y;
      return false;
   }
}


bool can_move_rock(map_t *map, int x, int y) {
   rock_t rock = map->rock;
   move_rock(&rock, x, y);
   for (int i=0; i<rock.size; i++) {
      if (rock.mat[i].y < 0 || rock.mat[i].x < 0 || rock.mat[i].x > 6)
         return false;
      for (int j=0; j<map->size; j++) {
         if (rock.mat[i].x == map->mat[j].x && rock.mat[i].y == map->mat[j].y)
            return false;
      }
   }
   return true;
}


void rock_factory(rock_t *rock, int type) {
   if (type == 0) {
      rock->size = 4;
      rock->mat[0] = (point_t){0, 0};
      rock->mat[1] = (point_t){1, 0};
      rock->mat[2] = (point_t){2, 0};
      rock->mat[3] = (point_t){3, 0};
   } else if (type == 1) {
      rock->size = 5;
      rock->mat[0] = (point_t){1, 2};
      rock->mat[1] = (point_t){0, 1};
      rock->mat[2] = (point_t){1, 1};
      rock->mat[3] = (point_t){2, 1};
      rock->mat[4] = (point_t){1, 0};
   } else if (type == 2) {
      rock->size = 5;
      rock->mat[0] = (point_t){2, 2};
      rock->mat[1] = (point_t){2, 1};
      rock->mat[2] = (point_t){0, 0};
      rock->mat[3] = (point_t){1, 0};
      rock->mat[4] = (point_t){2, 0};
   } else if (type == 3) {
      rock->size = 4;
      rock->mat[0] = (point_t){0, 3};
      rock->mat[1] = (point_t){0, 2};
      rock->mat[2] = (point_t){0, 1};
      rock->mat[3] = (point_t){0, 0};
   } else if (type == 4) {
      rock->size = 4;
      rock->mat[0] = (point_t){0, 1};
      rock->mat[1] = (point_t){1, 1};
      rock->mat[2] = (point_t){0, 0};
      rock->mat[3] = (point_t){1, 0};
   }
}


void move_rock(rock_t *rock, int x, int y) {
   for (int i=0; i<rock->size; i++) {
      rock->mat[i].x += x;
      rock->mat[i].y += y;
   }
}


int cmp(point_t *a, point_t *b) {
   if (a->y == b->y)
      return a->x < b->x ? -1 : a->x > b->x ? 1 : 0;
   return a->y < b->y ? -1 : a->y > b->y ? 1 : 0;
}


void print(map_t *map) {
   //point_t mat[map->size];
   //memcpy(mat, map->mat, sizeof(point_t) * map->size);
   //qsort(mat, map->size, sizeof(point_t), cmp);

   point_t rock_mat[map->rock.size];
   memcpy(rock_mat, map->rock.mat, sizeof(point_t) * map->rock.size);
   qsort(rock_mat, map->rock.size, sizeof(point_t), cmp);

   int line_no = rock_mat[map->rock.size-1].y + 1;
   printf("line no: %d\n", line_no);
   //int last_line = -1;
   //char line[15];
   //for (int i=map->rock.size-1; i>=0; i--) {
   //   if (last_line < 0
   //   sprintf(line, "% 5d .......", line_no);
   //}

   char disp[line_no][14];
   for (int i=0; i<line_no; i++)
      sprintf(disp[i], "....... % 5d", i);

   for (int i=0; i<map->size; i++)
      disp[map->mat[i].y][map->mat[i].x] = '#';

   for (int i=0; i<map->rock.size; i++)
      disp[map->rock.mat[i].y][map->rock.mat[i].x] = '@';

   for (int i=line_no-1; i>=0; i--)
      printf("%s\n", disp[i]);

}
