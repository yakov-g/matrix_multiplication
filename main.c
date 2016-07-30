#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <time.h>

#include "matrix.h"
#include "matrix_thread.h"

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

int
main(int argc, char **argv)
{
   const char *path_input1 = NULL, *path_input2 = NULL, *path_output = NULL;
   int n_threads = -1;
   clock_t start, end;
   Matrix *mt1 = NULL, *mt2 = NULL;

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
   T_Pool *tpool = t_pool_create(n_threads);

   mt1 = matrix_from_file_create(filename1);
   mt2 = matrix_from_file_create(filename2);

   if (!mt1 || !mt2)
      goto end;

   start = clock();
   Matrix *mult2 = matrix_no_transpose_mult(mt1, mt2);
   end = clock();
   printf("Time: %ld - %ld = %ld\n", end, start, end - start);
   printf("Time: %ld\n", (end - start)/CLOCKS_PER_SEC);

   start = clock();
   Matrix *mult = matrix_mult(mt1, mt2);
   end = clock();
   printf("Time: %ld - %ld = %ld\n", end, start, end - start);
   printf("Time: %ld\n", (end - start)/CLOCKS_PER_SEC);

   start = clock();
   Matrix *mult3 = matrix_mult_thread(tpool, mt1, mt2);
   end = clock();
   printf("Time: %ld - %ld = %ld\n", end, start, end - start);
   printf("Time: %ld\n", (end - start)/CLOCKS_PER_SEC);

   printf("Matrix cmp 1-2: %d\n", matrix_cmp(mult, mult2));
   printf("Matrix cmp 2-3: %d\n", matrix_cmp(mult2, mult3));

   matrix_delete(mult);
   matrix_delete(mult2);
   matrix_delete(mult3);
   t_pool_destroy(tpool);

end:
   if (filename1) free(filename1);
   if (filename2) free(filename2);
   if (mt1) matrix_delete(mt1);
   if (mt2) matrix_delete(mt2);
   return 0;
}
