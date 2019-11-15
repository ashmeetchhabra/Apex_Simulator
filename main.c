/*
 *  main.c
 *
 *  Author :
 *  Gaurav Kothari (gkothar1@binghamton.edu)
 *  State University of New York, Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include<string.h>

#include "cpu.h"

int
main(int argc, char const* argv[])
{
printf("argc::%d\n",argc);
  if (!(argc == 3 || argc == 4)) {
    fprintf(stderr, "APEX_Help : Usage ./apex_sim <input_file> command no.OfCycles(optional)\n");
    exit(1);
  }

  APEX_CPU* cpu = APEX_cpu_init(argv[1]);
  if (!cpu) {
    fprintf(stderr, "APEX_Error : Unable to initialize CPU\n");
    exit(1);
  }

  if(!(strcmp(argv[2],"display"))){
  cpu->command_num=2; //2 for display
  }

  if(!(strcmp(argv[2],"simulate"))){
  //printf("BEFORE::The 4th arg is::%d",atoi(argv[3]));

  if(argc==4){

  //i = atoi(s);


  printf("The 4th arg is::%d",atoi(argv[3]));

        cpu->num_clockcycles_to_simulate=atoi(argv[3]);//atoi converts string to integer


    cpu->command_num=3; //3 for simulate for number of clock cycles


  }
   else{

    cpu->command_num=1; //1 for simulate only
    }





  }


  APEX_cpu_run(cpu);
  APEX_cpu_stop(cpu);
  return 0;
}
