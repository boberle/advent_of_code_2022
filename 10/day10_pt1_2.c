#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF_LEN 10
#define TICK_COUNT 240
#define DISPLAY_WIDTH 40


int compute_signal_strengths(FILE *);
int compute_signal_strength(int, int);


struct cpu_s {
   FILE *fp;
   int reg_x;
   int pending_count;
   int pending_value;
};
typedef struct cpu_s cpu_t;


struct screen_s {
   int pos;
   char display[TICK_COUNT];
};
typedef struct screen_s screen_t;


void tick(cpu_t *, screen_t *, int *);
void init_cpu(cpu_t *, FILE *);
void operate_cpu(cpu_t *);
void init_screen(screen_t *);
void operate_screen(screen_t *, int);
void print_screen(screen_t *);



int main(int argc, char **argv) {

   if (argc != 2) {
      fprintf(stderr, "missing filename\n");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      fprintf(stderr, "unable to open %s\n", filename);
      exit(1);
   }

   int strength = 0;
   cpu_t cpu;
   init_cpu(&cpu, fp);

   screen_t screen;
   init_screen(&screen);

   tick(&cpu, &screen, &strength);

   printf("strength is: %d\n", strength);
   print_screen(&screen);

   fclose(fp);

   return 0;
}


void tick(cpu_t *cpu, screen_t *screen, int *strength) {

   for (int cycle=1; cycle<=TICK_COUNT; cycle++) {
      if (cycle == 20 || cycle == 60 || cycle == 100 || cycle == 140 || cycle == 180 || cycle == 220) {
         *strength += cycle * cpu->reg_x;
      }
      operate_screen(screen, cpu->reg_x);
      operate_cpu(cpu);
   }

}


void init_cpu(cpu_t *cpu, FILE *fp) {
   cpu->fp = fp;
   cpu->reg_x = 1;
   cpu->pending_count = 0;
}


void operate_cpu(cpu_t *cpu) {

   if (cpu->pending_count > 0) {
      cpu->pending_count--;
      if (cpu->pending_count == 0)
         cpu->reg_x += cpu->pending_value;
      return;
   }
   
   char buf[BUF_LEN];
   if (fgets(buf, BUF_LEN, cpu->fp) == NULL) {
      fprintf(stderr, "reading error\n");
      exit(1);
   }
   buf[strlen(buf)-1] = '\0';

   if (strcmp(buf, "noop") == 0) {
      return;
   } else if (memcmp(buf, "addx", 4) == 0) {
      cpu->pending_count = 1;
      cpu->pending_value = atoi(buf + 5);
   } else {
      fprintf(stderr, "invalid operation: %s\n", buf);
      exit(1);
   }
}


void init_screen(screen_t *screen) {
   screen->pos = 0;
}

void operate_screen(screen_t *screen, int sprite) {
   char c;
   sprite += DISPLAY_WIDTH * (screen->pos / DISPLAY_WIDTH);
   if (sprite - 1 <= screen->pos && screen->pos <= sprite + 1) {
      c = '#';
   } else {
      c = '.';
   }
   screen->display[screen->pos++] = c;
}

void print_screen(screen_t *screen) {
   int offset = 0;
   while (offset < TICK_COUNT) {
      fwrite(screen->display + offset, 1, DISPLAY_WIDTH, stdout);
      puts("");
      offset += DISPLAY_WIDTH;
   }
}
