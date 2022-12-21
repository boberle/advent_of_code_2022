#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


struct blueprint_s {
   int id;
   int or;
   int cl;
   int ob_or;
   int ob_cl;
   int ge_or;
   int ge_ob;
};
typedef struct blueprint_s blueprint_t;


size_t parse_file(FILE *, blueprint_t[40]);
int explore(int or, int cl, int ob, int ge, int or_r, int cl_r, int ob_r, int ge_r, blueprint_t *blueprint, int time, int max_time);


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

   blueprint_t blueprints[40];
   size_t size = parse_file(fp, blueprints);
   fclose(fp);

   printf("number of blueprints: %lu\n", size);

   unsigned int total_p1 = 0;
   for (int i=0; i<size; i++) {
      int max = explore(0, 0, 0, 0, 1, 0, 0, 0, &blueprints[i], 0, 23);
      printf("id %d, max geodes opened: %d, quality: %d\n", blueprints[i].id, max, blueprints[i].id * max);
      total_p1 += blueprints[i].id * max;
   }
   printf("total: %d (first part)\n", total_p1);

   unsigned int total_p2 = 1;
   for (int i=0; i<size; i++) {
      int max = explore(0, 0, 0, 0, 1, 0, 0, 0, &blueprints[i], 0, 31);
      printf("id %d, max geodes opened: %d\n", blueprints[i].id, max);
      total_p2 *= max;
      if (i == 2)
         break;
   }
   printf("total: %d (second part)\n", total_p2);

   return 0;
}


size_t parse_file(FILE *fp, blueprint_t blueprints[40]) {
   size_t size = 0;
   int id, or, cl, ob_or, ob_cl, ge_or, ge_ob;
   char *pat = "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.\n";
   while (fscanf(fp, pat, &id, &or, &cl, &ob_or, &ob_cl, &ge_or, &ge_ob) != EOF) {
      blueprints[size++] = (blueprint_t){id, or, cl, ob_or, ob_cl, ge_or, ge_ob};
   }
   return size;
}



int explore(int or, int cl, int ob, int ge, int or_r, int cl_r, int ob_r, int ge_r, blueprint_t *blueprint, int time, int max_time) {

   //if (time == 23)
   if (time == max_time)
      return ge + ge_r;

   int max = ge;
   bool robot = false;

   // create robots
   if (time < 15 && or >= blueprint->or) {
      robot = true;
      int ans = explore(or + or_r - blueprint->or, cl + cl_r, ob + ob_r, ge + ge_r, or_r + 1, cl_r, ob_r, ge_r, blueprint, time + 1, max_time);
      if (ans > max)
         max = ans;
   }
   if (time < 20 && or >= blueprint->cl) {
      robot = true;
      int ans = explore(or + or_r - blueprint->cl, cl + cl_r, ob + ob_r, ge + ge_r, or_r, cl_r + 1, ob_r, ge_r, blueprint, time + 1, max_time);
      if (ans > max)
         max = ans;
   }
   if (or >= blueprint->ob_or && cl >= blueprint->ob_cl) {
      robot = true;
      int ans = explore(or + or_r - blueprint->ob_or, cl + cl_r - blueprint->ob_cl, ob + ob_r, ge + ge_r, or_r, cl_r, ob_r + 1, ge_r, blueprint, time + 1, max_time);
      if (ans > max)
         max = ans;
   }
   if (or >= blueprint->ge_or && ob >= blueprint->ge_ob) {
      robot = true;
      int ans = explore(or + or_r - blueprint->ge_or, cl + cl_r, ob + ob_r - blueprint->ge_ob, ge + ge_r, or_r, cl_r, ob_r, ge_r + 1, blueprint, time + 1, max_time);
      if (ans > max)
         max = ans;
   }

   if ((time < 20 || !robot)) {
      int ans = explore(or + or_r, cl + cl_r, ob + ob_r, ge + ge_r, or_r, cl_r, ob_r, ge_r, blueprint, time + 1, max_time);
      if (ans > max)
         max = ans;
   }

   return max;

}
