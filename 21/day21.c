#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define BUF_LEN 50
#define MAX_MONKEYS 3000

struct monkey_s {
   char name[5];
   long long value;
   char op;
   char m1[5];
   char m2[5];
};
typedef struct monkey_s monkey_t;


void parse_file(FILE *, monkey_t[MAX_MONKEYS], size_t *);
long long get_monkey_value(size_t size, monkey_t[size], char[5]);


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

   monkey_t monkeys[MAX_MONKEYS];
   size_t size = 0;
   parse_file(fp, monkeys, &size);
   fclose(fp);

   long long root_value = get_monkey_value(size, monkeys, "root");
   printf("number of monkeys: %lu\n", size);
   printf("root value: %lld\n", root_value);

   return 0;
}


void parse_file(FILE *fp, monkey_t monkeys[MAX_MONKEYS], size_t *size) {
   
   char buf[BUF_LEN];
   while (fgets(buf, BUF_LEN, fp) != NULL) {
      monkey_t m = { .value=LLONG_MAX };
      if (strlen(buf) == 18) {
         sscanf(buf, "%[^:]: %s %c %s\n", m.name, m.m1, &m.op, m.m2);
      } else {
         sscanf(buf, "%[^:]: %lld\n", m.name, &m.value);
      }
      monkeys[(*size)++] = m;
   }
}


long long get_monkey_value(size_t size, monkey_t monkeys[size], char name[5]) {
   for (int i=0; i<size; i++) {
      if (strcmp(monkeys[i].name, name) == 0) {
         if (monkeys[i].value != LLONG_MAX) {
            return monkeys[i].value;
         } else {
            long long v1 = get_monkey_value(size, monkeys, monkeys[i].m1);
            long long v2 = get_monkey_value(size, monkeys, monkeys[i].m2);
            long long v;
            switch (monkeys[i].op) {
               case '+': v = v1 + v2; break;
               case '-': v = v1 - v2; break;
               case '*': v = v1 * v2; break;
               case '/': v = v1 / v2; break;
               default:
                  printf("unknown operator: %c\n", monkeys[i].op);
                  exit(1);
            }
            monkeys[i].value = v;
            return v;
         }
      }
   }
   printf("unknown monkey: %s\n", name);
   exit(1);
}
