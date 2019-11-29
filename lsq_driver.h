/*  lsq_driver.h
 *
 *  State University of New York, Binghamton
 */


int check_lsq_free(APEX_CPU* cpu);


int insert_lsq_entry(APEX_CPU* cpu, LSQ_Entry* new_lsq_entry);


void process_ins_to_MEM(APEX_CPU* cpu);


void modify_lsq_entry(APEX_CPU* cpu, enum STAGES FU_type);


void distribute_result_to_lsq(APEX_CPU* cpu, enum STAGES FU_type);


void flush_lsq(APEX_CPU* cpu, int branch_id);


void lsq_transition(APEX_CPU* cpu);


void print_lsq(APEX_CPU* cpu);
