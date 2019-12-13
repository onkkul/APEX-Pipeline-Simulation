MOVC R1, #4
MOVC R2, #5
MUL R2, R1, R2 /* checks if you can handle a case where one src and dest arch. regs are the same)
ADD R3, R1, R2 /* check timing to see if back-to-back execution is supported */
MOVC R4, 4000
STR R1, R4, R2
HALT