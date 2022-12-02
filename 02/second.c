#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUF_LEN 5

enum result {win, loss, draw};


int get_score(FILE *);
char standardize_move(char);
char get_move_b(char, enum result);
enum result standardize_result(char);


int main(int argc, char **argv) {

   if (argc != 2) {
      printf("argument required: filename\n");
      return 1;
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("Unable to open '%s'\n", filename);
      return 1;
   }

   int score = get_score(fp);
   printf("score is: %d\n", score);

   fclose(fp);

   return 0;

}


int get_score(FILE *fp) {

   char line[BUF_LEN];
   int score = 0;
   while (fgets(line, BUF_LEN, fp) != NULL && strlen(line)) {
      char move_a = standardize_move(line[0]);
      enum result result = standardize_result(line[2]);
      char move_b = get_move_b(move_a, result);
      switch (move_b) {
         case 'R': score += 1; break;
         case 'P': score += 2; break;
         case 'S': score += 3; break;
      }
      switch (result) {
         case draw: score += 3; break;
         case win: score += 6; break;
      }
   }
   return score;
}


char standardize_move(char move) {
   if (move == 'A' || move == 'X') {
      return 'R';
   } else if (move == 'B' || move == 'Y') {
      return 'P';
   } else if (move == 'C' || move == 'Z') {
      return 'S';
   }
}

enum result standardize_result(char result) {
   if (result == 'X') return loss;
   if (result == 'Y') return draw;
   if (result == 'Z') return win;

}


char get_move_b(char move_a, enum result result) {
   if (result == draw)
      return move_a;
   if (move_a == 'R')
      return result == win ? 'P' : 'S';
   if (move_a == 'S')
      return result == win ? 'R' : 'P';
   if (move_a == 'P')
      return result == win ? 'S' : 'R';
}
