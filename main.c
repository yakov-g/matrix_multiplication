#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>

typedef struct
{
   unsigned int lines;
   unsigned int columns;
   int *data;
} Matrix;

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

static void
_file_to_array_read(const char *filename)
{
   if (!filename) return;
   int n = 0, m = 0, i = 0, j = 0, res;
   FILE *f = fopen(filename, "r");
   long long *matr = NULL;

   if (!f)
     {
        printf("Can not open file: \"%s\".\n", filename);
        return;
     }

   res = fscanf(f, "%d %d ", &n, &m);
   if (res != 2)
     {
        goto end;
     }

   printf("%d %d %d\n", res, n, m);
   matr = malloc(n * m * sizeof(long long));
   for (i = 0; i < n; i++)
     {
#if 0
        char *str = NULL;
        size_t n = 0;
        getline(&str, &n, f);
        printf(":%s", str);
        free(str);
        str = NULL;
#endif
        for (j = 0; j < m; j++)
          {
             int k;
             res = fscanf(f, "%d", &k);
             if (res != 1) goto end;
             *(matr + m * i +j) = k;
          }
     }

   for (i = 0; i < n; i++)
     {
        for (j = 0; j < m; j++)
          {
             printf("%d ", matr[m * i + j]);
          }
        printf("\n");
     }

end:
   fclose(f);
   if (matr) free(matr);
}

long long
vectors_multiply(const long long *v1, const long long *v2, size_t size)
{
   long long ret = 0;
   size_t i = 0;
   if (!v1 || !v2)
     {
        printf("v1 or v2 is null: %p %p\n", v1, v2);
        return 0;
     }
  for (i = 0; i < size; i++)
    {
       ret = ret + v1[i] * v2[i];
    }
  return ret;
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

   _file_to_array_read(filename1);


end:
   printf("goodbye\n");
   if (filename1) free(filename1);
   if (filename2) free(filename2);
   return 0;
}
