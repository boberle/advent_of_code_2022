#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define BUF_LEN 100


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
   int max = 0;
   while (fgets(line, BUF_LEN, fp) != NULL) {
      if (strlen(line) == 1) {
         if (cur > max) max = cur;
         cur = 0;
      } else {
         cur += atoi(line);
      }
   }
   if (cur > max) max = cur;

   printf("max is: %d\n", max);
   fclose(fp);

   return 0;
}
