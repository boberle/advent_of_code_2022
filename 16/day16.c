#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define part 1


#define MAX_NODES 60
#define BUF_LEN 1024
#define ALLOCATION 10000

#if part == 1
#define MAX_TIME 30
#else
#define MAX_TIME 26
#endif

struct combo_s {
   char valves[MAX_NODES];
   int total;
   unsigned long bits;
};
typedef struct combo_s combo_t;


struct combo_set_s {
   combo_t *combos;
   size_t size;
   int visited;
};
typedef struct combo_set_s combo_set_t;


int get_valve_name(FILE *fp, char *valve_name);
int get_rate_value(FILE *fp);
size_t get_valve_index(char valve_names[MAX_NODES][3], size_t *size, char *valve_name);
void parse_file(FILE *fp, unsigned short rates[MAX_NODES], size_t *node_size, int edges[MAX_NODES][MAX_NODES], int *start);
void set_combo_total(combo_t *combo, size_t node_size, unsigned short [node_size]);
void set_combo_bits(combo_t *combo, size_t node_size);
void explore(
      unsigned short rates[MAX_NODES],
      int edges[MAX_NODES][MAX_NODES],
      size_t node_size,
      combo_set_t current_frame[node_size],
      combo_set_t next_frame[node_size],
      char time);
combo_set_t merge(combo_set_t combos1, combo_set_t combos2);
void merge_into(combo_set_t *target, combo_set_t source);
void print_frame(size_t node_size, combo_set_t frame[node_size]);
void print_combo(combo_t combo, size_t node_size);
void print_combo_set(combo_set_t combo_set, size_t node_size);
int get_max_pressure(size_t node_size, combo_set_t frame[node_size]);
int get_max_pressure2(size_t node_size, combo_set_t frame[node_size]);


int main(int argc, char **argv) {

   if (argc != 2) {
      fprintf(stderr, "missing filename");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      fprintf(stderr, "unable to open '%s'\n", filename);
      exit(1);
   }

   
   unsigned short rates[MAX_NODES];
   size_t node_size;
   int edges[MAX_NODES][MAX_NODES];
   for (size_t i=0; i<MAX_NODES; i++)
      for (size_t j=0; j<MAX_NODES; j++)
         edges[i][j] = -1;
   int start;

   parse_file(fp, rates, &node_size, edges, &start);

   combo_set_t current_frame[node_size];
   memset(current_frame, 0, sizeof(combo_set_t) * node_size);
   current_frame[start].visited = 1;
   combo_set_t next_frame[node_size];
   memset(next_frame, 0, sizeof(combo_set_t) * node_size);
   next_frame[start].visited = 1;

   for (size_t i=0; i<MAX_TIME; i++) {
      // printf("explore: %lu\n", i);
      explore(rates, edges, node_size, current_frame, next_frame, i);
      // print_frame(node_size, next_frame);
      for (size_t j=0; j<node_size; j++) {
         free(current_frame[j].combos);
         current_frame[j] = next_frame[j];
         next_frame[j].combos = NULL;
         next_frame[j].size = 0;
      }
   }
   #if part == 1
   int max = get_max_pressure(node_size, current_frame);
   printf("max pressure (part one): %d\n", max);
   #else
   int max = get_max_pressure2(node_size, current_frame);
   printf("max pressure (part two): %d\n", max);
   #endif

   // free memory (make valgrind happy)
   for (size_t j=0; j<node_size; j++) {
      free(current_frame[j].combos);
   }


   return 0;
}


int get_valve_name(FILE *fp, char *valve_name) {
   char c;
   char a = 0;
   char b = 0;
   while ((c = fgetc(fp)) != EOF) {
      if (65 <= c && c <= 90) {
         if (a == 0) {
            a = c;
         } else {
            b = c;
            break;
         }
      } else {
         if (a != 0)
            a = 0;
      }
   }
   if (a) {
      valve_name[0] = a;
      valve_name[1] = b;
      valve_name[2] = 0;
      return 1;
   }

   return 0;
}


int get_rate_value(FILE *fp) {
   char c;
   char s[10];
   size_t k = 0;
   while ((c = fgetc(fp)) != EOF) {
      if (48 <= c && c <= 57) {
         s[k++] = c;
         while ((c = fgetc(fp)) != EOF && 48 <= c && c <= 57)
            s[k++] = c;
         s[k] = 0;
         return atoi(s);
      }
   }
   return -1;
}

size_t get_valve_index(char valve_names[MAX_NODES][3], size_t *size, char *valve_name) {
   for (size_t i=0; i<*size; i++) {
      if (strcmp(valve_names[i], valve_name) == 0)
         return i;
   }
   strcpy(valve_names[*size], valve_name);
   return (*size)++;
}



void parse_file(FILE *fp, unsigned short rates[MAX_NODES], size_t *node_size, int edges[MAX_NODES][MAX_NODES], int *start) {

   *node_size = 0;
   char valve_names[MAX_NODES][3];

   while (1) {
      char valve_name[3];
      if (!get_valve_name(fp, valve_name))
         break;

      unsigned short rate = (unsigned short) get_rate_value(fp);
      if (rate < 0) {
         fprintf(stderr, "can't find a rate for valve '%s'\n", valve_name);
         exit(1);
      }

      int index = get_valve_index(valve_names, node_size, valve_name);
      rates[index] = rate;

      for (size_t i=0; i<MAX_NODES; i++)
         edges[index][i] = -1;

      int c = 0;
      while (get_valve_name(fp, valve_name)) {
         int target_index = get_valve_index(valve_names, node_size, valve_name);
         edges[index][c++] = target_index;
         if (fgetc(fp) == '\n')
            break;
      }

   }

   *start = get_valve_index(valve_names, node_size, "AA");
   printf("number of valves: %lu\n", *node_size);

   for (size_t i=0; i<*node_size; i++)
      printf("%lu: %s rate: %d\n", i, valve_names[i], rates[i]);

   /*
   for (size_t i=0; i<*node_size; i++) {
      printf("%d (%s)\n", i, valve_names[i]);
      for (size_t j=0; j<*node_size; j++) {
         if (edges[i][j] == -1)
            break;
         printf("- %d (%s)\n", edges[i][j], valve_names[edges[i][j]]);
      }
   }
   */

}


void set_combo_total(combo_t *combo, size_t node_size, unsigned short rates[node_size]) {
   int total = 0;
   for (size_t i=0; i<node_size; i++)
      if (rates[i] && combo->valves[i])
         total += rates[i] * (MAX_TIME - combo->valves[i]);
   combo->total = total;
}


void set_combo_bits(combo_t *combo, size_t node_size) {
   unsigned long bits = 0;
   for (size_t i=0; i<node_size; i++) {
      bits <<= 1;
      if (combo->valves[i])
         bits += 1;
   }
   combo->bits = bits;
}


void explore(
      unsigned short rates[MAX_NODES],
      int edges[MAX_NODES][MAX_NODES],
      size_t node_size,
      combo_set_t current_frame[node_size],
      combo_set_t next_frame[node_size],
      char time) {

   for (int i=0; i<node_size; i++) {

      if (!current_frame[i].visited)
         continue;

      // move
      int target;
      size_t c = 0;
      while ((target = edges[i][c++]) != -1) {
         combo_set_t new = merge(next_frame[target], current_frame[i]);
         free(next_frame[target].combos);
         next_frame[target] = new;
         next_frame[target].visited = 1;
      }

      // open
      if (rates[i]) {
         // open valve for each existing combo
         combo_set_t new_combos = {
            (combo_t *) calloc(current_frame[i].size + 1, sizeof(combo_t)),
            current_frame[i].size
         };
         if (current_frame[i].size)
            memcpy(new_combos.combos, current_frame[i].combos, current_frame[i].size * sizeof(combo_t));
         for (size_t j=0; j<new_combos.size; j++) {
            combo_t *combo = new_combos.combos + j;
            if (combo->valves[i] == 0) { // if not already opened
               combo->valves[i] = time + 1;
               set_combo_total(combo, node_size, rates);
               set_combo_bits(combo, node_size);
            }
         }
         // create a new combo with only the opened valve
         combo_t *new_combo = new_combos.combos + new_combos.size;
         new_combos.size++;
         new_combo->valves[i] = time + 1;
         set_combo_total(new_combo, node_size, rates);
         set_combo_bits(new_combo, node_size);

         combo_set_t new = merge(next_frame[i], new_combos);
         free(new_combos.combos);
         free(next_frame[i].combos);
         next_frame[i] = new;
         next_frame[i].visited = 1;
      }

   }

}


combo_set_t merge(combo_set_t combos1, combo_set_t combos2) {
   combo_set_t combos = {
      (combo_t *) malloc(sizeof(combo_t) * ALLOCATION),
      0
   };
   if (combos1.combos != NULL)
      merge_into(&combos, combos1);
   merge_into(&combos, combos2);

   return combos;
}


void merge_into(combo_set_t *target, combo_set_t source) {
   for (size_t i=0; i<source.size; i++) {
      int replaced = 0;
      for (size_t j=0; j<target->size; j++) {
         // if has same keys and total ==
         if ((source.combos + i)->bits == (target->combos + j)->bits) {
            if ((source.combos + i)->total > (target->combos + j)->total) {
               memcpy(target->combos + j, source.combos + i, sizeof(combo_t));
            }
            replaced = 1;
            break;
         }
      }
      if (!replaced) {
         // reallocate if necessary
         if (target->size && target->size % ALLOCATION == 0) {
            target->combos = (combo_t *) realloc(target->combos, sizeof(combo_t) * (target->size + ALLOCATION));
            if (target->combos == NULL) {
               fprintf(stderr, "unable to realloc\n");
               exit(1);
            }
         }
         // add to target
         memcpy(target->combos + target->size, source.combos + i, sizeof(combo_t));
         target->size++;
      }
   }
}


int get_max_pressure(size_t node_size, combo_set_t frame[node_size]) {
   int max = 0;
   // size_t count = 0;
   for (size_t i=0; i<node_size; i++) {
      // count += frame[i].size;
      if (!frame[i].visited || !frame[i].size)
         continue;
      for (size_t j=0; j<frame[i].size; j++) {
         if (frame[i].combos[j].total > max) {
            max = frame[i].combos[j].total;
         }
      }
   }
   // printf("count: %lu\n", count);
   return max;
}


int get_max_pressure2(size_t node_size, combo_set_t frame[node_size]) {

   combo_set_t combos = {
      (combo_t *) malloc(sizeof(combo_t) * ALLOCATION),
      0
   };
   for (size_t i=0; i<node_size; i++)
      if (frame[i].visited && frame[i].size)
         merge_into(&combos, frame[i]);

   int max = 0;
   for (size_t i=0; i<combos.size; i++) {
      for (size_t j=0; j<combos.size; j++) {
         if (!(combos.combos[i].bits & combos.combos[j].bits)) {
            int total = combos.combos[i].total + combos.combos[j].total;
            if (total > max)
               max = total;
         }
      }
   }

   free(combos.combos);

   // printf("count: %lu\n", combos.size);
   return max;
}


void print_frame(size_t node_size, combo_set_t frame[node_size]) {
   for (size_t i=0; i<node_size; i++) {
      printf("[[ %lu ]] visited=%d\n", i, frame[i].visited);
      print_combo_set(frame[i], node_size);
   }
}


void print_combo_set(combo_set_t combo_set, size_t node_size) {
   for (size_t j=0; j<combo_set.size; j++) {
      print_combo(combo_set.combos[j], node_size);
   }
}


void print_combo(combo_t combo, size_t node_size) {
   for (size_t k=0; k<node_size; k++)
      printf("%02d ", combo.valves[k]);
   printf("tot=%d bits=%lu\n", combo.total, combo.bits);
}
