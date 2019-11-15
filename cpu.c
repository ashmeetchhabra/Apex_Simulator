/*
 *  cpu.c
 *  Contains APEX cpu pipeline implementation
 *
 *  Author :
 *  Gaurav Kothari (gkothar1@binghamton.edu)
 *  State University of New York, Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"

/* Set this flag to 1 to enable debug messages */
#define ENABLE_DEBUG_MESSAGES 1

/*
 * This function creates and initializes APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
APEX_CPU*
APEX_cpu_init(const char* filename)
{
  if (!filename) {
    return NULL;
  }

  APEX_CPU* cpu = malloc(sizeof(*cpu));
  if (!cpu) {
    return NULL;
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->pc = 4000;
  memset(cpu->regs, 0, sizeof(int) * 32);
  memset(cpu->regs_valid, 1, sizeof(int) * 32);
  memset(cpu->stage, 0, sizeof(CPU_Stage) * NUM_STAGES);
  memset(cpu->data_memory, 0, sizeof(int) * 4000);

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);

  if (!cpu->code_memory) {
    free(cpu);
    return NULL;
  }

  if (ENABLE_DEBUG_MESSAGES) {
    fprintf(stderr,
            "APEX_CPU : Initialized APEX CPU, loaded %d instructions\n",
            cpu->code_memory_size);
    fprintf(stderr, "APEX_CPU : Printing Code Memory\n");
    printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode", "rd", "rs1", "rs2", "imm");

    for (int i = 0; i < cpu->code_memory_size; ++i) {
      printf("%-9s %-9d %-9d %-9d %-9d\n",
             cpu->code_memory[i].opcode,
             cpu->code_memory[i].rd,
             cpu->code_memory[i].rs1,
             cpu->code_memory[i].rs2,
             cpu->code_memory[i].imm);
    }
  }

  /* Make all stages busy except Fetch stage, initally to start the pipeline */
  for (int i = 1; i < NUM_STAGES; ++i) {
    cpu->stage[i].busy = 1;
  }

  return cpu;
}

/*
 * This function de-allocates APEX cpu.
 *
 * Note : You are free to edit this function according to your
 * 				implementation
 */
void
APEX_cpu_stop(APEX_CPU* cpu)
{
  free(cpu->code_memory);
  free(cpu);
}

/* Converts the PC(4000 series) into
 * array index for code memory
 *
 * Note : You are not supposed to edit this function
 *
 */
int
get_code_index(int pc)
{
  return (pc - 4000) / 4;
}

static void
print_instruction(CPU_Stage* stage)
{
  if (strcmp(stage->opcode, "STORE") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
  }

  if (strcmp(stage->opcode, "MOVC") == 0) {
    printf("%s,R%d,#%d ", stage->opcode, stage->rd, stage->imm);
  }

  if (strcmp(stage->opcode,"ADDL")==0)
  {
    //printf("ADDED");
    printf("%s,R%d,R%d,#%d",stage->opcode, stage->rd, stage->rs1, stage->imm);
  }

  if (strcmp(stage->opcode,"SUB")==0)
  {
    printf("%s,R%d,R%d,#%d",stage->opcode, stage->rd, stage->rs1, stage->rs2);
  }

  if (strcmp(stage->opcode, "LOAD") == 0) {
    printf(
      "%s,R%d,R%d,#%d ", stage->opcode, stage->rs1, stage->rs2, stage->imm);
  }

  if (strcmp(stage->opcode, "JUMP") == 0) {
    printf(
      "%s,R%d,#%d ", stage->opcode, stage->rs1, stage->imm);
  }
}

/* Debug function which dumps the cpu stage
 * content
 *
 * Note : You are not supposed to edit this function
 *
 */
static void
print_stage_content(char* name, CPU_Stage* stage)
{
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}

/*
 *  Fetch Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
fetch(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[F];
  if (!stage->busy && !stage->stalled) {
    /* Store current PC in fetch latch */
    stage->pc = cpu->pc;

    /* Index into code memory using this pc and copy all instruction fields into
     * fetch latch
     */
    APEX_Instruction* current_ins = &cpu->code_memory[get_code_index(cpu->pc)];

    strcpy(stage->opcode, current_ins->opcode);
    stage->rd = current_ins->rd;
    stage->rs1 = current_ins->rs1;
    stage->rs2 = current_ins->rs2;
    stage->imm = current_ins->imm;
    //stage->rd = current_ins->rd;

    /* Update PC for next instruction */
    cpu->pc += 4;

    /* Copy data from fetch latch to decode latch*/
    cpu->stage[DRF] = cpu->stage[F];

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", stage);
    }
  }
  return 0;
}

/*
 *  Decode Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
decode(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];

  printf("THe valu of stage->stalled::%d\n",stage->stalled);
  if(stage->stalled) {
    stage->stalled = 0;
  }

  if (!stage->busy && !stage->stalled) {

    /* Read data from register file for store */
    if (strcmp(stage->opcode, "STORE") == 0) {
    stage->rs1_value=stage->rs1;
    stage->rs2_value=stage->rs2;
    }

    /* No Register file read needed for MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
    //printf("Val of rd in decode stage:: %d \n",cpu->regs_valid[stage->rd]);
    //stage->buffer = cpu->regs[stage->imm];

    stage->buffer = stage->imm;


    //cpu->regs_valid[stage->rd] = 1;
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {
    printf("Validity of rs1:: %d\n",cpu->regs_valid[stage->rs1]);
    printf("Validity of rs2:: %d\n",cpu->regs_valid[stage->rs2]);
        if(cpu->regs_valid[stage->rs1] && cpu->regs_valid[stage->rs2]){
        printf("::::::::::::::::::NOT In stalled::::::::::::::::");
        cpu->stage[F].stalled=0;
        cpu->stage[DRF].stalled=0;
        stage->rs1_value=cpu->regs[stage->rs1];
         cpu->regs_valid[stage->rd]=0;
        }
        else{
        printf("::::::::::::::::::In stalled::::::::::::::::");
        cpu->stage[F].stalled=1;
        cpu->stage[DRF].stalled=1;
        cpu->clock++;
        }

    }


    if (strcmp(stage->opcode, "SUB") == 0) {
    stage->rs1_value=stage->rs1;
    stage->rs2_value=stage->rs2;
    }

    if (strcmp(stage->opcode, "LOAD") == 0) {
    stage->rs1_value=stage->rs1;
    //printf("DRF::Val of rs1 in load::%d\n",stage->rs1);
    }

    if (strcmp(stage->opcode, "JUMP") == 0) {
        stage->rs1_value= cpu->regs[stage->rs1];

    }


    /* Copy data from decode latch to execute latch*/
    cpu->stage[EX1] = cpu->stage[DRF];

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Decode/RF", stage);
    }
  }
  return 0;
}

/*
 *  Execute Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
execute1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[EX1];
  if (!stage->busy && !stage->stalled) {

    /* Store */
    if (strcmp(stage->opcode, "STORE") == 0) {
    }

    /* MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {
    }
    if (strcmp(stage->opcode, "SUB") == 0) {
    }
    if (strcmp(stage->opcode, "LOAD") == 0) {
    }

    /* Copy data from Execute latch to Memory latch*/
    cpu->stage[EX2] = cpu->stage[EX1];

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute1", stage);
    }
  }
  return 0;
}

int
execute2(APEX_CPU* cpu)
{
    CPU_Stage* stage = &cpu->stage[EX2];
    if (!stage->busy && !stage->stalled) {

    if (strcmp(stage->opcode, "MOVC") == 0) {
    }
    if (strcmp(stage->opcode, "STORE") == 0) {
    stage->mem_address=stage->rs2_value+stage->imm;
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {

    stage->temp_result=(stage->rs1_value+stage->imm);

    }

    if (strcmp(stage->opcode, "SUB") == 0) {

    //printf("The value of rs1 is::%d\n",stage->rs1_value);
    //printf("The value of rs2 is::%d\n",stage->rs2_value);
    stage->temp_result=(stage->rs1_value-stage->rs2_value);
    //printf("The value of test_resukt in SUB is::%d\n",stage->temp_result);
    }

    if (strcmp(stage->opcode, "LOAD") == 0) {
    stage->mem_address=stage->rs1_value+stage->imm;
    printf("EX2::Val of address in load::%d\n",stage->mem_address);
    }
        cpu->stage[MEM1] = cpu->stage[EX2];
        if (ENABLE_DEBUG_MESSAGES) {
            print_stage_content("Execute2", stage);
        }
    }
    return 0;
}

/*
 *  Memory Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
memory1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[MEM1];
  if (!stage->busy && !stage->stalled) {



    /* Store */
    if (strcmp(stage->opcode, "STORE") == 0) {
    }

    /* MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {
    }

     if (strcmp(stage->opcode, "SUB") == 0) {
    }

    if (strcmp(stage->opcode, "LOAD") == 0) {
    }

    /* Copy data from decode latch to execute latch*/
    cpu->stage[MEM2] = cpu->stage[MEM1];


    if (ENABLE_DEBUG_MESSAGES) {

      print_stage_content("Memory1", stage);

    }
  }

  return 0;
}
int
memory2(APEX_CPU* cpu)
{
    CPU_Stage* stage = &cpu->stage[MEM2];
  if (!stage->busy && !stage->stalled) {

  if (strcmp(stage->opcode, "STORE") == 0) {
  cpu->data_memory[stage->mem_address]=stage->rs1_value;


    }

    /* MOVC */
    if (strcmp(stage->opcode, "MOVC") == 0) {
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {
    }

    if (strcmp(stage->opcode, "SUB") == 0) {
    }
    if (strcmp(stage->opcode, "LOAD") == 0) {
    stage->buffer=cpu->data_memory[stage->mem_address];
    }
        cpu->stage[WB] = cpu->stage[MEM2];
        if (ENABLE_DEBUG_MESSAGES) {
            print_stage_content("Memory2", stage);
    }



    }
    return 0;
}

/*
 *  Writeback Stage of APEX Pipeline
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
writeback(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[WB];
  if (!stage->busy && !stage->stalled) {

    /* Update register file */
    if (strcmp(stage->opcode, "MOVC") == 0) {

    //cpu->regs[stage->rd] = stage->imm;

      cpu->regs[stage->rd] = stage->buffer;
      cpu->regs_valid[stage->rd]=0;
      //cpu->ins_completed++;

//      printf("BUFFER::%d \n",stage->buffer);

//      printf("Write Back::MOV: %d\n",cpu->regs[stage->rd]);
    }

    if (strcmp(stage->opcode, "STORE") == 0) {
    }

    if (strcmp(stage->opcode, "ADDL") == 0) {
    cpu->regs[stage->rd]=stage->temp_result;
    cpu->regs_valid[stage->rd]=1;

    }

    if (strcmp(stage->opcode, "SUB") == 0) {
    cpu->regs[stage->rd]=stage->temp_result;
    cpu->regs_valid[stage->rd]=0;
    }

    if (strcmp(stage->opcode, "LOAD") == 0) {
    cpu->regs[stage->rd]=stage->buffer;
    printf("WB::Val of buffer in load::%d\n",stage->buffer);
    cpu->regs_valid[stage->rd]=0;
    }




    cpu->ins_completed++;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Writeback", stage);
    }
  }
  return 0;
}

/*
 *  APEX CPU simulation loop
 *
 *  Note : You are free to edit this function according to your
 * 				 implementation
 */
int
APEX_cpu_run(APEX_CPU* cpu)
{
//int quit_flag=0;
int no_of_cycles;
//while (quit_flag!=1)
//{
int ch=cpu->command_num;
//printf("Choose an option::\n");
//printf("1.Enter s for simulate\n");
//printf("2.Enter d for display\n");
//printf("3.Enter n for number of cycles upto which simulation will run\n");
//printf("4.Enter q to quit\n");
//scanf("%lc",&ch);

switch(ch)
{
    case 1: {
    printf("----------SIMULATE---------\n");
    //==================================================================================================
    printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n");

         for(int i= 0;i<16;i++)
     {
        printf("|\tR[%d]\t|\tValue %d \t|\tStatus= ",i,cpu->regs[i]);

        if(cpu->regs_valid[i]==0)
            printf("Valid");
        else
            printf("Invalid");
        printf("\t|\n");
    }
    //==================================================================================================
    printf("============== STATE OF DATA MEMORY =============\n");

    for(int i=0;i<100;i++)
    {
        printf("|\tMEM[%d]\t|\tData Value=%d\t|\n",i,cpu->data_memory[i]);
    }

    break;
    //==================================================================================================
    }


    case 2:{
    printf("----------DISPLAY---------\n");
    //==================================================================================================

    //==================================================================================================
    while (1) {
    if (cpu->ins_completed == cpu->code_memory_size) {
      printf("(apex) >> Simulation Complete\n");
      break;

      //==================================================================================================
    }

    if (ENABLE_DEBUG_MESSAGES) {
      printf("--------------------------------\n");
      printf("Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
    }

    writeback(cpu);
    memory2(cpu);
    memory1(cpu);
    execute2(cpu);
    execute1(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;
    }
//==================================================================================================
    printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n");


        for(int i= 0;i<16;i++)
    {
        printf("|\tR[%d]\t|\tValue %d \t|\tStatus= ",i,cpu->regs[i]);

        if(cpu->regs_valid[i]==0)
            printf("Valid");
        else
            printf("Invalid");
        printf("\t|\n");
    }
    //==================================================================================================
    printf("============== STATE OF DATA MEMORY =============\n");

    for(int i=0;i<100;i++)
    {
        printf("|\tMEM[%d]\t|\tData Value=%d\t|\n",i,cpu->data_memory[i]);
    }

    break;
    break;
    //==================================================================================================

    break;
    }

    case 3:{
    printf("----------SIMULATE TO NUMBER OF CYCLES---------\n");
    no_of_cycles=cpu->num_clockcycles_to_simulate;
    //printf("Enter the number of cycles::");
    //scanf("%d",&no_of_cycles);

    //==================================================================================================

    printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n");


      for(int i= 0;i<16;i++)
     {
        printf("|\tR[%d]\t|\tValue %d \t|\tStatus= ",i,cpu->regs[i]);

        if(cpu->regs_valid[i]==0)
            printf("Valid");
        else
            printf("Invalid");
        printf("\t|\n");
    }
    //==================================================================================================
    printf("============== STATE OF DATA MEMORY =============\n");

    for(int j=0;j<100;j++)
    {
        printf("|\tMEM[%d]\t|\tData Value=%d\t|\n",j,cpu->data_memory[j]);
    }
    //==================================================================================================



    for(int k=0;k<no_of_cycles;k++)
    {
      //==================================================================================================
    if (cpu->ins_completed == cpu->code_memory_size ) {
      printf("(apex) >> Simulation Complete\n");
        //break;

      //==================================================================================================
    }

    if (ENABLE_DEBUG_MESSAGES) {
      printf("--------------------------------\n");
      printf("Clock Cycle #: %d\n", cpu->clock);
      printf("--------------------------------\n");
    }

    writeback(cpu);
    memory2(cpu);
    memory1(cpu);
    execute2(cpu);
    execute1(cpu);
    decode(cpu);
    fetch(cpu);
    cpu->clock++;
    }

    //==================================================================================================

    printf("=============== STATE OF ARCHITECTURAL REGISTER FILE AFTER SIMULATE==========\n");


      for(int i= 0;i<16;i++)
     {
        printf("|\tR[%d]\t|\tValue %d \t|\tStatus= ",i,cpu->regs[i]);

        if(cpu->regs_valid[i]==0)
            printf("Valid");
        else
            printf("Invalid");
        printf("\t|\n");
    }
    //==================================================================================================
    printf("============== STATE OF DATA MEMORY AFTER SIMULATE=============\n");

    for(int j=0;j<100;j++)
    {
        printf("|\tMEM[%d]\t|\tData Value=%d\t|\n",j,cpu->data_memory[j]);
    }
    //==================================================================================================

    break;
    //==================================================================================================
    }

   // case 4:{
    //quit_flag=1;

    //break;
    //==================================================================================================
    //}

 // while (1) {

//    /* All the instructions committed, so exit */
//   if (cpu->ins_completed == cpu->code_memory_size) {
//      printf("(apex) >> Simulation Complete");
//      break;
//    }

//    if (ENABLE_DEBUG_MESSAGES) {
//      printf("--------------------------------\n");
//      printf("Clock Cycle #: %d\n", cpu->clock);
//      printf("--------------------------------\n");
//    }

//    writeback(cpu);
//    memory2(cpu);
//    memory1(cpu);
//    execute2(cpu);
//    execute1(cpu);
//    decode(cpu);
//    fetch(cpu);
//    cpu->clock++;
//  }
  //}
}
  return 0;
}
