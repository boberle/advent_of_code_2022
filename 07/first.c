#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


#define BUF_LEN 256
#define TOTAL_DISK_SIZE 70000000
#define MIN_DISK_FOR_UPDATE 30000000


struct file_s {
   char name[50];
   long size;
};
typedef struct file_s file_t;


struct dir_s {
   char name[50];
   struct file_s *files;
   int file_count;
   struct dir_s *dirs;
   int dir_count;
   long size;
};
typedef struct dir_s dir_t;


int read_ls(FILE *, dir_t *);
void get_total_size(dir_t *, long *);
void find_dir(dir_t *, long, long *);



int main(int argc, char **argv) {

   if (argc != 2) {
      printf("missing filename\n");
      exit(1);
   }

   char *filename = argv[1];
   FILE *fp = fopen(filename, "r");
   if (fp == NULL) {
      printf("can't open %s\n", filename);
      exit(1);
   }

   char buf[BUF_LEN]; 
   fgets(buf, BUF_LEN, fp); // $ cd /

   dir_t root;
   strcpy(root.name, "/");
   read_ls(fp, &root);

   long total_size = 0l;
   get_total_size(&root, &total_size);
   printf("total size (first part): %ld\n", total_size);

   long dir_size = LONG_MAX;
   long min_size =  MIN_DISK_FOR_UPDATE - (TOTAL_DISK_SIZE - root.size);
   printf("min size to find: %ld\n", min_size);
   find_dir(&root, min_size, &dir_size);
   printf("dir size (second part): %ld\n", dir_size);

   fclose(fp);

   return 0;
}


int read_ls(FILE *fp, dir_t *parent_dir) {

   char buf[BUF_LEN]; 
   dir_t dirs[256];
   int dir_counter = 0;
   file_t files[256];
   int file_counter = 0;
   long size = 0;

   fgets(buf, BUF_LEN, fp); // $ ls

   while (fgets(buf, BUF_LEN, fp) != NULL) {
      buf[strlen(buf)-1] = '\0';
      if (memcmp(buf, "dir", 3) == 0) {
         dir_t dir;
         strcpy(dir.name, buf+4);
         dir.files = NULL;
         dir.dirs = NULL;
         dir.size = 0;
         dirs[dir_counter++] = dir;
      } else if (buf[0] == '$') {
         char *name = buf + 5;
         dir_t *dir = NULL;
         if (strcmp(name, "..") == 0)
            break;
         for (int i=0; i<dir_counter; i++) {
            if (strcmp(dirs[i].name, name) == 0) {
               dir = &dirs[i];
               break;
            }
         }
         if (dir == NULL) {
            printf("can't find dir %s\n", name);
            exit(1);
         }
         size += read_ls(fp, dir);
      } else {
         char *space = strchr(buf, ' ');
         file_t file;
         strcpy(file.name, space+1);
         *space = '\0';
         file.size = atol(buf);
         files[file_counter++] = file;
         size += file.size;
      }
   }

   parent_dir->dirs = (dir_t *)malloc(sizeof(dir_t)*dir_counter);
   memcpy(parent_dir->dirs, dirs, sizeof(dir_t)*dir_counter);
   parent_dir->dir_count = dir_counter;

   parent_dir->files = (file_t *)malloc(sizeof(file_t)*file_counter);
   memcpy(parent_dir->files, files, sizeof(file_t)*file_counter);
   parent_dir->file_count = file_counter;

   parent_dir->size = size;
   printf("size of %s is %ld\n", parent_dir->name, parent_dir->size);

   return size;

}


void get_total_size(dir_t *root, long *result) {
   if (root->size <= 100000)
      *result += root->size;
   for (int i=0; i<root->dir_count; i++) {
      get_total_size(root->dirs + i, result);
   }
}


void find_dir(dir_t *root, long min, long *result) {
   if (root->size >= min) {
      if (root->size < *result) {
         *result = root->size;
      }
   }
   for (int i=0; i<root->dir_count; i++) {
      find_dir(root->dirs + i, min, result);
   }
}
