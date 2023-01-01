#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


#define MAX_DIGITS 28
#define STR_LEN 29
#define MAX_NUMBERS 150

unsigned long long convert_from_snafu(char *);
void convert_to_snafu(unsigned long long, char[STR_LEN]);
void parse_file(FILE *, char[MAX_NUMBERS][STR_LEN], size_t *);


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

   char numbers[MAX_NUMBERS][STR_LEN];
   size_t size;
   parse_file(fp, numbers, &size);
   printf("number of numbers: %ld\n", size);

   unsigned long long sum = 0;
   for (int i=0; i<size; i++) {
      sum += convert_from_snafu(numbers[i]);
   }
   char buf[STR_LEN];
   convert_to_snafu(sum, buf);
   printf("total (first part): %llu, aka:\n%s\n", sum, buf);

   fclose(fp);

   return 0;
}


void parse_file(FILE *fp, char numbers[MAX_NUMBERS][STR_LEN], size_t *size) {
   *size = 0;
   char buf[STR_LEN];
   while (fgets(buf, STR_LEN, fp) != NULL) {
      buf[strlen(buf)-1] = '\0';
      strcpy(numbers[(*size)++], buf);
   }
}


void convert_to_snafu(unsigned long long n, char out[STR_LEN]) {
   int digits[MAX_DIGITS] = {0};
   for (int i=MAX_DIGITS-1, c=0; i>=0; i--, c++) {
      unsigned long long p = (long long) powl(5, i);
      digits[c] = (unsigned long long) n / (unsigned long long) p;
      n = (unsigned long long) n % (unsigned long long) p;
   }

   for (int i=MAX_DIGITS-1; i>=0; i--) {
      if (digits[i] == 5) {
         digits[i] = 0;
         digits[i-1]++;
      } else if (digits[i] == 3) {
         digits[i] = -2;
         digits[i-1]++;
      } else if (digits[i] == 4) {
         digits[i] = -1;
         digits[i-1]++;
      }
   }

   int c = 0;
   for (int i=0; i<MAX_DIGITS; i++) {
      if (digits[i] || c || i == MAX_DIGITS-1) {
         if (digits[i] == -2)
            out[c++] = '=';
         else if (digits[i] == -1)
            out[c++] = '-';
         else
            out[c++] = digits[i] + 48;
      }
   }
   out[c] = '\0';
}


unsigned long long convert_from_snafu(char *n) {
   int len = strlen(n);
   unsigned long long out = 0;
   for (int i=0; i<len; i++) {
      unsigned long long p = (unsigned long long) powl(5, len - i - 1);
      switch (n[i]) {
         case '0':
            break;
         case '1':
            out += p;
            break;
         case '2':
            out += 2 * p;
            break;
         case '-':
            out -= p;
            break;
         case '=':
            out -= 2 * p;
            break;
      }
   }
   return out;
}
