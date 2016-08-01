Multi - threaded matrix multiplication
======================================
Scroll down for usage


CONSTRAINS
----------
* data type: long long; code can be tuned to float, double, etc
* long long overflow is not checked


SUPPORTING DATA STRUCTURES
--------------------------
In order to work with threads and optimize multiplication algorithms,
this is nice to have convenient data structures. In this case data will
not depend on data structures and we can focus on algorithm tuning.

For this I implemented T_Pool(thread pool), Queue, T_Queue(thread safe queue based on Queue, T_Task - generic task, which receives task function and data, and T_Event - syncronization counter which blocks main thread until tasks are ready (counter == 0).

MULTIPLICATION
--------------
Matrix multiplication M1 x M2 performed by multiplying:  
M1.line1 x M2.column1  
M1.line1 x M2.column2  
...  
M1.line2 x M2.column1  
...  
etc.  

Thus main task is to multiplicate two vectors of the same size: v1 x v2.

Because of the consequence memory layout, in order to decrease number of cache-misses,
simplify code and algorithm, M2 should be transposed. 
In this case we are going to multiply line by line from M1 and M2T (transposed).

Now, all we need to do, is to put two vectors(task data) and task function
into the task queue. But in which order?

Some profiling results (perf):
Tasks are pushed into the queue consequently.

Intel(R) Core(TM)2 Duo CPU @ 2.40GHz
(2 CPU)

---
1. Multiply M1.line by M2.column /per task.  
   Ntasks = M1.lines * M2.columns.  
  
|Threads|Cache-misses|Page-faults|  
|       |   (10^6)   |           |  
|-------|:----------:|----------:|  
|  1    |     4.2    |   28217   |  
|  2    |     6.7    |   26000   |  
|  4    |     7.0    |   21056   |  
|  8    |     8.0    |   19500   |  

---

2. Multiply M1.line by M2 / per task.  
   Ntasks = M1.lines.  
  
|Threads|Cache-misses|Page-faults|  
|       |   (10^6)   |           |  
|-------|:----------:|----------:|  
|  1    |     2.9    |   7914    |  
|  2    |     2.2    |   7920    |  
|  4    |     2.6    |   7924    |  
|  8    |     2.5    |   7589    |  
  
---

3. Multiply M1.line by 1st half of M2 / per task;  
            M1.line by 2nd half of M2 / per task.  
   Ntasks = M1.lines * 2  
  
|Threads|Cache-misses|Page-faults|  
|       |   (10^6)   |           |  
|-------|:----------:|----------:|  
|  1    |     3.3    |   2344    |  
|  2    |     2.6    |   1813    |  
|  4    |     3.5    |   2340    |  
|  8    |     3.2    |   2346    |  

---

4. Multiply all M1.line by 1st half of M2 / per task;  
            all M2.line by 2nd half of M2 / per task.  
   Ntasks = M1.lines * 2. Same as in 3 but in different order.  

|Threads|Cache-misses|Page-faults|  
|       |   (10^6)   |           |  
|-------|:----------:|----------:|  
|  1    |     2.6    |   2326    |  
|  2    |     1.5    |   1818    |  
|  4    |     1.6    |   2331    |  
|  8    |     1.7    |   1831    |  

---

5. Split M1 to number-of-threads chunks.  
   Multiply chunk by M2 / per task;   
   Ntasks = number of threads, 1 task per thread.  

|Threads|Cache-misses|Page-faults|  
|       |   (10^6)   |           |  
|-------|:----------:|----------:|  
|  1    |     1.8    |   2267    |  
|  2    |     2.7    |   2265    |  
|  4    |     2.4    |   1761    |  
|  8    |     2.5    |   2277    |  

---

As can be seen 4th method gave the best results, thus was choosen as main.  
But method 5 should be checked and tested once again.  

Working with queue adds memory allocations and free.  
But profiling didn't show any owerhead at all in this actions.  

USAGE & USE CASES (Scroll down for random generates)
----------------------------------------------------

Build
-----
> make

Run
---
Either ./marix_multiply or ./mm can be used

General usage.
./mm --input1 file1 --input2 file2 -t 2 -o file  
./mm --input1 file1 --input2 file2 --threads 2 --outfile file  

-o/--outfile can be ommited, so no output file will be created.  

Test cases
----------
./mm --input1 m1_2-1 --input2 m2_1-1 -t 2 -o file  

(1, 2) * (-2 )  =  (0)  
         ( 1 )

Vise versa  
./mm --input1 m2_1-1 --input2 m1_2-1 -t 2 -o file  

(-2) * (1, 2) == (-2, -4)  
(1)              (1,   2)   

./mm --input1 m1_1-1 --input2 m1_1-2 -t 2 -o file  
(2) * (4) == (8)

./mm --input1 m4_3-1 --input2 m3_4-1 -t 2 -o file  
./mm --input1 m3_4-1 --input2 m4_3-1 -t 2 -o file  


Multiply larger matrixes (output probaly is useless)
----------------------------------------------------

./mm --input1 m1k_1k-2 --input2 m1k_1k-1 -t 2
./mm --input1 m2k_2k-2 --input2 m2k_2k-1 -t 2


Randomly generate two matrixes (500x500) and multiply them
----------------------------------------------------------
./mm -t 2 -r 500

Randomly generate two matrixes (500x500), multiply them and save to file
./mm -t 2 -r 500 -o file


