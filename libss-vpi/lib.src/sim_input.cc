#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "Thread.h"
//#include "processor.h"
//#include "globals.h"
#include "global_vars.h"
//#include "common.h"

//extern "C" {
//include "misc.h"
//}

/* if error message should be printed */
extern int	getopt_error;
/* index into parent argv vector */
extern int	getopt_index;
/* argument associated with option */
extern char *getopt_arg;

extern void getopt_init(void);
extern char getopt_next(int nargc, char **nargv, char *ostr);

bool USE_INSTR_END_LIMIT = false;
SS_TIME_TYPE INSTR_END_LIMIT = 1000000000;

/////////////////////////////////////////////////////////
// For use by qsort.
// This function is used for sorting simulation args,
// a necessary step in generating the output filename.
/////////////////////////////////////////////////////////
int compare_args(const void *fa, const void *fb) {
	char **a;
	char **b;
	char A;
	char B;

	a = (char **)fa;
	b = (char **)fb;

	A = (*a)[1];
	B = (*b)[1];

	return( A - B );
}

//////////////////////////////////////////////////////////////////////////
// This function overrides the defaults set in the library.
// The library is common to all simulators; this function
// allows the simulators to use their own defaults.
//////////////////////////////////////////////////////////////////////////
void set_defaults() {
}	// set_defaults()


/* simulator-specific options */
char *sim_optstring = "A:D:ef:iI:kK:oPS:w:z:Z:";
 
void
sim_options(int argc, char **argv)
{
  char c;
  unsigned int i;
 
  bool user_specified_output = false;
  char output_filename[1024];

  time_t t;
  char timestring[32];

  int exec_index;

  //////////////////////////////////////////////////////////////////////////
  // Set up defaults that I tend to use often in this simulator.
  //////////////////////////////////////////////////////////////////////////
  set_defaults();
 
  ///* parse options */

  getopt_init();
  while ((c = getopt_next(argc, argv, sim_optstring)) != EOF)
  exec_index = getopt_index;
//   if (exec_index == nargs)
//     exec_index--;
  fprintf(stderr, "exec_index = %d\n", exec_index);

  // Must do getopt_init()
  getopt_init();
  while ((c = getopt_next(argc, argv, sim_optstring)) != EOF){
    fprintf(stderr, "Option: %c\n", c);
    switch(c) {

    case 'Y': // Load Checkpoint File                                                                                                        
      fprintf(stderr, "Checkpoint file: %s\n",getopt_arg);
      CHECKPOINT = fopen(getopt_arg, "r");
      if (CHECKPOINT == NULL)
	fprintf(stderr, "Cannot load checkpoint %s\n",getopt_arg);
      break;

    case 'z':
      USE_INSTR_LIMIT = true;
      INSTR_LIMIT = atoll(getopt_arg);
      break;

    case 'Z':
      SKIP_AMT = atoll(getopt_arg);
      break;

    case 'L':
      USE_INSTR_LIMIT = true;
      USE_INSTR_END_LIMIT = true;
      INSTR_END_LIMIT = atoll(getopt_arg);
      break;

    default:
      break;

    } // switch
  } // while

  if (USE_INSTR_END_LIMIT)
    INSTR_LIMIT = (INSTR_END_LIMIT-SKIP_AMT);

  if (USE_INSTR_LIMIT)
    INSTR_LIMIT = (SKIP_AMT + INSTR_LIMIT);
 
  ////fprintf(stderr, "jobfile = %s\n", argv[exec_index]);
  ////// Open the job file
  ////(*fp_job) = fopen(argv[exec_index], "r");
  ////if ((*fp_job) == NULL) {
  ////  fprintf(stderr,"Cannot open job file\nexec_index = %d\n", exec_index);
  ////  exit(0);
  ////}


  //getopt_init();
  //while ((c = getopt_next(argc, argv, sim_optstring)) != EOF)
  //  {
  //    switch (c) {

	// case 'f':
	//    user_specified_output = true;
	//    strcpy(output_filename, "mt.");
	//    strcat(output_filename, getopt_arg);
	//    break;

	// case 'z':
	//    USE_INSTR_LIMIT = true;
	//    INSTR_LIMIT = atoll(getopt_arg);
	//    break;

	// case 'Z':
	//    SKIP_AMT = atoll(getopt_arg);
	//    break;

  //       default:
  //          break;
  //    }
  //  }

  //  // 9/3/05 ER: User now specifies *delta* to run after skip amount.
  //  if (USE_INSTR_LIMIT)
  //     INSTR_LIMIT = (SKIP_AMT + INSTR_LIMIT);

  ///////////////////////////////////////////////
  // Compute the output filename.
  // The filename is derived from non-default
  // simulation settings (i.e. sorted args).
  ///////////////////////////////////////////////
  int sorted_argc;
  char **sorted_argv;

  if (!user_specified_output) {
     sorted_argc = 0;
     sorted_argv = new char *[argc];

     i = 1;
     while ( (i < argc) && (argv[i][0] == '-') ) {
        sorted_argv[sorted_argc] = argv[i];
        sorted_argc += 1;

        i += 1;
     }

     qsort(sorted_argv, sorted_argc, sizeof(char *), compare_args);

     // Now create the output filename from sorted argument list.

     strcpy(output_filename, "mt.");
     if (sorted_argc == 0) {
	      strcat(output_filename, "base");
     }
     else {
        for (i = 0; i < sorted_argc; i++)
	        strcat(output_filename, sorted_argv[i]);
     }
  }

  fp_info = fopen(output_filename, "r");
  if (fp_info) {
     // fatal("Output file '%s' already exists.", output_filename);
     fclose(fp_info);

     //time(&t);
     //cftime(timestring, ".%b%d.%T", &t);

     time(&t);
#if 0
     strftime(timestring, 32, ".%b%d.%T", localtime(&t));
#else
     strftime(timestring, 32, ".%b%d.%H.%M.%S", localtime(&t));
#endif
     strcat(output_filename, timestring);

     fp_info = fopen(output_filename, "r");
     assert(!fp_info);
  }
  fp_info = fopen(output_filename, "w");

  ///////////////////////////////////////////////
  // Print out Simulation Parameters:
  ///////////////////////////////////////////////

  INFO("************************ PARAMETERS *************************");
 
  INFO("**ARGS**");
  for (i = 0; i < argc; i++)
     INFO("%s", argv[i]);

  INFO("--------Benchmark Control.--------------");

  // 9/3/05 ER: Better display of simulation range.
  char from[100], to[100];
//#if !defined(__CYGWIN32__)
//  INFO("FROM -> TO = %s -> %s",
//	(SKIP_AMT ? comma(SKIP_AMT, from, 100) : "begin"),
//        (USE_INSTR_LIMIT ? comma(INSTR_LIMIT, to, 100) : "end"));
//#else
//  INFO("FROM -> TO = %s -> %s",
//	(SKIP_AMT ? (snprintf(from, 100, "%lld", SKIP_AMT), from) : "begin"),
//        (USE_INSTR_LIMIT ? (snprintf(to, 100, "%lld", INSTR_LIMIT), to) : "end"));
//#endif
  //INFO("USE_INSTR_LIMIT = %d", USE_INSTR_LIMIT ? 1 : 0);
  //INFO("SKIP_AMT        = %d", SKIP_AMT);
  //INFO("INSTR_LIMIT     = %d", INSTR_LIMIT);


  INFO("----------------------------------------");

  fflush(fp_info);
}
