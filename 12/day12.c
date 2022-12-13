#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define BUF_LEN 1024
// check your input if that's enough
#define MAX_VERTICES 160 * 41
#define MAX_EDGES MAX_VERTICES * 4
#define MAX_PATH_SIZE 10000


struct vertex_s {
   int dist;
   int *path;
   int path_size;
};
typedef struct vertex_s vertex_t;


struct edge_s {
   int origin;
   int target;
};
typedef struct edge_s edge_t;


struct graph_s {
   vertex_t vertices[MAX_VERTICES];
   int vertex_count;
   edge_t edges[MAX_EDGES];
   int edge_count;
   int start;
   int end;
   int edge_indices[MAX_VERTICES];
};
typedef struct graph_s graph_t;


struct graph_info_s {
   char vertices[MAX_VERTICES];
   int height;
   int width;
   int start;
};
typedef struct graph_info_s graph_info_t;


void init_graph(FILE *, graph_t *, graph_info_t *, int[MAX_VERTICES], int *);
int find_shortest_path(graph_t *);
void reset_graph(graph_t *, int);
void draw_svg(graph_info_t *, int *, int);


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

   graph_t graph;
   int starts[MAX_VERTICES];
   int starts_size = 0;
   int main_start;

   graph_info_t graph_info;

   init_graph(fp, &graph, &graph_info, starts, &starts_size);

   int shortest_path = find_shortest_path(&graph);
   printf("min number of steps (part 1): %d\n", shortest_path-1);

   draw_svg(&graph_info, graph.vertices[graph.end].path, shortest_path);

   for (int i=0; i<starts_size; i++) {
      if ((i + 1) % 100 == 0)
         printf("progress %d/%d\n", i+1, starts_size);
      reset_graph(&graph, starts[i]);
      int alternative = find_shortest_path(&graph);
      if (alternative < shortest_path)
         shortest_path = alternative;
   }
   printf("min number of steps (part 2): %d\n", shortest_path-1);

   return 0;

}


void reset_graph(graph_t *graph, int start) {
   for (int i=0; i<graph->vertex_count; i++) {
      graph->vertices[i].dist = INT_MAX;
      graph->vertices[i].path_size = 0;
   }
   graph->start = start;
   graph->vertices[start].dist = 0;
   graph->vertices[start].path_size = 1;
   graph->vertices[start].path[0] = start;
}


int find_shortest_path(graph_t *graph) {
   bool visited[MAX_VERTICES] = {};
   visited[graph->start] = true;
   int cur_vertex = graph->start;

   while (cur_vertex != graph->end) {
      visited[cur_vertex] = true;

      if (graph->edge_indices[cur_vertex] != -1) {
         for (int e=graph->edge_indices[cur_vertex]; graph->edges[e].origin == cur_vertex; e++) {
            if (!visited[graph->edges[e].target]) {
               vertex_t *origin = &graph->vertices[graph->edges[e].origin];
               vertex_t *target = &graph->vertices[graph->edges[e].target];
               if (origin->dist + 1 < target->dist) {
                  target->dist = origin->dist + 1;
                  target->path = memcpy(target->path, origin->path, sizeof(int) * origin->path_size);
                  target->path[origin->path_size] = graph->edges[e].target;
                  target->path_size = origin->path_size + 1;
               }
            }
         }
      }

      int next_vertex = -1;
      for (int v=0; v<graph->vertex_count; v++) {
         if (!visited[v] && (next_vertex == -1 || graph->vertices[v].dist < graph->vertices[next_vertex].dist)) {
            next_vertex = v;
         }
      }
      cur_vertex = next_vertex;
   }

   return graph->vertices[cur_vertex].path_size;

}


void init_graph(FILE *fp, graph_t *graph, graph_info_t *graph_info, int starts[MAX_VERTICES], int *starts_size) {

   graph->edge_count = 0;

   char buf[BUF_LEN];
   char vertices[MAX_VERTICES];
   int width = 0;
   int height = 0;
   while (fgets(buf, BUF_LEN, fp) != NULL) {
      buf[strlen(buf)-1] = '\0';
      width = strlen(buf);
      memcpy(vertices + height * width, buf, width);
      height++;
   }

   graph->vertex_count = width * height;

   for (int i=0; i<graph->vertex_count; i++) {
      graph->vertices[i].dist = INT_MAX;
      graph->vertices[i].path = malloc(sizeof(vertex_t *) * MAX_PATH_SIZE);
      graph->vertices[i].path_size = 0;
      // get all the possible starts for part 2
      if (vertices[i] == 'a')
         starts[(*starts_size)++] = i;
   }

   char *start = strchr(vertices, 'S');
   graph->start = start - vertices;
   *start = 'a';
   graph->vertices[graph->start].dist = 0;
   graph->vertices[graph->start].path[0] = graph->start;
   graph->vertices[graph->start].path_size = 1;


   char *end = strchr(vertices, 'E');
   graph->end = end - vertices;
   *end = 'z';

   graph_info->width = width;
   graph_info->height = height;
   graph_info->start = graph->start;
   memcpy(graph_info->vertices, vertices, width * height);

   for (int i=0; i<width; i++) {
      for (int j=0; j<height; j++) {
         int origin = j * width + i;
         int edge_count = graph->edge_count;
         // east
         if (i < width-1) {
            int target = j * width + i + 1;
            if (vertices[target] <= vertices[origin] + 1) {
               edge_t edge = {origin, target};
               graph->edges[graph->edge_count++] = edge;
            }
         }
         // west
         if (i > 0) {
            int target = j * width + i - 1;
            if (vertices[target] <= vertices[origin] + 1) {
               edge_t edge = {origin, target};
               graph->edges[graph->edge_count++] = edge;
            }
         }
         // north
         if (j > 0) {
            int target = (j-1) * width + i;
            if (vertices[target] <= vertices[origin] + 1) {
               edge_t edge = {origin, target};
               graph->edges[graph->edge_count++] = edge;
            }
         }
         // south
         if (j < height-1) {
            int target = (j+1) * width + i;
            if (vertices[target] <= vertices[origin] + 1) {
               edge_t edge = {origin, target};
               graph->edges[graph->edge_count++] = edge;
            }
         }
         if (edge_count != graph->edge_count)
            graph->edge_indices[origin] = edge_count;
         else
            graph->edge_indices[origin] = -1;
      }
   }

}


void draw_svg(graph_info_t *graph, int *path, int path_size) {

   char filename[] = "drawing.svg";
   FILE *fp = fopen(filename, "w");
   if (fp == NULL) {
      printf("can't open %s\n", filename);
      exit(1);
   }

   char colors[26][7] = {
      "89cc78",
      "badf96",
      "ebf2b2",
      "fff2b1",
      "ffe59d",
      "f6d389",
      "ceaf75",
      "a68a60",
      "acda8d",
      "dcecaa",
      "fff6b7",
      "ffe9a4",
      "ffdc8f",
      "daba7c",
      "b29567",
      "9dd484",
      "cde6a0",
      "fff9bc",
      "ffedaa",
      "ffe095",
      "e7c582",
      "be9f6d",
      "8fce7c",
      "bee197",
      "f0f4b5",
      "fff0af",
   };
   
   fprintf(fp, "<svg viewBox=\"0 0 %d %d\" xmlns=\"http://www.w3.org/2000/svg\">\n", graph->width * 10, graph->height * 10);
   int vertex_index = 0;
   for (int y=0; y<graph->height; y++) {
      for (int x=0; x<graph->width; x++) {
         char color[7];
         char vertex_elevation = graph->vertices[vertex_index] - 97;
         strcpy(color, colors[vertex_elevation]);
         fprintf(fp, "<rect x=\"%d\" y=\"%d\" fill=\"#%s\" width=\"10\" height=\"10\" stroke-width=\"0\" />\n", x * 10, y * 10, color);
         vertex_index++;
      }
   }

   fprintf(fp, "<path d=\"M %d %d ", graph->start % graph->width * 10 + 5, graph->start / graph->width * 10 + 5);
   for (int i=0; i<path_size; i++) {
      fprintf(fp, "L %d %d ", *(path + i) % graph->width * 10 + 5, *(path + i) / graph->width * 10 + 5);
   }
   fprintf(fp, "\" stroke=\"red\" stroke-width=\"1px\" fill=\"transparent\" />\n");

   fprintf(fp, "</svg>");

   fclose(fp);

}
