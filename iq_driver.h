/* iq_driver.h
 *  State University of New York, Binghamton
 */

int check_iq_entry_free(APEX_CPU* cpu);

int insert_iq_entry(APEX_CPU* cpu, ISSUE_QUEUE_Entry* new_iq_entry);

int fetch_ins_for_FUs(APEX_CPU* cpu, enum STAGES FU_Type);

int update_counters(APEX_CPU* cpu);

int distribute_result_to_iq(APEX_CPU* cpu, enum STAGES FU_type);

void flush_iq(APEX_CPU* cpu, int branch_id);

int iq_transition(APEX_CPU* cpu);

void print_iq(APEX_CPU* cpu);
