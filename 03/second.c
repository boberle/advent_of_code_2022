#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define GROUP_SIZE 3
#define BUF_LEN 1024


int sum_priorities(FILE *);
bool is_char_in_all_members(char [][BUF_LEN], char);
bool is_char_in_line(char *, char);
int convert_item_to_priority(char);



int main(int argc, char **argv) {

   if (argc != 2) {
      printf("filename missing\n");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("can't open %s\n", filename);
      exit(1);
   }

   int priorities = sum_priorities(fp);
   printf("sum of priorities: %d\n", priorities);

   fclose(fp);

   return 0;
}


int sum_priorities(FILE *fp) {

   int total = 0;
   int member = 0;
   char lines[GROUP_SIZE][BUF_LEN];
   while (fgets(lines[member++], BUF_LEN, fp) != 0) {
      if (member == GROUP_SIZE) {
         for (int i=0; i<strlen(lines[0]); i++) {
            char item = lines[0][i];
            if (is_char_in_all_members(lines, item)) {
               total += convert_item_to_priority(item);
               break;
            }
         }
         member = 0;
      }
   }

   return total;
}


bool is_char_in_all_members(char lines[][BUF_LEN], char c) {
   for (int i=1; i<GROUP_SIZE; i++) {
      if (!is_char_in_line(lines[i], c))
         return false;
   }
   return true;
}


bool is_char_in_line(char *line, char c) {
   for (int i=0; i<strlen(line); i++) {
      if (line[i] == c)
         return true;
   }
   return false;
}

int convert_item_to_priority(char item) {
   if ('A' <= item && item <= 'Z')
      return item - 'A' + 27;
   return item - 'a' + 1;
}
