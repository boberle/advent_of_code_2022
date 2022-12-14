#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#define BUF_LEN 1024

struct item_s {
   int is_integer;
   void *value;
   int size;
};
typedef struct item_s item_t;


int is_in_order(item_t *a, item_t *b);
void get_pair(FILE *fp, item_t *item1, item_t *item2);
int get_line(char *p, item_t *item);
int get_item(char **p, item_t *item);
int get_list(char **p, item_t **items, int *size);
int get_lbracket(char **p);
int get_rbracket(char **p);
int get_item_sequence(char **p, item_t **items, int *size);
int get_integer(char **p, int *value);
void print_item(item_t item, int indent);


int main(int argc, char **argv) {
   
   if (argc != 2) {
      puts("missing filename");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("unable to open %s\n", filename);
      exit(1);
   }

   char buf[BUF_LEN];
   int index = 1;
   int sum = 0;

   item_t items[1000];
   int size = 0;

   do {
      item_t item1;
      item_t item2;
      get_pair(fp, &item1, &item2);
      int res = is_in_order(&item1, &item2);
      if (res == -1)
         sum += index;
      index++;

      items[size++] = item1;
      items[size++] = item2;
   } while (fgets(buf, BUF_LEN, fp) != NULL);

   printf("sum of pair indices in order: %d\n", sum);

   // add the separators
   char *p;
   item_t one, two;

   strcpy(buf, "[[2]]");
   p = buf;
   if (!get_item(&p, &items[size])) {
      puts("can't parse [[2]]");
      exit(1);
   }
   one = items[size];
   size++;

   strcpy(buf, "[[6]]");
   p = buf;
   if (!get_item(&p, &items[size])) {
      puts("can't parse [[6]]");
      exit(1);
   }
   two = items[size];
   size++;

   qsort(items, size, sizeof(item_t), is_in_order);

   int total = 1;
   for (int i=0; i<size; i++) {
      if (memcmp(&items[i], &one, sizeof(item_t)) == 0 || memcmp(&items[i], &two, sizeof(item_t)) == 0) {
         // printf("---> %d\n", i + 1);
         total *= (i + 1);
      }
   }

   printf("decoder key: %d\n", total);

   return 0;
}

/* -1: in order
 *  0: equal
 *  1: not in order
 */
int is_in_order(item_t *a, item_t *b) {
   if (a->is_integer && b->is_integer) {
      if (*(int *)(a->value) < *(int *)(b->value)) {
         return -1;
      } else if (*(int *)(a->value) == *(int *)(b->value)) {
         return 0;
      } else {
         return 1;
      }
   } else if (!a->is_integer && b->is_integer) {
      item_t new_item = {0, b, 1};
      return is_in_order(a, &new_item);
   } else if (a->is_integer && !b->is_integer) {
      item_t new_item = {0, a, 1};
      return is_in_order(&new_item, b);
   } else {
      int size = a->size < b->size ? a->size : b->size;
      item_t **items1 = a->value;
      item_t **items2 = b->value;
      for (int i=0; i<size; i++) {
         int ans = is_in_order(((item_t *)a->value) + i, ((item_t *)b->value) + i);
         if (ans)
            return ans;
      }
      return a->size == b->size ? 0 : a->size < b->size ? -1 : 1;
   }
}


void get_pair(FILE *fp, item_t *item1, item_t *item2) {
   char buf[BUF_LEN];
   if (!(fgets(buf, BUF_LEN, fp) != NULL && get_line(buf, item1))) {
      printf("can't parse: %s", buf);
      exit(1);
   }
   if (!(fgets(buf, BUF_LEN, fp) != NULL && get_line(buf, item2))) {
      printf("can't parse: %s", buf);
      exit(1);
   }
}


int get_line(char *p, item_t *item) {
   if (get_item(&p, item) && *p == '\n') {
      return 1;
   }
   return 0;
}


int get_item(char **p, item_t *item) {
   if (**p == ',')
      (*p)++;

   int buf;
   if (get_integer(p, &buf)) {
      item->is_integer = 1;
      item->value = (int *)malloc(sizeof(int));
      *(int *)(item->value) = buf;
      return 1;
   }
   if (get_list(p, &item->value, &item->size)) {
      item->is_integer = 0;
      return 1;
   }
   return 0;
}


int get_list(char **p, item_t **items, int *size) {
   char *orig = *p;
   if (get_lbracket(p) && get_item_sequence(p, items, size) && get_rbracket(p)) {
      return 1;
   }
   *p = orig;
   return 0;
}


int get_lbracket(char **p) {
   if (**p == '[') {
      (*p)++;
      return 1;
   }
   return 0;
}


int get_rbracket(char **p) {
   if (**p == ']') {
      (*p)++;
      return 1;
   }
   return 0;
}


int get_item_sequence(char **p, item_t **items, int *size) {
   item_t list[100];
   int counter = 0;
   while (get_item(p, &list[counter])) {
      counter++;
   }
   *items = (item_t *)malloc(sizeof(item_t) * counter);
   memcpy(*items, list, sizeof(item_t) * counter);
   *size = counter;
   return 1; // may be empty
}


int get_integer(char **p, int *value) {
   int v = 0;
   int pass = 0;
   while (isdigit(**p)) {
      v = v * 10 + (**p - 48);
      (*p)++;
      pass = 1;
   }
   *value = v;
   return pass;
}


void print_item(item_t item, int indent) {
   if (item.is_integer) {
      for (int i=0; i<indent; i++)
         printf(" ");
      printf("- %d\n", *(int *)item.value);
   } else if (item.size == 0) {
      for (int i=0; i<indent; i++)
         printf(" ");
      printf("- (no item)\n");
   } else {
      item_t it = *(item_t *)item.value;
      for (int i=0; i<item.size; i++) {
         print_item(*((item_t *)item.value + i), indent + 2);
      }
   }
}
