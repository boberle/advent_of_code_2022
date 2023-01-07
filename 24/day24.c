#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define BUF_LEN 1024
#define MAX_HEIGHT 25
#define MAX_WIDTH 160


enum dir_e {North, East, South, West};
typedef enum dir_e dir_t;


struct point_s {
   unsigned short x;
   unsigned short y;
};
typedef struct point_s point_t;


struct cloud_s {
   unsigned short x;
   unsigned short y;
   dir_t dir;
};
typedef struct cloud_s cloud_t;


struct map_s {
   size_t width;
   size_t height;
   cloud_t clouds[MAX_WIDTH * MAX_HEIGHT];
   size_t cloud_size;
   point_t points[MAX_WIDTH * MAX_HEIGHT];
   size_t point_size;
   point_t dest;
};
typedef struct map_s map_t;



void parse_file(FILE *, map_t *);
int move(map_t *);
size_t add_point(size_t size, point_t points[size], unsigned short x, unsigned short y);
int is_free(map_t *, unsigned short, unsigned short);
void update_cloud_positions(map_t *);
void print_map(map_t *);
void init_map_start_to_end(map_t *);
void init_map_end_to_start(map_t *);


int main(int argc, char **argv) {

   if (argc != 2) {
      puts("missing filename");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("unable to open file '%s'\n", filename);
      exit(1);
   }

   map_t map = {0};
   parse_file(fp, &map);
   unsigned long long total_time = 0;

   for (int i=0; i<3; i++) {
      if (i % 2 == 0)
         init_map_start_to_end(&map);
      else
         init_map_end_to_start(&map);

      unsigned long long time = 1;
      update_cloud_positions(&map); // time 1
      while (1) {
         time++;
         if (move(&map)) {
            total_time += time;
            printf("journey #%d, accessing destination at: %llu (total time: %llu)\n", i+1, time, total_time);
            break;
         }
      }
   }

   printf("total time of the 3 journeys: %llu\n", total_time);

   return 0;
}


void parse_file(FILE *fp, map_t *map) {
   char buf[BUF_LEN];
   map->cloud_size = 0;
   map->width = 0;
   map->height = 1; // wall + entrance

   while (fgets(buf, BUF_LEN, fp) != NULL) {

      if (!map->width)
         map->width = strlen(buf) - 3;

      if (buf[2] == '#')
         continue;
      
      int len = strlen(buf) - 2;
      for (int i=1; i<len; i++) {
         char c = buf[i];
         if (strchr("^>v<", c) != NULL)
            map->clouds[map->cloud_size++] = (cloud_t){ i-1, map->height, c == '^' ? North : c == '>' ? East : c == 'v' ? South : West };
      }

      map->height++;
   }

   map->height++; // wall + exit

}


void init_map_start_to_end(map_t *map) {
   map->points[0] = (point_t){0, 0};
   map->point_size = 1;
   map->dest = (point_t) {map->width-1, map->height-1};
}


void init_map_end_to_start(map_t *map) {
   map->points[0] = (point_t){map->width-1, map->height-1};
   map->point_size = 1;
   map->dest = (point_t) {0, 0};
}


int move(map_t *map) {
   update_cloud_positions(map);

   point_t points[MAX_WIDTH * MAX_HEIGHT];
   size_t size = 0;

   size_t w = map->width - 1;
   size_t h = map->height - 1;

   for (size_t i=0; i<map->point_size; i++) {
      point_t *point = &map->points[i];
      if (point->x > 0 && is_free(map, point->x - 1, point->y))
         size += add_point(size, points, point->x - 1, point->y);
      if (point->y > 0 && is_free(map, point->x, point->y - 1))
         size += add_point(size, points, point->x, point->y - 1);
      if (point->x < w && is_free(map, point->x + 1, point->y))
         size += add_point(size, points, point->x + 1, point->y);
      if (point->y < h && is_free(map, point->x, point->y + 1))
         size += add_point(size, points, point->x, point->y + 1);
      if (is_free(map, point->x, point->y))
         size += add_point(size, points, point->x, point->y);
   }

   if (size == 0) {
      fprintf(stderr, "no more points\n");
      exit(1);
   }

   for (size_t i=0; i<size; i++) {
      if (points[i].x == map->dest.x && points[i].y == map->dest.y)
         return 1;
   }

   map->point_size = size;
   memcpy(map->points, points, sizeof(point_t) * size);

   return 0;
}


size_t add_point(size_t size, point_t points[size], unsigned short x, unsigned short y) {
   for (size_t i=0; i<size; i++) {
      if (points[i].x == x && points[i].y == y) {
         return 0;
      }
   }

   points[size++] = (point_t){x, y};
   return 1;
}


int is_free(map_t *map, unsigned short x, unsigned short y) {
   // walls
   if ((y == 0 && x != 0) || (y == map->height-1 && x != map->width-1))
      return 0;

   // clouds
   for (size_t i=0; i<map->cloud_size; i++) {
      if (map->clouds[i].x == x && map->clouds[i].y == y)
         return 0;
   }
   return 1;
}


void update_cloud_positions(map_t *map) {
   size_t w = map->width - 1;
   size_t h = map->height - 2; // bottom wall
   for (size_t i=0; i<map->cloud_size; i++) {
      cloud_t *cloud = &map->clouds[i];
      switch (cloud->dir) {
         case North:
            cloud->y = cloud->y > 1 ? cloud->y - 1 : h; // top wall
            break;
         case South:
            cloud->y = cloud->y < h ? cloud->y + 1 : 1; // top wall
            break;
         case West:
            cloud->x = cloud->x > 0 ? cloud->x - 1 : w;
            break;
         case East:
            cloud->x = cloud->x < w ? cloud->x + 1 : 0;
            break;
      }
   }
}


void print_map(map_t *map) {
   char printed[map->height][map->width];
   memset(printed, '.', map->width * map->height);

   for (size_t i=0; i<map->cloud_size; i++) {
      printed[map->clouds[i].y][map->clouds[i].x] = 'c';
   }

   for (size_t y=0; y<map->height; y++) {
      fwrite(printed[y], sizeof(char), map->width, stdout);
      printf("\n");
   }
}

