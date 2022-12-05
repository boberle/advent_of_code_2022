#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUF_LEN 1024
#define STACK_COUNT 20
#define STACK_SIZE 100

// change here 1 = first part of the game, 2 = second part
#define GAME_PART 2

void read_crates(FILE *, char[STACK_COUNT][STACK_SIZE], int *);
void move_crates(FILE *, char[STACK_COUNT][STACK_SIZE]);
void get_tops(char[STACK_COUNT][STACK_SIZE], int, char *);
void str_rev(char *);


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

   char crates[STACK_COUNT][STACK_SIZE] = {0};
   int count;
   read_crates(fp, crates, &count);

   move_crates(fp, crates);

   char tops[STACK_COUNT] = {0};
   get_tops(crates, count, tops);

   printf("tops: %s\n", tops);
   fclose(fp);

   return 0;
}


void str_rev(char *str) {
   int len = strlen(str);
   for (int i=0; i<len/2; i++) {
      char tmp = str[i];
      str[i] = str[len-i-1];
      str[len-i-1] = tmp;
   }
}


void read_crates(FILE *fp, char crates[STACK_COUNT][STACK_SIZE], int *count) {
   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != 0) {
      if (strchr("0123456789", buf[1]) != NULL) {
         fgets(buf, BUF_LEN, fp);
         break;
      }
      for (int i=1, j=0; i<strlen(buf); i += 4, j++) {
         if (buf[i] != ' ')
            crates[j][strlen(crates[j])] = buf[i];
         *count = j;
      }
   }
   (*count)++;
   for (int i=0; i<*count; i++) {
      str_rev(crates[i]);
   }
}


#if GAME_PART == 1

void move_crates(FILE *fp, char crates[STACK_COUNT][STACK_SIZE]) {
   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != 0) {
      int count, start, end;
      sscanf(buf, "move %d from %d to %d", &count, &start, &end);
      start--;
      end--;
      for (int i=0; i<count; i++) {
         int start_len = strlen(crates[start]);
         int end_len = strlen(crates[end]);
         crates[end][end_len] = crates[start][start_len-1];
         crates[start][start_len-1] = '\0';
      }
   }
}

#else

void move_crates(FILE *fp, char crates[STACK_COUNT][STACK_SIZE]) {
   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != 0) {
      int count, start, end;
      sscanf(buf, "move %d from %d to %d", &count, &start, &end);
      start--;
      end--;
      int start_len = strlen(crates[start]);
      strcat(crates[end], crates[start]+(start_len-count));
      crates[start][start_len-count] = '\0';
   }
}

#endif


void get_tops(char crates[STACK_COUNT][STACK_SIZE], int crate_count, char *tops) {
   // assumes no stack is empty
   for (int i=0; i<crate_count; i++) {
      tops[i] = crates[i][strlen(crates[i])-1];
   }
}
