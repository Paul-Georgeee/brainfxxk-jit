#ifndef IRH
#define IRH

#include <stdint.h>
#include <vector>
#include <fstream>
enum IRKind
{
    AddVal,
    SubVal,
    AddPtr,
    SubPtr,
    Read,
    Write,
    Jz,
    Jnz
};

struct IR
{
    IRKind kind;
    uint32_t op; //Used for add、sub and jump.
    //In jump IR, take "[++++]+" as example, op of '[' is 6 and
    // op of ']' is 1.(index starts from 0) This information is useful when interprete.
    //It can also useful in jit because for each IR they gen specified number instr but I don't use it.
    IR(IRKind k) {kind = k;}
    IR(IRKind k, uint32_t _op) {kind = k; op = _op;}
};

void gen_IRs(std::vector<IR> &ir, std::ifstream &fin);
void print_IRs(std::vector<IR> &ir);

#endif