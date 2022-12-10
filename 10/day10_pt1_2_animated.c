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


void tick(cpu_t *, screen_t *, int *, char [TICK_COUNT][1024]);
void init_cpu(cpu_t *, FILE *);
void operate_cpu(cpu_t *);
void init_screen(screen_t *);
void operate_screen(screen_t *, int);
void print_screen(screen_t *);
void make_html(char [1024], screen_t *, int);
void print_html(char [TICK_COUNT][1024]);



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

   char html_texts[TICK_COUNT][1024];

   tick(&cpu, &screen, &strength, html_texts);

   printf("strength is: %d\n", strength);
   print_screen(&screen);
   puts("------");
   print_html(html_texts);

   fclose(fp);

   return 0;
}


void tick(cpu_t *cpu, screen_t *screen, int *strength, char html_texts[TICK_COUNT][1024]) {

   for (int cycle=1; cycle<=TICK_COUNT; cycle++) {
      if (cycle == 20 || cycle == 60 || cycle == 100 || cycle == 140 || cycle == 180 || cycle == 220) {
         *strength += cycle * cpu->reg_x;
      }
      operate_screen(screen, cpu->reg_x);
      make_html(html_texts[cycle-1], screen, cpu->reg_x);
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


void make_html(char html_text[1024], screen_t *screen, int sprite) {
   int k = 0;
   int pos = screen->pos - 1;
   sprite += DISPLAY_WIDTH * (screen->pos / DISPLAY_WIDTH);
   int sprite_start = sprite - 1;
   int sprite_end = sprite + 1;

   for (int i=0; i<TICK_COUNT; i++) {

      if (sprite_start == i) {
         //char t[] = "<span style=\"border: 1px black solid\">";
         char t[] = "<span class=\"sprite\">";
         strcpy(html_text + k, t);
         k += strlen(t);
      }

      if (pos == i) {
         //char t[] = "<span style=\"color: red; font-weight: bold\">";
         char t[] = "<span class=\"cursor\">";
         strcpy(html_text + k, t);
         k += strlen(t);
      }

      char c = screen->display[i];
      html_text[k++] = c ? c : ' ';

      if (pos == i) {
         char t[] = "</span>";
         strcpy(html_text + k, t);
         k += strlen(t);
      }

      if (sprite_end == i) {
         char t[] = "</span>";
         strcpy(html_text + k, t);
         k += strlen(t);
      }

      if ((i + 1) % DISPLAY_WIDTH == 0)
         html_text[k++] = '\n';

   }
   html_text[k] = '\0';
}


void print_html(char html_texts[TICK_COUNT][1024]) {
   printf("<html><head><style>.sprite{border:1px black solid;}.cursor{color:red;font-weight:bold;}</style></head><body><code><pre id=\"e\"></pre></code><script>const e=document.getElementById(\"e\"); let c=0;; const t=[");
   for (int i=0; i<TICK_COUNT; i++) {
      printf("`%s`,", html_texts[i]);
   }
   printf("]; setInterval(()=>{e.innerHTML=t[c++]; if(c==t.length) c=0;},200);</script></body></html>\n");
}

