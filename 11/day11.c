#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define BUF_LEN 100
#define MAX_ITEMS 200
#define MAX_MONKEYS 10


struct monkey_s {
   long items[MAX_ITEMS];
   int items_hold;
   int total_inspected;
   char op_op;
   int op_value;
   int test;
   int true_monkey;
   int false_monkey;
};
typedef struct monkey_s monkey_t;


int init_monkeys(FILE *, monkey_t[MAX_MONKEYS]);
void init_monkey(FILE *, monkey_t *);
void play_monkey(monkey_t *, monkey_t *, monkey_t *, int);
void play_round(int size, monkey_t[size], int);
unsigned long find_monkey_business(int size, monkey_t [size]);


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

   monkey_t monkeys1[MAX_MONKEYS];
   int monkey_count = init_monkeys(fp, monkeys1);

   fclose(fp);

   monkey_t monkeys2[MAX_MONKEYS];
   memcpy(monkeys2, monkeys1, sizeof(monkeys1));

   // part 1
   for (int i=1; i<=20; i++)
      play_round(monkey_count, monkeys1, -1);

   unsigned long monkey_business = find_monkey_business(monkey_count, monkeys1);
   printf("monkey business (first part): %ld\n", monkey_business);

   // part 2
   int mod = 1;
   for (int m=0; m<monkey_count; m++)
      mod *= monkeys2[m].test;
   for (int i=1; i<=10000; i++)
      play_round(monkey_count, monkeys2, mod);

   monkey_business = find_monkey_business(monkey_count, monkeys2);
   printf("monkey business (second part): %ld\n", monkey_business);

   return 0;

}


int init_monkeys(FILE *fp, monkey_t monkeys[MAX_MONKEYS]) {
   int monkey_index;

   while (fscanf(fp, "Monkey %d:\nStarting items: ", &monkey_index) > 0) {
      monkey_t *m = monkeys + monkey_index;
      m->total_inspected = 0;

      long item;
      while (fscanf(fp, "%ld, ", &item))
         m->items[m->items_hold++] = item;

      char buf[10];
      if (fscanf(fp, "Operation: new = old %c %s\n  Test: divisible by %d\n    If true: throw to monkey %d\nIf false: throw to monkey %d\n\n", &(m->op_op), buf, &(m->test), &(m->true_monkey), &(m->false_monkey)) != 5) {
         puts("can't read");
         exit(1);
      }
      if (strcmp(buf, "old") == 0)
         m->op_value = -1;
      else
         m->op_value = atoi(buf);
   }

   return ++monkey_index;
}


void play_round(int size, monkey_t monkeys[size], int mod) {
   for (int i=0; i<size; i++) {
      play_monkey(monkeys + i, &monkeys[monkeys[i].true_monkey], &monkeys[monkeys[i].false_monkey], mod);
   }
}


void play_monkey(monkey_t *monkey, monkey_t *true_monkey, monkey_t *false_monkey, int mod) {
   for (int i=0; i<monkey->items_hold; i++) {
      monkey->total_inspected++;
      // operation
      long item = monkey->items[i];
      long op_value = monkey->op_value < 0 ? item : monkey->op_value;
      if (monkey->op_op == '*') {
         item *= op_value;
      } else if (monkey->op_op == '+') {
         item += op_value;
      } else {
         printf("unknown operation: %c\n", monkey->op_op);
         exit(1);
      }
      // div by 3 or use mod
      if (mod < 0)
         item = item / 3;
      else
         item %= mod;
      // test and throw to another monkey
      monkey_t *target = item % monkey->test == 0 ? true_monkey : false_monkey;
      target->items[target->items_hold++] = item;
   }

   monkey->items_hold = 0;
}


unsigned long find_monkey_business(int size, monkey_t monkeys[size]) {
   int a = 0, b = 0;
   for (int i=0; i<size; i++) {
      int inspected = monkeys[i].total_inspected;
      if (inspected > a) {
         b = a;
         a = inspected;
      } else if (inspected > b) {
         b = inspected;
      }
   }
   return (unsigned long) a * b;
}

