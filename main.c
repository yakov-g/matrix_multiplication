#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <time.h>

#include "matrix.h"
#include "matrix_thread.h"

/* Helper function which checks if path exists
 * and is a file.
 * If condition met - resolves realpath.
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

/* Helper function to chec if path exists. */
static int
_is_path_exist(const char *input_path)
{
   struct stat st;
   if (!input_path) return 0;
   return !stat(input_path, &st);
}

int
main(int argc, char **argv)
{
   const char *path_input1 = NULL, *path_input2 = NULL, *path_output = NULL;
   int n_threads = -1;
   struct timespec start, end;
   Matrix *mt1 = NULL, *mt2 = NULL, *mult = NULL;

   char *filename1 = NULL, *filename2 = NULL;
   int help = 0, random = 0;
   static struct option long_options[] =
     {
          {"input1", required_argument, 0, '1'},
          {"input2", required_argument, 0, '2'},
          {"output", required_argument, 0, 'o'},
          {"threads", required_argument, 0, 't'},
          {"random", required_argument, 0, 'r'},
          {"help", no_argument, 0, 'h'},
          {0, 0, 0, 0}
     };

   int long_index = 0, opt;
   while ((opt = getopt_long(argc, argv, "ho:t:r:", long_options, &long_index)) != -1)
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
           case 'r':
                   {
                      random = strtol(argv[optind - 1], NULL, 0);
                      if (random <= 0)
                         random = 0;
                      break;
                   }
           default:
                   {
                      help = 1;
                   }
          }
     };

   if (!random)
     {
        if (!path_input1 || !path_input2)
          {
             help = 1;
          }
     }

   if (n_threads <= 0)
     {
        printf("Error: --threads parameter is < 0\n");
        help = 1;
     }

   if (help)
     {
        printf("Usage: %s --input1 file1 --input2 file2 -t N [-h/--help] [-o OUTFILE] [-r/--random N]\n", argv[0]);
        printf("       --help/-h Print this help.\n");
        printf("       --input1 Provide 1st input filename.\n");
        printf("       --input2 Provide 2nd input filename.\n");
        printf("       --output/-o Provide output filename.\n");
        printf("       --threads/-t Provide number of threads\n");
        printf("       --random/-r Provide size for random square matrix\n");
        printf("\n");
        goto end;
     }

   if (!random)
     {
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
     }

   if (path_output && _is_path_exist(path_output))
     {
        printf("Output file already exists\n");
        goto end;
     }

   if (!random)
     {
        mt1 = matrix_from_file_create(filename1);
        mt2 = matrix_from_file_create(filename2);
     }
   else
     {
        mt1 = matrix_random_create(random, random);
        mt2 = matrix_random_create(random, random);
     }

   if (!mt1 || !mt2)
      goto end;

#define SEC(x) (x.tv_sec)

   clockid_t clock_type = CLOCK_MONOTONIC_RAW;

   /* Create locked thread pool.
    * Pool need to be run when tasks are ready. */
   T_Pool *tpool = t_pool_create(n_threads, 1);

   clock_gettime(clock_type, &start);
   mult = matrix_mult_thread(tpool, mt1, mt2);
   clock_gettime(clock_type, &end);
   if (mult)
      printf("Time: %ld - %ld = %ld\n", SEC(end), SEC(start), SEC(end) - SEC(start));

#undef SEC

   if (mult)
     {
        printf("Output path: %s\n", path_output);
        matrix_to_file_save(mult, path_output);
     }

   t_pool_run(tpool);
   t_pool_destroy(tpool);

end:
   if (filename1) free(filename1);
   if (filename2) free(filename2);
   if (mt1) matrix_destroy(mt1);
   if (mt2) matrix_destroy(mt2);
   if (mult) matrix_destroy(mult);
   return 0;
}
