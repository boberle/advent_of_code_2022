#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>


#define MAX_SENSORS 100

//#define TEST_MODE

#define SEARCH_SPACE_START_RANGE 0

#ifdef TEST_MODE
   #define SEARCH_LINE 10
   #define SEARCH_SPACE_END_RANGE 20
#else
   #define SEARCH_LINE 2000000
   #define SEARCH_SPACE_END_RANGE 4000000
#endif


struct sensor_s {
   int x;
   int y;
   int d;
   int bx;
   int by;
};
typedef struct sensor_s sensor_t;


int load_sensors(FILE *, sensor_t[MAX_SENSORS]);
void get_first_and_last_pos(int size, sensor_t[size], int *, int*);
int get_coverage(int size, sensor_t[size], int, int, int);
unsigned long long find_beacon(int size, sensor_t sensors[size], int, int);


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

   sensor_t sensors[MAX_SENSORS];
   int size = load_sensors(fp, sensors);
   fclose(fp);
   printf("nb of sensors: %d\n", size);

   int start, end;
   get_first_and_last_pos(size, sensors, &start, &end);
   printf("start: %d, end: %d\n", start, end);

   int coverage = get_coverage(size, sensors, SEARCH_LINE, start, end);
   printf("coverage (first part): %d\n", coverage);

   unsigned long long tuning_freq = find_beacon(size, sensors, SEARCH_SPACE_START_RANGE, SEARCH_SPACE_END_RANGE);
   printf("coverage (second part): %llu\n", tuning_freq);

   return 0;
}



int load_sensors(FILE *fp, sensor_t sensors[MAX_SENSORS]) {
   int size = 0;
   int sx, sy, bx, by;
   while (fscanf(fp, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d\n", &sx, &sy, &bx, &by) != EOF) {
      sensor_t sensor = {sx, sy, abs(bx-sx) + abs(by-sy), bx, by};
      sensors[size++] = sensor;
   }
   return size;
}


void get_first_and_last_pos(int size, sensor_t sensors[size], int *min, int *max) {
   *min = INT_MAX;
   *max = INT_MIN;
   for (int i=0; i<size; i++) {
      if (sensors[i].x - sensors[i].d < *min)
         *min = sensors[i].x - sensors[i].d;
      if (sensors[i].x + sensors[i].d > *max)
         *max = sensors[i].x + sensors[i].d;
   }
}


int get_coverage(int size, sensor_t sensors[size], int line, int start, int end) {
   bool data[end-start+1];
   for (int x=start; x<=end; x++) {
      bool found = false;
      for (int i=0; i<size; i++) {
         sensor_t *s = &sensors[i];
         if ((s->x == x && s->y == line) || (s->bx == x && s->by == line))
            break;
         int dist = abs(x - s->x) + abs(line - s->y);
         if (dist <= s->d) {
            found = true;
            break;
         }
      }
      data[x-start] = found;
   }
   int total = 0;
   for (int i=0; i<end-start+1; i++)
      if (data[i])
         total++;
   return total;
}


unsigned long long find_beacon(int size, sensor_t sensors[size], int start, int end) {
   for (int s=0; s<size; s++) {
      sensor_t *sensor = &sensors[s];

      int sensor_start = sensor->y - sensor->d - 1;
      int sensor_end = sensor->y + sensor->d + 1;
      sensor_start = sensor_start < start ? start : sensor_start;
      sensor_end = sensor_end > end ? end : sensor_end;

      int r = sensor->d + 1;

      for (int y=sensor_start; y<=sensor_end; y++) {
         int d = abs(y - sensor->y);
         if (d > r) {
            puts("we have a problem");
            exit(1);
         }
         int xs[2] = {sensor->x - (r - d), sensor->x + (r - d)};
         for (int i=0; i<2; i++) {
            int x = xs[i];
            if (x < start || x > end)
               continue;
            bool covered = false;
            for (int t=0; t<size; t++) {
               if (s == t)
                  continue;
               sensor_t *s2 = &sensors[t];
               if ((x == s2->x && y == s2->y) || (x == s2->bx && y == s2->by) || abs(x - s2->x) + abs(y - s2->y) <= s2->d) {
                  covered = true;
                  break;
               }
            }
            if (!covered) {
               printf("x=%d, y=%d\n", x, y);
               //return (unsigned long long) x * 4000000 + y;
               // or:
               return x * 4000000llu + y;
            }
         }

      }
   }
}
