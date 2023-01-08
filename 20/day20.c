#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAX_NUMBER_SIZE 5500
#define BUF_LEN 1024


size_t parse_file(FILE *, long long*);
void mix(size_t, long long**, unsigned short);
void move_before(size_t, long long**, int, int);
void move_after(size_t, long long**, int, int);
long long get_grove_coordinates(size_t, long long**);
int modulo(int, int);
size_t get_index(size_t size, long long *numbers[size], long long *p);


int main(int argc, char **argv) {

   if (argc != 2) {
      fprintf(stderr, "missing filename\n");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      fprintf(stderr, "unable to open file '%s'\n", filename);
      exit(1);
   }

   long long values[MAX_NUMBER_SIZE];
   size_t size = parse_file(fp, values);
   printf("number of values: %lu\n", size);

   long long *numbers[MAX_NUMBER_SIZE];

   for (size_t i=0; i<size; i++)
      numbers[i] = &values[i];
   mix(size, numbers, 1);
   long long groove_coordinates1 = get_grove_coordinates(size, numbers);
   printf("grove coordinates (first part): %lld\n", groove_coordinates1);

   for (size_t i=0; i<size; i++) {
      values[i] *= 811589153ll;
      numbers[i] = &values[i];
   }
   mix(size, numbers, 10);
   long long groove_coordinates2 = get_grove_coordinates(size, numbers);
   printf("grove coordinates (second part): %lld\n", groove_coordinates2);

   return 0;
}


size_t parse_file(FILE *fp, long long values[MAX_NUMBER_SIZE]) {
   char buf[BUF_LEN];
   size_t size = 0;
   while (fgets(buf, BUF_LEN, fp) != NULL)
      values[size++] = atoi(buf);
   return size;
}


size_t get_index(size_t size, long long *numbers[size], long long *p) {
   for (size_t i=0; i<size; i++)
      if (numbers[i] == p)
         return i;
   fprintf(stderr, "Unable to find pointer for value %lld\n", *p);
   exit(1);
}


void mix(size_t size, long long *numbers[size], unsigned short n) {
   long long *order[size];
   for (size_t i=0; i<size; i++)
      order[i] = numbers[i];

   for (unsigned short k=0; k<n; k++) {
      for (size_t i=0; i<size; i++) {
         long long value = *order[i];
         int turns = labs(value) % (size - 1);
         if (value < 0)
            turns *= -1;
         int old_index = get_index(size, numbers, order[i]);
         int new_index = modulo(old_index + turns, (int)size);
         if (turns < 0)
            move_before(size, numbers, old_index, new_index);
         else if (turns > 0)
            move_after(size, numbers, old_index, new_index);
      }
   }
}


void move_after(size_t size, long long *numbers[size], int old_index, int new_index) {
   long long *value = numbers[old_index];
   long long *new[size];
   size_t c = 0;
   for (int i=0; i<size; i++) {
      if (i == old_index)
         continue;
      new[c++] = numbers[i];
      if (i == new_index)
         new[c++] = value;
   }
   memcpy(numbers, new, sizeof(long long *) * size);
}


void move_before(size_t size, long long *numbers[size], int old_index, int new_index) {
   long long *value = numbers[old_index];
   long long *new[size];
   size_t c = 0;
   for (int i=0; i<size; i++) {
      if (i == old_index)
         continue;
      if (i == new_index)
         new[c++] = value;
      new[c++] = numbers[i];
   }
   memcpy(numbers, new, sizeof(long long *) * size);
}


long long get_grove_coordinates(size_t size, long long *numbers[size]) {
   size_t index_of_0 = 0;
   for (size_t i=0; i<size; i++) {
      if (!*numbers[i]) {
         index_of_0 = i;
         break;
      }
   }

   int a = modulo(index_of_0 + 1000, (int)size);
   int b = modulo(index_of_0 + 2000, (int)size);
   int c = modulo(index_of_0 + 3000, (int)size);
   return *numbers[a] + *numbers[b] + *numbers[c];
}


int modulo(int a, int b) {
    return (a % b + b) % b;
}

