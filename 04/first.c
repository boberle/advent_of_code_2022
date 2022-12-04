#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_LEN 1024


int main(int argc, char **argv) {

   if (argc != 2) {
      printf("missing filename\n");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("unable to open '%s'\n", filename);
      exit(1);
   }

   int included = 0;
   int overlaps = 0;
   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != 0) {
      int start1, end1, start2, end2;
      sscanf(buf, "%d-%d,%d-%d", &start1, &end1, &start2, &end2);
      if ((start1 <= start2 && end2 <= end1)
            || (start2 <= start1 && end1 <= end2)) {
         included += 1;
         printf("%d-%d,%d-%d\n", start1, end1, start2, end2);
      } else if ((start1 <= start2 && start2 <= end1 && end1 <= end2)
            || (start2 <= start1 && start1 <= end2 && end2 <= end1)) {
         overlaps += 1;
         printf("%d-%d,%d-%d\n", start1, end1, start2, end2);
      }
   }
   printf("Included (first ans): %d\n", included);
   printf("Overlaps: %d\n", overlaps);
   printf("Total (second ans): %d\n", included + overlaps);
   fclose(fp);

}
