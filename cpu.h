#ifndef _APEX_CPU_H_
#define _APEX_CPU_H_
/**
 *  cpu.h
 *  Contains various CPU and Pipeline Data structures
 *
 *  Author :
 *  Gaurav Kothari (gkothar1@binghamton.edu)
 *  State University of New York, Binghamton
 */

enum
{
  F,
  DRF,
  EX1,
  EX2,
  MEM1,
  MEM2,
  WB,
  NUM_STAGES
};

/* Format of an APEX instruction  */
typedef struct APEX_Instruction
{
  char opcode[128];	// Operation Code
  int rd;		    // Destination Register Address
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int imm;		    // Literal Value
} APEX_Instruction;

/* Model of CPU stage latch */
typedef struct CPU_Stage
{
  int pc;		    // Program Counter
  char opcode[128];	// Operation Code
  int rs1;		    // Source-1 Register Address
  int rs2;		    // Source-2 Register Address
  int rd;		    // Destination Register Address
  int imm;		    // Literal Value
  int rs1_value;	// Source-1 Register Value
  int rs2_value;	// Source-2 Register Value
  int buffer;		// Latch to hold some value
  int mem_address;	// Computed Memory Address
  int busy;		    // Flag to indicate, stage is performing some action
  int stalled;		// Flag to indicate, stage is stalled
  int temp_result;  // to compute the result for add


} CPU_Stage;

/* Model of APEX CPU */
typedef struct APEX_CPU
{

 int command_num;      // Flag to determine the command
 int num_clockcycles_to_simulate; //num of clock cycles to simulate
  /* Clock cycles elasped */
  int clock;
  int clock_stalled_cycles;

  /* Current program counter */
  int pc;

  /* Integer register file */
  int regs[32];
  int regs_valid[32];

  /* Array of 5 CPU_stage */
  CPU_Stage stage[8];

  /* Code Memory where instructions are stored */
  APEX_Instruction* code_memory;
  int code_memory_size;

  /* Data Memory */
  int data_memory[4096];

  /* Some stats */
  int ins_completed;

} APEX_CPU;

APEX_Instruction*
create_code_memory(const char* filename, int* size);

APEX_CPU*
APEX_cpu_init(const char* filename);

int
APEX_cpu_run(APEX_CPU* cpu);

void
APEX_cpu_stop(APEX_CPU* cpu);

int
fetch(APEX_CPU* cpu);

int
decode(APEX_CPU* cpu);

int
execute1(APEX_CPU* cpu);

int
execute2(APEX_CPU* cpu);

int
memory1(APEX_CPU* cpu);

//int
//memory2(APEX_CPU* cpu);

int
writeback(APEX_CPU* cpu);

#endif
