#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#include "matrix.h"


/* Helper function which checks if path exists and
 * resolves realpath.
 * Returns newly allocated string - must be freed. */
static char *
_realpath_get(const char *input_path)
{
   char *ret = NULL;
   struct stat st;

   if (!input_path) return NULL;
   /* If path was found and this is regular file,
    * resolve real path.*/
   if (!stat(input_path, &st))
     {
        if (S_ISREG(st.st_mode))
           ret = realpath(input_path, NULL);
     }

   return ret;
}




void
matrix_transponse(Matrix *mt)
{
   if (!mt) return;
   long long *m_new = malloc(mt->lines * mt->columns * sizeof(long long));
   size_t i, j;
   for (i = 0; i < mt->lines; i++)
     {
        for (j = 0; j < mt->columns; j++)
          {
             m_new[j * mt->lines + i] = mt->data[i * mt->columns + j];
          }
     }
   free(mt->data);
   i = mt->lines;
   mt->lines = mt->columns;
   mt->columns = i;
   mt->data = m_new;
}

int
main(int argc, char **argv)
{
   const char *path_input1 = NULL, *path_input2 = NULL, *path_output = NULL;
   long int n_threads = 0;

   char *filename1 = NULL, *filename2 = NULL;
   static struct option long_options[] =
     {
          {"input1", required_argument, 0, '1'},
          {"input2", required_argument, 0, '2'},
          {"output", required_argument, 0, 'o'},
          {"threads", required_argument, 0, 't'},
          {"help", no_argument, 0, 'h'},
          {0, 0, 0, 0}
     };

   int long_index = 0, opt;
   while ((opt = getopt_long(argc, argv, "ho:t:", long_options, &long_index)) != -1)
     {
        switch (opt)
          {
           case 0: break;
           case '1':
                   {
                      path_input1 = argv[optind - 1];
                      break;
                   }
           case '2':
                   {
                      path_input2 = argv[optind - 1];
                      break;
                   }
           case 'o':
                   {
                      path_output = argv[optind - 1];
                      break;
                   }
           case 't':
                   {
                      n_threads = strtol(argv[optind - 1], NULL, 0);
                      break;
                   }
           default:
                   {
                   }
          }
     };

   if (!path_input1 || !path_input2)
     {
        printf("check usage\n");
        goto end;
     }

   filename1 = _realpath_get(path_input1);
   if (!filename1)
     {
        printf("Input path 1: doesn't exist or not a file\n");
        goto end;
     }

   filename2 = _realpath_get(path_input2);
   if (!filename2)
     {
        printf("Input path 2: doesn't exist or not a file\n");
        goto end;
     }

   if (n_threads < 0)
     {
        printf("Error: --threads parameter is < 0\n");
        goto end;
     }

   Matrix *mt = matrix_from_file_create(filename1);
   matrix_print(mt);
   matrix_transponse(mt);
   printf("-----------\n");
   matrix_print(mt);

   long long a[] = {1, 2, 3};
   long long b[] = {1, 2, 3};
   printf("res: %lld\n", vectors_multiply(a, b, 3));


end:
   printf("goodbye\n");
   if (filename1) free(filename1);
   if (filename2) free(filename2);
   if (mt) matrix_delete(mt);
   return 0;
}
