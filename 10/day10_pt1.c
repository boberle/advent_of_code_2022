#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_LEN 10


int compute_signal_strengths(FILE *);
int compute_signal_strength(int, int);


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

   int strength = compute_signal_strengths(fp);
   printf("strength is: %d\n", strength);

   fclose(fp);

   return 0;
}


int compute_signal_strength(int cycle, int regx) {
   if (cycle == 20 || cycle == 60 || cycle == 100 || cycle == 140 || cycle == 180 || cycle == 220) {
      return cycle * regx;
   }
   return 0;
}


int compute_signal_strengths(FILE *fp) {

   char buf[BUF_LEN];
   int cycle_counter = 1;
   int x = 1;
   int total = 0;

   while (fgets(buf, BUF_LEN, fp) != NULL) {
      buf[strlen(buf)-1] = '\0';
      if (strcmp(buf, "noop") == 0) {
         total += compute_signal_strength(cycle_counter, x);
         cycle_counter++;
      } else if (memcmp(buf, "addx", 4) == 0) {
         total += compute_signal_strength(cycle_counter, x);
         cycle_counter++;
         total += compute_signal_strength(cycle_counter, x);
         cycle_counter++;
         x += atoi(buf + 5);
      } else {
         printf("invalid operation: %s\n", buf);
         exit(1);
      }
   }

   return total;
}
