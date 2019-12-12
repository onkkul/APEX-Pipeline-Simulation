/*  rob_driver.c
*   State University of New York, Binghamton
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "rob_driver.h"
#include "iq_driver.h"
#include "lsq_driver.h"
#include "registers_driver.h"
#include "branch_driver.h"

int check_rob_empty(APEX_CPU* cpu)
{
    for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
    {
        if (!cpu->rob.rob_entry[i].free)
        {
            return 0;
        }
    }
    return 1;
}

int check_rob_entry_free(APEX_CPU* cpu)
{
    if (cpu->rob.rob_entry[cpu->rob.tail].free)
    {
        return 1;
    }

    return 0;
}

int insert_rob_entry(APEX_CPU* cpu, ROB_Entry* new_rob_entry)
{
    int free_entry = cpu->rob.tail;
    cpu->rob.rob_entry[free_entry].free = new_rob_entry->free;
    strcpy(cpu->rob.rob_entry[free_entry].opcode, new_rob_entry->opcode);

    cpu->rob.rob_entry[free_entry].pc = new_rob_entry->pc;
    cpu->rob.rob_entry[free_entry].status = new_rob_entry->status;
    cpu->rob.rob_entry[free_entry].branch_id = new_rob_entry->branch_id;

    cpu->rob.rob_entry[free_entry].arch_rd = new_rob_entry->arch_rd;
    cpu->rob.rob_entry[free_entry].phys_rd = new_rob_entry->phys_rd;

    cpu->rob.rob_entry[free_entry].arch_rs1 = new_rob_entry->arch_rs1;
    cpu->rob.rob_entry[free_entry].phys_rs1 = new_rob_entry->phys_rs1;

    cpu->rob.rob_entry[free_entry].arch_rs2 = new_rob_entry->arch_rs2;
    cpu->rob.rob_entry[free_entry].phys_rs2 = new_rob_entry->phys_rs2;

    cpu->rob.rob_entry[free_entry].arch_rs3 = new_rob_entry->arch_rs3;
    cpu->rob.rob_entry[free_entry].phys_rs3 = new_rob_entry->phys_rs3;

    cpu->rob.rob_entry[free_entry].imm = new_rob_entry->imm;
    cpu->rob.tail++;
    if (cpu->rob.tail == ROB_ENTRIES_NUMBER)
    {
        cpu->rob.tail = 0;
    }

    return free_entry;
}


int save_rob_entry(APEX_CPU* cpu)
{
    if (cpu->rob.rob_entry[cpu->rob.head].status && !cpu->rob.rob_entry[cpu->rob.head].free)
    {

        // Do not commit instructions that do not have physical destination address - BNZ, BZ, STORE
        // for these instructions simly remove entry from ROB
        if (cpu->rob.rob_entry[cpu->rob.head].phys_rd != -1 )
        {
            int deallocate_index = cpu->rob.rob_entry[cpu->rob.head].arch_rd;
            int phys_reg_to_be_commit = cpu->rob.rob_entry[cpu->rob.head].phys_rd;
            // printf("%d\t%d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!ROB_Entry!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", cpu->prf[phys_reg_to_be_commit].value, phys_reg_to_be_commit);
            commit_reg(cpu, deallocate_index, phys_reg_to_be_commit); // commits in R-RAT and deallocates phys reg in prf

            if (strcmp(cpu->rob.rob_entry[cpu->rob.head].opcode, "JAL") == 0)
            {
                int branch_id = cpu->rob.rob_entry[cpu->rob.head].branch_id;
                release_branch_id(cpu, branch_id);
            }
        }

        else
        {
            if (strcmp(cpu->rob.rob_entry[cpu->rob.head].opcode, "BZ") == 0 || strcmp(cpu->rob.rob_entry[cpu->rob.head].opcode, "BNZ") == 0 || strcmp(cpu->rob.rob_entry[cpu->rob.head].opcode, "JUMP") == 0)
            {
                int branch_id = cpu->rob.rob_entry[cpu->rob.head].branch_id;
                release_branch_id(cpu, branch_id);
            }
        }

        if (strcmp(cpu->rob.rob_entry[cpu->rob.head].opcode, "HALT") == 0)
        {
            if (cpu->mem_cycle == 1 && strcmp(cpu->stage[MEM].opcode, "") == 0)
            {
                cpu->rob.rob_entry[cpu->rob.head].free = 1;    // making free ROB entry after commitment
                cpu->rob.head++;
                if (cpu->rob.head == ROB_ENTRIES_NUMBER)
                {
                    cpu->rob.head = 0;
                }
            }
        }
        else
        {
            cpu->rob.rob_entry[cpu->rob.head].free = 1;    // making free ROB entry after commitment
            cpu->rob.head++;
            if (cpu->rob.head == ROB_ENTRIES_NUMBER)
            {
                cpu->rob.head = 0;
            }
            cpu->commitments++;
        }

        if (cpu->fill_in_rob > 2 && check_rob_empty(cpu))
        {
            cpu->simulation_completed = 1;
        }

        return 1;
    }

    if (cpu->fill_in_rob > 2 && check_rob_empty(cpu) && cpu->mem_cycle == 1 && strcmp(cpu->stage[MEM].opcode, "") == 0)
    {
        cpu->simulation_completed = 1;
    }

    return 0;
}


int modify_rob_entry(APEX_CPU* cpu, enum STAGES FU_type)
{
    int rob_entry_id = cpu->stage[FU_type].rob_entry_id;

    printf("\n FU_type=%d\n",FU_type);

    if(FU_type==2 || FU_type==4 || FU_type==5)
    {
      cpu->rob.rob_entry[rob_entry_id].status = 0;
    }
    else if(FU_type==3 || FU_type==6)
    {

      cpu->rob.rob_entry[rob_entry_id].status = 1;//valid mul3(3,6)
    }

    // cpu->rob.rob_entry[rob_entry_id].status = 1;
    return 0;
}


void delete_str_from_rob(APEX_CPU* cpu)
{
    int head = cpu->rob.head;
    cpu->rob.rob_entry[head].free = 1;
    cpu->rob.rob_entry[head].status = 1;
    cpu->rob.head++;
    if (cpu->rob.head == ROB_ENTRIES_NUMBER)
    {
        cpu->rob.head = 0;
    }
}


void display_rob_for_dbg(APEX_CPU* cpu)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Details of ROB State\n");
    for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
    {
        if (!cpu->rob.rob_entry[i].free)
        {
            printf("| ID=%d, FREE=%d, OPCODE=%s, PC=%d, ARCH_RD=%d, PHYS_RD=%d, STATUS=%d, BRCH_ID=%d |\n",i, cpu->rob.rob_entry[i].free, cpu->rob.rob_entry[i].opcode, cpu->rob.rob_entry[i].pc,
            cpu->rob.rob_entry[i].arch_rd, cpu->rob.rob_entry[i].phys_rd, cpu->rob.rob_entry[i].status,cpu->rob.rob_entry[i].branch_id);
        }
    }

    printf("Tail: %d, Head: %d\n\n", cpu->rob.tail, cpu->rob.head);
}


void print_rob(APEX_CPU* cpu)
{
    printf("-------------------------------------- ROB --------------------------------------\n");
    for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
    {
        if (!cpu->rob.rob_entry[i].free || i == cpu->rob.tail)
        {
            printf("| Index = %d | ", i);
            if (i == cpu->rob.tail)
            {
                printf("t |");
            }
            else
            {
                printf("  |");
            }

            if (i == cpu->rob.head)
            {
                printf(" h |");
            }
            else
            {
                printf("   |");
            }

            printf("\t");
            if (!cpu->rob.rob_entry[i].free)
            {
                printf("pc(%d)  ", cpu->rob.rob_entry[i].pc); CPU_Stage* instruction_to_print = malloc(sizeof(*instruction_to_print));
                strcpy(instruction_to_print->opcode, cpu->rob.rob_entry[i].opcode);

                instruction_to_print->arch_rs1 = cpu->rob.rob_entry[i].arch_rs1;
                instruction_to_print->phys_rs1 = cpu->rob.rob_entry[i].phys_rs1;

                instruction_to_print->arch_rs2 = cpu->rob.rob_entry[i].arch_rs2;
                instruction_to_print->phys_rs2 = cpu->rob.rob_entry[i].phys_rs2;

                instruction_to_print->arch_rs3 = cpu->rob.rob_entry[i].arch_rs3;
                instruction_to_print->phys_rs3 = cpu->rob.rob_entry[i].phys_rs3;

                instruction_to_print->arch_rd = cpu->rob.rob_entry[i].arch_rd;
                instruction_to_print->phys_rd = cpu->rob.rob_entry[i].phys_rd;

                instruction_to_print->imm = cpu->rob.rob_entry[i].imm;
                print_instruction(0, instruction_to_print);
                printf("\t|");
            }
        printf("\n");
        }
    }
    printf("---------------------------------------------------------------------------------\n\n");
}


int search_branch_in_rob(APEX_CPU* cpu)
{
    for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
    {
        if (!cpu->rob.rob_entry[i].free && cpu->rob.rob_entry[i].pc == cpu->stage[Int_FU].pc)
        {
            return i;
        }
    }
    return -1;
}

void flush_rob_old_stage(APEX_CPU* cpu)
{
    int rob_id = search_branch_in_rob(cpu);
    int new_tail = rob_id;
    new_tail++;
    if (new_tail == ROB_ENTRIES_NUMBER)
    {
        new_tail = 0;
    }

    int branch_id = cpu->rob.rob_entry[rob_id].branch_id;

    if (branch_id <= cpu->last_branch_id)
    {
        while (branch_id <= cpu->last_branch_id)
        {
            for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
            {
                if (rob_id != i && !cpu->rob.rob_entry[i].free && cpu->rob.rob_entry[i].branch_id == branch_id)
                {
                    cpu->rob.rob_entry[i].free = 1;
                }
            }
            branch_id++;
        }
    }
    else
    {
        while (branch_id < BIS_ENTRIES_NUMBER)
        {
            for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
            {
                if (rob_id != i && !cpu->rob.rob_entry[i].free && cpu->rob.rob_entry[i].branch_id == branch_id)
                {
                    cpu->rob.rob_entry[i].free = 1;
                }
            }
        branch_id++;
        }

        branch_id = 0;
        while (branch_id <= cpu->last_branch_id)
        {
            for (int i = 0; i < ROB_ENTRIES_NUMBER; i++)
            {
                if (rob_id != i && !cpu->rob.rob_entry[i].free && cpu->rob.rob_entry[i].branch_id == branch_id)
                {
                    cpu->rob.rob_entry[i].free = 1;
                }
            }
            branch_id++;
        }
    }

    if (check_rob_empty(cpu))
    {
        cpu->rob.head = 0;
        cpu->rob.tail = 0;
    }
    else
    {
        cpu->rob.tail = new_tail;
    }
}


void flush_rob(APEX_CPU* cpu)
{
    int branch_index_in_rob = search_branch_in_rob(cpu);

    cpu->rob.tail--;
    if (cpu->rob.tail < 0)
    {
        cpu->rob.tail = ROB_ENTRIES_NUMBER;
    }

    if (cpu->rob.tail != branch_index_in_rob)
    {
        if (cpu->rob.tail > branch_index_in_rob)
        {
            while (cpu->rob.tail > branch_index_in_rob)
            {
                if (!cpu->rob.rob_entry[cpu->rob.tail].free)
                {
                    if (cpu->rob.rob_entry[cpu->rob.tail].phys_rd != -1)
                    {
                        int phys_reg_to_deallocate = cpu->rob.rob_entry[cpu->rob.tail].phys_rd;
                        int arch_reg_to_deallocate = cpu->rob.rob_entry[cpu->rob.tail].arch_rd;
                        release_pr(cpu, arch_reg_to_deallocate, phys_reg_to_deallocate);
                    }
                    cpu->rob.rob_entry[cpu->rob.tail].free = 1;
                }
                cpu->rob.tail--;
            }
        }
        else
        {
            while (cpu->rob.tail >= 0)
            {
                if (!cpu->rob.rob_entry[cpu->rob.tail].free)
                {
                    if (cpu->rob.rob_entry[cpu->rob.tail].phys_rd != -1)
                    {
                        int phys_reg_to_deallocate = cpu->rob.rob_entry[cpu->rob.tail].phys_rd;
                        int arch_reg_to_deallocate = cpu->rob.rob_entry[cpu->rob.tail].arch_rd;
                        release_pr(cpu, arch_reg_to_deallocate, phys_reg_to_deallocate);
                    }
                    cpu->rob.rob_entry[cpu->rob.tail].free = 1;
                }
                cpu->rob.tail--;
            }

            cpu->rob.tail = ROB_ENTRIES_NUMBER - 1;
            while (cpu->rob.tail > branch_index_in_rob)
            {
                if (!cpu->rob.rob_entry[cpu->rob.tail].free)
                {
                    if (cpu->rob.rob_entry[cpu->rob.tail].phys_rd != -1)
                    {
                        int phys_reg_to_deallocate = cpu->rob.rob_entry[cpu->rob.tail].phys_rd;
                        int arch_reg_to_deallocate = cpu->rob.rob_entry[cpu->rob.tail].arch_rd;
                        release_pr(cpu, arch_reg_to_deallocate, phys_reg_to_deallocate);
                    }
                    cpu->rob.rob_entry[cpu->rob.tail].free = 1;
                }
                cpu->rob.tail--;
            }
        }
    }
    cpu->rob.tail = branch_index_in_rob + 1;
}
