#include <IR.hpp>
#include <vector>
#include <stack>

using namespace std;
static uint8_t mem[30000];
static int pointer = 0;
void interprete_run(vector<IR> &ir)
{
    stack<int> s;
    int pc = 0, size = ir.size();
    while (pc != size)
    {
        IR &instr = ir[pc];
        ++pc;
        switch (instr.kind)
        {
        case AddVal:
            mem[pointer] += instr.op;
            break;
        case SubVal:
            mem[pointer] -= instr.op;
            break;
        case AddPtr:
            pointer += instr.op;
            break;
        case SubPtr:
            pointer -= instr.op;
            break;
        case Read:
            mem[pointer] = getchar();
            break;
        case Write:
            putchar(mem[pointer]);
            break;
        case Jz:
            if(mem[pointer] == 0)
                pc = instr.op;
            break;
        case Jnz:
            if(mem[pointer] != 0)
                pc = instr.op;
            break;
        default:
            break;
        }
    }
    
}