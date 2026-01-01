#ifndef BRUNTIME_BC_H
#define BRUNTIME_BC_H

int BC_Argc(void);
char** BC_Argv(void);

void BC_Initialize(int argc, char** argv);
void BC_Deinitialize(void);

#endif //BRUNTIME_BC_H