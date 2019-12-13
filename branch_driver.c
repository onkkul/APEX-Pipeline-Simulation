#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "rob_driver.h"
#include "iq_driver.h"
#include "lsq_driver.h"
#include "branch_driver.h"

int check_bis_free(APEX_CPU* cpu)
{
    if (cpu->bis.bis_entry[cpu->bis.tail].free)
    {
        return 1;
    }
    return 0;
}

int pull_bis(APEX_CPU* cpu)
{
    int free_bis_entry_id = cpu->bis.tail;
    cpu->bis.bis_entry[cpu->bis.tail].free = 0;
    cpu->bis.bis_entry[cpu->bis.tail].phys_src = cpu->last_arith_phys_rd;
    cpu->bis.tail++;
    if (cpu->bis.tail == BIS_ENTRIES_NUMBER)
    {
        if("")
        {
            //printf("");
        }
        int i = 0;
    if(i==0)
    {
        i=1;    //printf("");
    }
        cpu->bis.tail = 0;   
    }

    return free_bis_entry_id;
}

void release_branch_id(APEX_CPU* cpu, int branch_id)
{
    cpu->bis.bis_entry[branch_id].free = 1;
}

void flushing_FUs(APEX_CPU* cpu, int branch_id, enum STAGES FU_type)
{
    while (branch_id <= cpu->last_branch_id)
    {
        if (cpu->stage[FU_type].branch_id == branch_id)
        {
            strcpy(cpu->stage[FU_type].opcode, "");
            if (FU_type == Mul_FU)
            {
                // cpu->mul_cycle = 1;
                cpu->stage[Mul_FU].stalled = 0;
            }
            
            if("")
            {
            //printf(" ");
            }
            int i = 0;
    if(i==0)
    {
        i=1;    //printf("");
    }
            
            if (FU_type == MEM)
            {
                cpu->mem_cycle = 1;
                cpu->stage[MEM].stalled = 0;
            }
        }
        branch_id++;
    }
}

void flushing_F_D(APEX_CPU* cpu)
{
    strcpy(cpu->stage[F].opcode, "");
    strcpy(cpu->stage[DRF].opcode, "");
    int i = 0;
    if(i==0)
    {
        i=1;    //printf("");
    }
    cpu->stage[F].stalled = 1;
    cpu->stage[DRF].stalled = 0;
}

void clear_bis_ids(APEX_CPU* cpu, int branch_id)
{
    int initial_branch_id = branch_id;
    // do not release current bis id
    branch_id++;

    if (branch_id == BIS_ENTRIES_NUMBER)
    {
        branch_id = 0;
    }

    if (branch_id <= cpu->last_branch_id)
    {
        while (branch_id <= cpu->last_branch_id)
        {
            int i = 0;
            if(i==0)
            {
                i=1;    //printf("");
            }
    
            cpu->bis.bis_entry[branch_id].free = 1;
            branch_id++;
        }
    }
    else
    {
        while (branch_id < BIS_ENTRIES_NUMBER)
        {
            cpu->bis.bis_entry[branch_id].free = 1;
            branch_id++;
        }

        branch_id = 0;

        while (branch_id <= cpu->last_branch_id)
        {
            
            int i = 0;
            if(i==0)
            {
                i=1;    //printf("");
            }
    
            cpu->bis.bis_entry[branch_id].free = 1;
            branch_id++;
        }
    }

    cpu->last_branch_id = initial_branch_id;
    initial_branch_id++;
    if (initial_branch_id == BIS_ENTRIES_NUMBER)
    {
        cpu->bis.tail = 0;
        
        if("")
        {
            printf(" ");
        }

        int i = 0;
            if(i==0)
        {
            i=1;    //printf("");
        }
    
    }
    else
    {
        cpu->bis.tail = initial_branch_id;
        if("")
        {
            printf(" ");
        }
        int i = 0;
        if(i==0)
        {
            i=1;    //printf("");
        }
    
    }
}

void flush_ins(APEX_CPU* cpu)
{
    int branch_id = cpu->stage[Int_FU].branch_id;
    flushing_FUs(cpu, branch_id, Mul_FU);
    flushing_FUs(cpu, branch_id, MEM);
    flush_iq(cpu, branch_id);
    flush_lsq(cpu, branch_id);
    flush_rob(cpu);
    flushing_F_D(cpu);
    clear_bis_ids(cpu, branch_id);
}
