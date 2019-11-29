/*  rob_driver.h
 *  State University of New York, Binghamton
 */

int check_rob_entry_free(APEX_CPU* cpu);

int insert_rob_entry(APEX_CPU* cpu, ROB_Entry* new_rob_entry);

int save_rob_entry(APEX_CPU* cpu);

int modify_rob_entry(APEX_CPU* cpu, enum STAGES FU_type);

void delete_str_from_rob(APEX_CPU* cpu);

void flush_rob(APEX_CPU* cpu);

void print_rob(APEX_CPU* cpu);
