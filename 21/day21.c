#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

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
long long get_monkey_value(size_t size, monkey_t[size], char[5], bool *);
monkey_t *get_monkey(size_t size, monkey_t monkeys[size], char[5]);
long long get_humn_value(size_t size, monkey_t monkeys[size], char name[5], long long value);
long long find_reversed_value(long long, long long, long long, char);
void get_monkey_values(size_t size, monkey_t monkeys[size], monkey_t *m, long long *v1, long long *v2, bool *h1, bool *h2);


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

   long long root_value = get_monkey_value(size, monkeys, "root", NULL);
   printf("number of monkeys: %lu\n", size);
   printf("root value: %lld (first part)\n", root_value);

   monkey_t *root = get_monkey(size, monkeys, "root");
   bool h1 = false, h2 = false;
   long long v1, v2;
   get_monkey_values(size, monkeys, root, &v1, &v2, &h1, &h2);
   printf("value to equal is: %lld\n", h1 ? v2 : v1);
   long long humn_value = get_humn_value(size, monkeys, h1 ? root->m1 : root->m2, h1 ? v2 : v1);
   printf("'humn' value is: %lld (second part)\n", humn_value);

   // check
   monkey_t *humn = get_monkey(size, monkeys, "humn");
   humn->value = humn_value;
   v1 = get_monkey_value(size, monkeys, root->m1, NULL);
   v2 = get_monkey_value(size, monkeys, root->m1, NULL);
   printf("v1 = %lld, v2 = %lld, test ", v1, v2);
   if (v1 == v2) {
      printf("OK\n");
   } else {
      printf("NON OK\n");
   }

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


void get_monkey_values(size_t size, monkey_t monkeys[size], monkey_t *m, long long *v1, long long *v2, bool *h1, bool *h2) {
   *v1 = get_monkey_value(size, monkeys, m->m1, h1);
   *v2 = get_monkey_value(size, monkeys, m->m2, h2);
   if ((*h1 && *h2) || (!*h1 && !*h2)) {
      puts("can't find humn");
      exit(1);
   }
}


long long get_monkey_value(size_t size, monkey_t monkeys[size], char name[5], bool *human) {
   if (human != NULL)
      *human = (*human) || (strcmp(name, "humn") == 0);

   monkey_t *m = get_monkey(size, monkeys, name);
   if (m->value != LLONG_MAX) {
      return m->value;
   } else {
      long long v1 = get_monkey_value(size, monkeys, m->m1, human);
      long long v2 = get_monkey_value(size, monkeys, m->m2, human);
      long long v;
      switch (m->op) {
         case '+': v = v1 + v2; break;
         case '-': v = v1 - v2; break;
         case '*': v = v1 * v2; break;
         case '/': v = v1 / v2; break;
         default:
            printf("unknown operator: %c\n", m->op);
            exit(1);
      }
      return v;
   }
}


monkey_t *get_monkey(size_t size, monkey_t monkeys[size], char name[5]) {
   for (size_t i=0; i<size; i++)
      if (strcmp(monkeys[i].name, name) == 0)
         return &monkeys[i];
   printf("unknown monkey: %s\n", name);
   exit(1);
}


long long get_humn_value(size_t size, monkey_t monkeys[size], char name[5], long long value) {
   monkey_t *m = get_monkey(size, monkeys, name);
   monkey_t *m1 = get_monkey(size, monkeys, m->m1);
   monkey_t *m2 = get_monkey(size, monkeys, m->m2);

   if (strcmp(m1->name, "humn") == 0) {
      long long v2 = get_monkey_value(size, monkeys, m2->name, NULL);
      return find_reversed_value(LLONG_MAX, v2, value, m->op);
   } else if (strcmp(m2->name, "humn") == 0) {
      long long v1 = get_monkey_value(size, monkeys, m1->name, NULL);
      return find_reversed_value(v1, LLONG_MAX, value, m->op);
   }

   bool h1, h2;
   long long v1, v2;
   get_monkey_values(size, monkeys, m, &v1, &v2, &h1, &h2);
   if (h1) {
      v1 = find_reversed_value(LLONG_MAX, v2, value, m->op);
   } else {
      v2 = find_reversed_value(v1, LLONG_MAX, value, m->op);
   }
   return get_humn_value(size, monkeys, h1 ? m->m1 : m->m2, h1 ? v1 : v2);
}


long long find_reversed_value(long long v1, long long v2, long long v, char op) {
   if (strchr("+-*/", op) == NULL) {
      printf("unknown operator: %c\n", op);
      exit(1);
   }

   if (v1 == LLONG_MAX) {
      switch (op) {
         case '+': v1 = v - v2; break;
         case '-': v1 = v + v2; break;
         case '*': v1 = v / v2; break;
         case '/': v1 = v * v2; break;
      }
      return v1;
   } else if (v2 == LLONG_MAX) {
      switch (op) {
         case '+': v2 = v - v1; break;
         case '-': v2 = v1 - v; break;
         case '*': v2 = v / v1; break;
         case '/': v2 = v1 / v; break;
      }
      return v2;
   }
   puts("v1 or v2 must be undefined");
   exit(1);
}
