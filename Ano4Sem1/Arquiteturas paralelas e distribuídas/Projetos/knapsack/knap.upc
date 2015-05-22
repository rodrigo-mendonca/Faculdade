#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

/* #include <upc.h> */
#include <bupc_collectivev.h>

#include "timer.h"
#include "knap_defaults.h"

shared int n_books = DEFAULT_N_BOOKS;
shared int bag_cap = DEFAULT_BAG_CAP;
shared int alloced_cap;
shared int* weight;
shared int* profit;
shared int* total;
int* use_book;

shared int cap_per_thread;
shared int books_per_thread;

void get_opts (int argc, char** argv);
void init_books ();
int solve (const int n_books, const int bag_cap,
	   const shared int* weight, const shared int* profit,
	   shared int* total);
void backtrack (const int n_books, const int bag_cap,
		const shared int* weight,
		const shared int* total,
		int* use_book);

int
main (int argc, char** argv)
{
  int total_profit, total_weight, book_i, n_sold;
  struct Timer solve_timer, backtrack_timer;

  if (0 == MYTHREAD) {
    get_opts (argc, argv);
    cap_per_thread = (bag_cap + THREADS) / THREADS;
    alloced_cap = cap_per_thread * THREADS;
    books_per_thread = (n_books + THREADS - 1) / THREADS;
  }

  upc_barrier;

  weight = upc_all_alloc (THREADS, books_per_thread * sizeof(int));
  profit = upc_all_alloc (THREADS, books_per_thread * sizeof(int));

  if (0 == MYTHREAD)
    use_book = malloc (n_books * sizeof(int));
  else
    use_book = NULL;

  if (!weight || !profit) {
    fprintf (stderr,
	     "Insufficient memory for %d books with max. capacity %d\n",
	     n_books, bag_cap);
    upc_global_exit (ENOMEM);
  }
  if (0 == MYTHREAD)
    init_books ();

  total = upc_all_alloc (THREADS, cap_per_thread * n_books * sizeof(int));
  if (!total) {
    fprintf (stderr,
	     "Insufficient memory for profit table\n");
    upc_global_exit (ENOMEM);
  }

  if (0 == MYTHREAD) {
    initialize_timer(&solve_timer);
    initialize_timer(&backtrack_timer);
    start_timer(&solve_timer);
  }

  total_profit = solve (n_books, bag_cap, weight, profit, total);

  if (0 == MYTHREAD)
    stop_timer(&solve_timer);

  if (0 == MYTHREAD) {
    start_timer(&backtrack_timer);
    backtrack (n_books, bag_cap, weight, total, use_book);
    stop_timer(&backtrack_timer);

    printf ("Total profit: %d\n", total_profit);
    printf ("Using books:\n");
    total_weight = n_sold = 0;
    for (book_i = 0; book_i < n_books; ++book_i)
      if (use_book[book_i]) {
	if (n_sold < 10)
	  printf ("\t%d (w %d; p %d)\n",
		  book_i,
		  weight[book_i],
		  profit[book_i]);
	else if (n_sold == 10)
	  printf ("\t...\n");
	total_weight += weight[book_i];
	total_profit -= profit[book_i];
	++n_sold;
      }
    if (total_profit != 0)
      printf ("Profit didn't balance!\n");
    printf ("Capacity used: %d of %d with %d books\n",
	    total_weight, bag_cap, n_sold);
    printf ("Times: solve %g, backtrack %g\n",
	    timer_duration(solve_timer),
	    timer_duration(backtrack_timer));
  }
  return 0;
}

int
solve (const int n_books, const int bag_cap,
       const shared int* weight, const shared int* profit,
       shared int* total)
{
  int book_i;
  int cap_j;
  int p;
  shared int* prev_book_total;
  shared int* book_total;

  int cap_start, cap_end;

  /* Base case for using only book 1. */
  upc_forall (cap_j = 0; cap_j <= bag_cap; ++cap_j; &total[cap_j]) {
    if (weight[0] > cap_j)
      /* Doesn't fit. */
      total[cap_j] = 0;
    else
      total[cap_j] = profit[0];
  }

  prev_book_total = &total[0];
  book_total = &total[alloced_cap];


  for (book_i = 1; book_i < n_books; ++book_i) {
    const int wi = weight[book_i];

    /*
      book_total[j] is total[book_i, j],
      prev_book_total[j] is total[book_i-1, j]
    */
    upc_barrier;

    upc_forall (cap_j = 0; cap_j <= bag_cap; ++cap_j; &book_total[cap_j]) {
      int new_profit;

      if (cap_j < weight[book_i]) {
	/* Doesn't fit. */
	book_total[cap_j] = prev_book_total[cap_j];
	continue;
      }

      new_profit = profit[book_i] + prev_book_total[cap_j - wi];

      book_total[cap_j] = (prev_book_total[cap_j] >= new_profit?
			   prev_book_total[cap_j] : new_profit);
    }

    prev_book_total = book_total;
    book_total += alloced_cap;
  }

  upc_barrier;
  return prev_book_total[bag_cap];
}

void
backtrack (const int n_books, const int bag_cap,
	   const shared int* weight,
	   const shared int* total,
	   int* use_book)
{
  int book_i;
  const shared int* cur_total;
  int off;

  memset (use_book, 0, n_books * sizeof(int));

  cur_total = &total[(n_books-1) * alloced_cap + bag_cap];
  for (book_i = n_books-1; book_i > 0; --book_i, cur_total -= alloced_cap) {
    if (*cur_total != *(cur_total - alloced_cap)) {
      use_book[book_i] = 1;
      cur_total -= weight[book_i];
    }
    use_book[0] = (*cur_total > 0);
  }
}

/* ------------------------------------------------------------ */


enum { STATIC, READFILE, RANDOM } init_from = STATIC;
char* fname;
int max_random_weight = DEFAULT_max_random_weight;
int max_random_profit = DEFAULT_max_random_profit;
long seedval = -1;

void get_opts (int argc, char** argv);
void dump_usage (FILE*);
void init_books ();
void read_books (FILE*, const int n, shared int* weight, shared int* profit);
void randgen_books (long seedval, const int n, shared int* weight,
		    shared int* profit);
void staticgen_books (const int n, shared int* weight, shared int* profit);

void
init_books ()
{
  switch (init_from) {
  case STATIC:
    staticgen_books (n_books, &weight[0], &profit[0]);
    break;
  case RANDOM:
    randgen_books (seedval, n_books, &weight[0], &profit[0]);
    break;
  case READFILE:
    {
      FILE* f;
      f = fopen (fname, "r");
      if (!f) {
	int saved_errno = errno;
	fprintf (stderr, "Couldn't open file %s: %s\n",
		 fname, strerror(saved_errno));
	exit (saved_errno);
      }
      read_books (f, n_books, &weight[0], &profit[0]);
      fclose (f);
    }
    break;
  default:
    fprintf (stderr, "Unknown initialization routine.\n");
    exit (1);
  }
}

void
get_opts (int argc, char** argv)
{
  int c;
  extern char *optarg;
  extern int optind;

  char* fname;

  while ((c = getopt(argc, argv, "h?b:c:f:rW:P:s:")) != EOF)
    switch (c) {
    case 'h':
    case '?':
      dump_usage (stdout);
      exit (0);
    case 'b':
      n_books = atoi(optarg);
      if (n_books <= 0) goto err;
      break;
    case 'c':
      bag_cap = atoi(optarg);
      if (bag_cap <= 0) goto err;
      break;
    case 'g':
      if (init_from != STATIC) goto err;
      init_from = STATIC;
      break;
    case 'f':
      fname = optarg;
      if (init_from != STATIC) goto err;
      init_from = READFILE;
      break;
    case 'r':
      if (init_from != STATIC) goto err;
      init_from = RANDOM;
      break;
    case 'W':
      max_random_weight = atoi(optarg);
      if (max_random_weight <= 0) goto err;
      break;
    case 'P':
      max_random_profit = atoi(optarg);
      if (max_random_profit <= 0) goto err;
      break;
    case 's':
      seedval = atol(optarg);
      break;
    default:
      goto err;
    }
  return;
 err:
  dump_usage (stderr);
  exit (1);
}

void
dump_usage (FILE* f)
{
  /*h?b:c:f:rW:P:s:*/
  fprintf (f, "Usage:"
	   "\t\t-h or -?      this message\n"
	   "\t\t-b <int>      number of books (%d)\n"
	   "\t\t-c <int>      bag capacity (%d)\n"
   "\t\t-g            generate weights and profits statically (default)\n"
	   "\t\t-f <filename> file to read weights and profits\n"
	   "\t\t-r            generate weights and profits randomly\n"
	   "\t\t-W <int>      max random weight\n"
	   "\t\t-P <int>      max random profit\n"
	   "\t\t-s <long>     random seed\n",
	   n_books, bag_cap);
}

void
read_books (FILE* f, const int n, shared int* weight, shared int* profit)
{
  int k;

  for (k = 0; k < n; ++k)
    fscanf (f, "%d %d\n", &weight[k], &profit[k]);
}

void
randgen_books_ (const int n, shared int* weight, shared int* profit)
{
  int k;
  double x;

  for (k = 0; k < n; ++k) {
    weight[k] = 1 + (int)(max_random_weight * drand48());
    profit[k] = 1 + (int)(max_random_profit * drand48());
  }
}

void
randgen_books (long seedval, const int n, shared int* weight,
	       shared int* profit)
{
  if (seedval < 0)
    seedval = (long)clock();
  srand48(seedval);
}

void
staticgen_books (const int n, shared int* weight, shared int* profit)
{
  srand48 (1010101L);
  randgen_books_ (n, weight, profit);
}

