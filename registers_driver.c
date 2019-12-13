

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"


int check_pr_free(APEX_CPU* cpu)
{
    for (int i = 0; i < PRF_ENTRIES_NUMBER; i++)
    {
        if (cpu->prf[i].free)
        {
            return 1;
        }
        if("")
        {
            //printf(" ");
        }
    }
    return 0;
}


int pull_pr(APEX_CPU* cpu)
{

    for (int i = 0; i < PRF_ENTRIES_NUMBER; i++)
    {
        if (cpu->prf[i].free)
        {
            return i;
        }
        if("")
        {
            //printf(" ");
        }
    }
    return -1;
}

int allocate_pr(APEX_CPU* cpu, int arch_reg)
{
    int free_phys_reg = pull_pr(cpu);
    cpu->prf[free_phys_reg].free = 0;
    cpu->prf[free_phys_reg].valid = 1;

    int free_phys_reg1 = pull_pr(cpu);
    cpu->prf[free_phys_reg1].free = 0;
    cpu->prf[free_phys_reg1].valid = 0;


    cpu->prf[free_phys_reg1].which_arch_reg = arch_reg;

    cpu->arf[arch_reg].allocate_phys_reg = free_phys_reg1;


    cpu->prf[free_phys_reg].free = 1;   // this phys reg is not free now
    cpu->prf[free_phys_reg].valid = 1;  // this phys reg is not valid now


    return free_phys_reg1;
}


void release_pr(APEX_CPU* cpu, int arch_reg, int phys_reg)
{
    if (phys_reg != -1)
    {

        cpu->arf[arch_reg].value = cpu->prf[phys_reg].value;
        cpu->arf[arch_reg].valid = 1;
        cpu->arf[arch_reg].allocate_phys_reg = -1;
        cpu->prf[phys_reg].which_arch_reg = -1;
        cpu->prf[phys_reg].free = 1;
        cpu->prf[phys_reg].valid = 0;

    }
    if("")
    {
           // printf(" ");
    }
}


void commit_reg(APEX_CPU* cpu, int arch_reg, int phys_reg)
{

    release_pr(cpu, arch_reg, phys_reg);
}


void rename_src1(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[DRF];
    int arch_rs1 = stage->arch_rs1;


      if (cpu->arf[arch_rs1].allocate_phys_reg == -1)
      {
        int fre_reg = allocate_pr(cpu, arch_rs1);
        cpu->arf[arch_rs1].allocate_phys_reg = fre_reg;
        cpu->prf[fre_reg].free = 0;   // this phys reg is not free now
        cpu->prf[fre_reg].valid = 1;  // this phys reg is not valid now

      }
      if("")
        {
            //printf(" ");
        }

      stage->phys_rs1 = cpu->arf[arch_rs1].allocate_phys_reg;

      if (cpu->prf[stage->phys_rs1].valid==0)
      {
          stage->rs1_value = cpu->prf[stage->phys_rs1].value;
          stage->rs1_valid = 1;
      }

      else
      {
            if("")
            {
                //printf(" ");
            }

      }


}


void rename_src2(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[DRF];
    int arch_rs2 = stage->arch_rs2;

    if (cpu->arf[arch_rs2].allocate_phys_reg == -1)
    {
      int fre_reg = allocate_pr(cpu, arch_rs2);
      cpu->arf[arch_rs2].allocate_phys_reg = fre_reg;
      cpu->prf[fre_reg].free = 0;   // this phys reg is not free now
      cpu->prf[fre_reg].valid = 1;  // this phys reg is not valid now
    }

    stage->phys_rs2 = cpu->arf[arch_rs2].allocate_phys_reg;


    if (cpu->prf[stage->phys_rs2].valid==0)
    {
        stage->rs2_value = cpu->prf[stage->phys_rs2].value;
        stage->rs2_valid = 1;
    }
    else
    {
        if("")
        {
            //printf(" ");
        }
    }


}

void rename_src3(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[DRF];
    int arch_rs3 = stage->arch_rs3;

    if (cpu->arf[arch_rs3].allocate_phys_reg == -1)
    {
      int fre_reg = allocate_pr(cpu, arch_rs3);
      cpu->arf[arch_rs3].allocate_phys_reg = fre_reg;
      cpu->prf[fre_reg].free = 0;   // this phys reg is not free now
      cpu->prf[fre_reg].valid = 1;  // this phys reg is not valid now
    }

    stage->phys_rs3 = cpu->arf[arch_rs3].allocate_phys_reg;

    if (cpu->prf[stage->phys_rs3].valid==0)
    {
        stage->rs3_value = cpu->prf[stage->phys_rs3].value;
        stage->rs3_valid = 1;
    }
    else
    {
      if("")
        {
            //printf(" ");
        }
    }


}

void read_src1(APEX_CPU* cpu)
{
  CPU_Stage* stage = &cpu->stage[DRF];
  int phys_rs1 = stage->phys_rs1;

  if (cpu->prf[phys_rs1].valid)
  {
      stage->rs1_value = cpu->prf[phys_rs1].value;
      stage->rs1_valid = 1;
  }
  if("")
    {
          //  printf(" ");
    }

}


void read_src2(APEX_CPU* cpu)
{
    CPU_Stage* stage = &cpu->stage[DRF];
    int phys_rs2 = stage->phys_rs2;

    if (cpu->prf[phys_rs2].valid)
    {
        stage->rs2_value = cpu->prf[phys_rs2].value;
        stage->rs2_valid = 1;
    }

}

void read_src3(APEX_CPU* cpu)
{
    CPU_Stage* stage = &cpu->stage[DRF];
    int phys_rs3 = stage->phys_rs3;
    if (cpu->prf[phys_rs3].valid)
    {
        stage->rs3_value = cpu->prf[phys_rs3].value;
        stage->rs3_valid = 1;
    }
}

void record_prf(APEX_CPU* cpu, enum STAGES FU_type)
{
    int phys_reg = cpu->stage[FU_type].phys_rd;
    int result = cpu->stage[FU_type].buffer;
    cpu->prf[phys_reg].value = result;
    cpu->prf[phys_reg].valid = 1;

}


void commit_prf_allocate(APEX_CPU* cpu, int branch_id)
{

    for (int j=0; j < ALLOCATE_PHY_REGISTER; j++)
    {
        cpu->bis.backup_entry[branch_id].arf[j].allocate_phys_reg = cpu->arf[j].allocate_phys_reg;
    }
    if("")
    {
     //       printf(" ");
    }
}


void restore_prf_allocate(APEX_CPU* cpu)
{
    int branch_id = cpu->stage[Int_FU].branch_id;

    for (int j=0; j < ALLOCATE_PHY_REGISTER; j++)
    {
        cpu->arf[j].allocate_phys_reg = cpu->bis.backup_entry[branch_id].arf[j].allocate_phys_reg;
    }
}


void print_allocate(APEX_CPU* cpu)
{


  printf("Details of arf State -\n");
  for (int i = 0; i < ARF_ENTRIES_NUMBER; i++)
  {
      if (cpu->arf[i].valid)
      {
          printf("\nR[%d]->%d",i, cpu->arf[i].value);
      }
      if("")
        {
            printf(" ");
    }
  }
  printf("\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");



    int allocate_empty = 1;
    printf("Details of RENAME TABLE State -\n");
    for (int i = 0; i < ALLOCATE_PHY_REGISTER; i++)
    {
        if (cpu->arf[i].allocate_phys_reg != -1)
        {
            allocate_empty = 0;
            printf("R[%d] -> %d\n",i, cpu->arf[i].allocate_phys_reg);
        }
    }

    if (allocate_empty)
    {
        printf("Empty");
    }
    printf("\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");



}



void display_stored_allocate(APEX_CPU* cpu, int branch_id)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Details of saved prf State for Branch ID: %d\n", branch_id);
    for (int i = 0; i < ALLOCATE_PHY_REGISTER; i++)
    {
        if (cpu->bis.backup_entry[branch_id].arf[i].allocate_phys_reg != -1)
        {
            printf("| prf[%d] = U%d |",i, cpu->bis.backup_entry[branch_id].arf[i].allocate_phys_reg);
        }
    }
    printf("\n");
}


void print_reg(APEX_CPU* cpu)
{
    print_allocate(cpu);
}

void print_arf(APEX_CPU* cpu)
{
    printf("\n========================== STATE OF ARCH REGISTER FILE ==========================\n");
    for (int i = 0; i < ARF_ENTRIES_NUMBER; i++)
    {
        // if (cpu->arf[i].valid)
        // {
            printf("         |\tarf[%d]\t|\tValue = %d\t|\tStatus = %d\t|\n",i, cpu->arf[i].value, cpu->arf[i].valid);
        // }
    }
    printf("==================================================================================\n");
}


void print_datamem(APEX_CPU* cpu)
{
  printf("\n=============================== STATE OF DATA MEMORY ===============================\n");
    for (int i = 0; i < 100; i++)
    {
        printf("                     |\tMEM[%d]\t|\tData Value = %d\t|\n",i, cpu->data_memory[i]);
    }
    printf("================================================================================\n\n");
}


void print_rm(APEX_CPU* cpu)
{
    print_arf(cpu);
    print_datamem(cpu);
}
