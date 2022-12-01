#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define BUF_LEN 100


void update_top3(int *top3, int val) {
   if (val > top3[0]) {
      top3[2] = top3[1];
      top3[1] = top3[0];
      top3[0] = val;
   } else if (val > top3[1]) {
      top3[2] = top3[1];
      top3[1] = val;
   } else if (val > top3[2]) {
      top3[2] = val;
   }
}


int main(int argc, char **argv) {

   if (argc != 2) {
      printf("Usage: ./a.out INPUT_FILE\n");
      return 1;
   }

   char *filename = argv[1];

   FILE *fp;
   fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("Can't open %s\n", filename);
      return 1;
   }

   char line[BUF_LEN];
   int cur = 0;
   int top3[3] = {0, 0, 0};
   while (fgets(line, BUF_LEN, fp) != NULL) {
      if (strlen(line) == 1) {
         update_top3(top3, cur);
         cur = 0;
      } else {
         cur += atoi(line);
      }
   }
   update_top3(top3, cur);

   printf("first part: %d\n", top3[0]);
   printf("second part: %d\n", top3[0] + top3[1] + top3[2]);
   fclose(fp);

   return 0;
}
