#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define BUF_LEN 1024


int sum_priorities(FILE *);
char find_item_in_both_compartments(char *);
int convert_item_to_priority(char);


int main(int argc, char **argv) {

   if (argc != 2) {
      printf("filename argument missing\n");
      return 1;
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("I can't open %s\n", filename);
      return 1;
   }

   int priorities = sum_priorities(fp);
   printf("sum of priorities: %d\n", priorities);
   fclose(fp);

   return 0;
}


int sum_priorities(FILE *fp) {
   int total = 0;
   char line[BUF_LEN];
   while (fgets(line, BUF_LEN, fp) != 0) {
      char common_item = find_item_in_both_compartments(line);
      if (common_item == 0) {
         printf("no common item\n");
         exit(1);
      }
      total += convert_item_to_priority(common_item);
   }
   return total;
}


char find_item_in_both_compartments(char *items) {
   int len = strlen(items) / 2;
   for (int i=0; i<len; i++) {
      for (int j=len; j<len*2; j++) {
         if (items[i] == items[j])
            return items[i];
      }
   }
   return 0;
}


int convert_item_to_priority(char item) {
   if ('A' <= item && item <= 'Z')
      return item - 'A' + 27;
   return item - 'a' + 1;
}
