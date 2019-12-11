/*  registers_driver.c
 *  State University of New York, Binghamton
 */


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
    }
    return 0; // there is NO free physical register
}


int pull_pr(APEX_CPU* cpu)
{
    for (int i = 0; i < PRF_ENTRIES_NUMBER; i++)
    {
        if (cpu->prf[i].free)
        {
            return i;
        }
    }
    return -1;
}


// Before calling this function, make sure you first call
// check_pr_free function explicitly
int allocate_pr(APEX_CPU* cpu, int arch_reg)
{
    int free_phys_reg = pull_pr(cpu);
    cpu->prf[free_phys_reg].free = 0;   // this phys reg is not free now
    cpu->prf[free_phys_reg].valid = 0;  // this phys reg is not valid now
    cpu->allocate[arch_reg].phys_reg = free_phys_reg;
    return free_phys_reg;
}


void release_pr(APEX_CPU* cpu, int phys_reg)
{
    if (phys_reg != -1)
    {
        cpu->prf[phys_reg].free = 1;
    }
}


void commit_reg(APEX_CPU* cpu, int arch_reg, int phys_reg)
{

    int phys_reg_to_deallocate = cpu->deallocate[arch_reg].commited_phys_reg;
    release_pr(cpu, phys_reg_to_deallocate);
    cpu->deallocate[arch_reg].commited_phys_reg = phys_reg;
}


void rename_src1(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[DRF];
    int arch_rs1 = stage->arch_rs1;
    stage->phys_rs1 = cpu->allocate[arch_rs1].phys_reg;

}


void rename_src2(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[DRF];
    int arch_rs2 = stage->arch_rs2;
    stage->phys_rs2 = cpu->allocate[arch_rs2].phys_reg;

}

void rename_src3(APEX_CPU* cpu)
{

    CPU_Stage* stage = &cpu->stage[DRF];
    int arch_rs3 = stage->arch_rs3;
    stage->phys_rs3 = cpu->allocate[arch_rs3].phys_reg;

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
        cpu->bis.backup_entry[branch_id].allocate[j].phys_reg = cpu->allocate[j].phys_reg;
    }
}


void restore_prf_allocate(APEX_CPU* cpu)
{
    int branch_id = cpu->stage[Int_FU].branch_id;

    for (int j=0; j < ALLOCATE_PHY_REGISTER; j++)
    {
        cpu->allocate[j].phys_reg = cpu->bis.backup_entry[branch_id].allocate[j].phys_reg;
    }
}


void display_prf_for_debug(APEX_CPU* cpu)
{
    printf("------------------------------ Details of prf State -----------------------------\n");
    for (int i = 0; i < PRF_ENTRIES_NUMBER; i++)
    {
        if (!cpu->prf[i].free)
        {
            printf("| prf[%d] = %d, VALID = %d |",i, cpu->prf[i].value, cpu->prf[i].valid);
        }
    }
    printf("\n");
    printf("---------------------------------------------------------------------------------\n\n");
}


void display_datamem_debug(APEX_CPU* cpu)
{
    printf("-------------------------- Details of Data Memory State -------------------------\n");
    for (int i = 0; i < 100; i++)
    {
        if (cpu->data_memory[i])
        {
            printf("| D[%d] = %d |",i, cpu->data_memory[i]);
        }
    }
    printf("\n");
    printf("---------------------------------------------------------------------------------\n\n");
}


void print_allocate(APEX_CPU* cpu)
{
    int allocate_empty = 1;
    printf("-------------------------------------- allocate --------------------------------------\n");
    for (int i = 0; i < ALLOCATE_PHY_REGISTER; i++)
    {
        if (cpu->allocate[i].phys_reg != -1)
        {
            allocate_empty = 0;
            printf("| allocate[%d] = U%d |",i, cpu->allocate[i].phys_reg);
        }
    }

    if (allocate_empty)
    {
        printf("Empty");
    }
    printf("\n");
    printf("---------------------------------------------------------------------------------\n\n");
}


void print_deallocate(APEX_CPU* cpu)
{
    int deallocate_empty = 1;
    printf("------------------------------------- deallocate -------------------------------------\n");
    for (int i = 0; i < DEALLOCATE_PHY_REGISTER; i++)
    {
        if (cpu->deallocate[i].commited_phys_reg != -1)
        {
            deallocate_empty = 0;
            printf("| R-allocate[%d] = U%d |",i, cpu->deallocate[i].commited_phys_reg);
        }
    }

    if (deallocate_empty)
    {
        printf("Empty");
    }
    printf("\n");
    printf("---------------------------------------------------------------------------------\n\n");
}


void display_stored_allocate(APEX_CPU* cpu, int branch_id)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Details of saved allocate State for Branch ID: %d\n", branch_id);
    for (int i = 0; i < ALLOCATE_PHY_REGISTER; i++)
    {
        if (cpu->bis.backup_entry[branch_id].allocate[i].phys_reg != -1)
        {
            printf("| allocate[%d] = U%d |",i, cpu->bis.backup_entry[branch_id].allocate[i].phys_reg);
        }
    }
    printf("\n");
}


void print_reg(APEX_CPU* cpu)
{
    print_deallocate(cpu);
    print_allocate(cpu);
}


void print_prf(APEX_CPU* cpu)
{
    printf("\n======================== STATE OF PHYSICAL REGISTER FILE ========================\n");
    for (int i = 0; i < PRF_ENTRIES_NUMBER; i++)
    {
        if (!cpu->prf[i].free)
        {
            printf("         |\tprf[%d]\t|\tValue = %d\t|\tStatus = %d\t|\n",i, cpu->prf[i].value, cpu->prf[i].valid);
        }
    }
    printf("================================================================================\n");
}


void print_datamem(APEX_CPU* cpu)
{
    printf("\n============================= STATE OF DATA MEMORY =============================\n");
    for (int i = 0; i < 100; i++)
    {
        printf("                     |\tMEM[%d]\t|\tData Value = %d\t|\n",i, cpu->data_memory[i]);
    }
    printf("================================================================================\n\n");
}


void print_rm(APEX_CPU* cpu)
{
    print_prf(cpu);
    print_datamem(cpu);
}
