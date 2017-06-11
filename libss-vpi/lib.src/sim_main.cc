#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "misc.h"
#include "mt_trace_consume.h"
#include "Thread.h"
#include "global_vars.h"
/////////////////////////////////////////////////////////////////////

void
sim_config(FILE *stream)
{
}

/* exit when this becomes non-zero */
int sim_exit_now = FALSE;

/* longjmp here when simulation is completed */
jmp_buf sim_exit_buf;

/* instruction jump table */
#ifdef sparc
register void **local_op_jump asm("g7");
#else
void **local_op_jump;
#endif


void
sim_main(void)
{

  ///////////////////////////////////////
  // Decode the binaries of each thread.
  ///////////////////////////////////////
  for (unsigned int i = 0; i < NumThreads; i++)
     THREAD[i]->decode();

  ///////////////////////
  // Load Checkpoint File
  ///////////////////////
  if (CHECKPOINT != NULL)
    {
      fprintf(stderr,"\nRestoring functional checkpoint\n");
      fflush(stderr);
      THREAD[0]->restore_checkpoint(CHECKPOINT);

//       if (SKIP_AMT < THREAD[0]->num_insn)
// 	{
// 	  vpi_printf("Invalid Skip Amount for this Checkpoint.\n");
// 	  vpi_printf("Skip Amount: %lld\n", SKIP_AMT);
// 	  vpi_printf("Checkpointed Skip Amount: %lld\n",
// 		 THREAD[0]->num_insn);
// 	  exit(0);
// 	}

//       vpi_printf("\nFunctional simulation restored to ");
//       skip_dist = THREAD[0]->num_insn;
//       if (skip_dist < 1000) vpi_printf("%u", skip_dist);
//       else if (skip_dist < 1000000) vpi_printf("%.2f K",
// 					   ((double)skip_dist)/1000);
//       else if (skip_dist < 1000000000) vpi_printf("%.2f M",
// 					      ((double)skip_dist)/1000000);
//       else vpi_printf("%.2f B", ((double)skip_dist)/1000000000);
//       vpi_printf(" instructions\n");
    }

  ///////////////////////////                                                                                                                
  // Perform Instruction Skip                                                                                                                
  ///////////////////////////                                                                                                                

  if (SKIP_AMT)
    {
//       skip_dist = SKIP_AMT - THREAD[0]->num_insn;
//       vpi_printf("Fast skipping ");
//       if (skip_dist < 1000) vpi_printf("%u", skip_dist);
//       else if (skip_dist < 1000000) vpi_printf("%.2f K",
// 					   ((double)skip_dist)/1000);
//       else if (skip_dist < 1000000000) vpi_printf("%.2f M",
// 					      ((double)skip_dist)/1000000);
//       else vpi_printf("%.2f B", ((double)skip_dist)/1000000000);

      fprintf(stderr,"Fast skipping %lld instructions ",SKIP_AMT);
      int data = THREAD[0]->get_arch_PC();
      fprintf(stderr,"from 0x%x\n",data);
      THREAD[0]->skip(SKIP_AMT);
      fprintf(stderr,"Current committed instructions: %lld\n",THREAD[0]->num_insn);
    }
  printf("Initial arch PC 0x%x\n",THREAD[0]->get_arch_PC());
  ///////////////////////////////////////
  // Initialize trace consumer.
  ///////////////////////////////////////
  trace_consume_init();

  ////////////////////////////////
  // Simulator Loop.
  ////////////////////////////////
  trace_consume();

}	// sim_main()
