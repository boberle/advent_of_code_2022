#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define BUF_LEN 4096


int find_marker(char *, int);
bool compare(int count, char [count], int);


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

   char buf[BUF_LEN];
   if (fgets(buf, BUF_LEN, fp) == NULL) {
      printf("can't read line\n");
      exit(1);
   }

   int marker_pos = find_marker(buf, 4);
   printf("4-char long marker is at %d\n", marker_pos);
   marker_pos = find_marker(buf, 14);
   printf("14-char long marker is at %d\n", marker_pos);

   fclose(fp);

   return 0;
}


bool compare(int count, char last[count], int start) {
   for (int j=start+1; j<count; j++) {
      if (last[start] == last[j]) {
         return false;
      }
   }
   return true;
}


int find_marker(char *buf, int count) {
   char last[count];

   int len = strlen(buf);
   for (int i=0; i<len; i++) {
      // move
      for (int j=0; j<count-1; j++) {
         last[j] = last[j+1];
      }
      last[count-1] = buf[i];
      // compare
      if (i >= count) {
         bool flag = true;
         for (int j=0; j<count-1; j++) {
            if (!compare(count, last, j)) {
               flag = false;
               break;
            }
         }
         if (flag) {
            for (int j=0; j<count; j++) {
               printf("%c", last[j]);
            }
            printf("\n");
            return i + 1;
         }
      }
   }

   printf("BAAAAAAAAAAD\n");
   exit(1);

}

