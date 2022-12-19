#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>


struct point_s {
   int x;
   int y;
   int z;
};
typedef struct point_s point_t;


size_t parse_file(FILE *, point_t[3000]);
unsigned int count_non_connected_sides(size_t size, const point_t[size]);
void explore(int x, int y, int z, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z, size_t size, const point_t points[size], point_t masks[3000], size_t *mask_size);
unsigned int find_exposed_area(size_t size, const point_t points[size]);


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

   point_t points[3000];
   size_t size = parse_file(fp, points);

   unsigned int non_connected_sides = count_non_connected_sides(size, points);
   printf("first part: nb of non connected sides %d (size %lu)\n", non_connected_sides, size);

   fclose(fp);

   unsigned int exposed_area = find_exposed_area(size, points);
   printf("second part: exposed area: %d\n", exposed_area);

   return 0;
}


size_t parse_file(FILE *fp, point_t points[3000]) {
   size_t size = 0;
   int x, y, z;
   while (fscanf(fp, "%d,%d,%d\n", &x, &y, &z) != EOF) {
      points[size++] = (point_t){x, y, z};
   }
   return size;
}


unsigned int count_non_connected_sides(size_t size, const point_t points[size]) {
   unsigned int sides = 0;
   for (size_t i=0; i<size; i++) {
      int connected = 0;
      for (int j=0; j<size; j++) {
         if ( (points[i].x == points[j].x && points[i].y == points[j].y && abs(points[i].z - points[j].z) == 1)
           || (points[i].x == points[j].x && points[i].z == points[j].z && abs(points[i].y - points[j].y) == 1)
           || (points[i].y == points[j].y && points[i].z == points[j].z && abs(points[i].x - points[j].x) == 1) )
            connected++;
      }
      sides += 6 - connected;
   }
   return sides;
}


unsigned int find_exposed_area(size_t size, const point_t points[size]) {

   // find the ranges
   int min_x = INT_MAX, max_x = INT_MIN;
   int min_y = INT_MAX, max_y = INT_MIN;
   int min_z = INT_MAX, max_z = INT_MIN;

   for (size_t i=0; i<size; i++) {
      if (points[i].x < min_x) min_x = points[i].x;
      if (points[i].x > max_x) max_x = points[i].x;
      if (points[i].y < min_y) min_y = points[i].y;
      if (points[i].y > max_y) max_y = points[i].y;
      if (points[i].z < min_z) min_z = points[i].z;
      if (points[i].z > max_z) max_z = points[i].z;
   }
   min_x--; max_x++;
   min_y--; max_y++;
   min_z--; max_z++;

   // compute all mask cubes
   point_t masks[10000];
   size_t mask_size = 0;
   explore(min_x, min_y, min_z, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, &mask_size);

   // do some math
   unsigned int non_connected_sides = count_non_connected_sides(mask_size, masks);
   unsigned int a = max_x - min_x + 1;
   unsigned int b = max_y - min_y + 1;
   unsigned int c = max_z - min_z + 1;
   unsigned int area = 2 * (a*b + b*c + a*c);
   //printf("DEBUG: x:[%d, %d], y:[%d, %d], z:[%d, %d]\n", min_x, max_x, min_y, max_y, min_z, max_z);
   //printf("DEBUG: a=%u, b=%u, c=%u\n", a, b, c);
   //printf("DEBUG: non conn: %u, area: %u\n", non_connected_sides, area);
   return non_connected_sides - area;

}


void explore(int x, int y, int z, int min_x, int max_x, int min_y, int max_y, int min_z, int max_z, size_t size, const point_t points[size], point_t masks[10000], size_t *mask_size) {
   if (x < min_x || x > max_x || y < min_y || y > max_y || z < min_z || z > max_z)
      return;
   for (size_t i=0; i<size; i++)
      if (x == points[i].x && y == points[i].y && z == points[i].z)
         return;
   for (size_t i=0; i<*mask_size; i++)
      if (x == masks[i].x && y == masks[i].y && z == masks[i].z)
         return;

   masks[(*mask_size)++] = (point_t){x, y, z};

   explore(x-1, y, z, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, mask_size);
   explore(x+1, y, z, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, mask_size);
   explore(x, y-1, z, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, mask_size);
   explore(x, y+1, z, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, mask_size);
   explore(x, y, z-1, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, mask_size);
   explore(x, y, z+1, min_x, max_x, min_y, max_y, min_z, max_z, size, points, masks, mask_size);
}
