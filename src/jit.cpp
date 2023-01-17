#include <IR.hpp>
#include <sys/mman.h>
#include <stack>
#include <vector>
#include <assert.h>
#include <string.h>
using namespace std;


static uint8_t mem[30000];
static vector<uint8_t> codes;
static void gen_codes(vector<uint8_t> v)
{
    codes.insert(codes.end(), v.begin(), v.end());
}
static void gen_32bits_num(uint32_t num, int index = -1)
{
    union {
        uint32_t num;
        uint8_t bytes[4];
    } tmp;
    tmp.num = num;
    if(index == - 1)
        for(int i = 0; i < 4; ++i)
            codes.push_back(tmp.bytes[i]);
    else
        for(int i = 0; i < 4; ++i)
            codes[index + i] = tmp.bytes[i];
}
static void gen_64bits_num(uint64_t num)
{
    union {
        uint64_t num;
        uint8_t bytes[8];
    } tmp;
    tmp.num = num;
    for(int i = 0; i < 8; ++i)
        codes.push_back(tmp.bytes[i]);
}

void jit_run(vector<IR> &ir)
{
    stack<int> s;
    
    // Save callee-saved reg and use rbx as the data pointer
    //  push rbx
    //  movabsq  rbx, (uint64_t)mem

    codes.push_back(0x53);

    codes.push_back(0x48);
    codes.push_back(0xbb);
    gen_64bits_num((uint64_t) mem);


    for(auto &i: ir)
    {
        int jump_target;
        int next_instr;
        
        switch (i.kind)
        {
        case AddVal:
            gen_codes({0x80, 0x03, (uint8_t)i.op});
            // add  byte ptr [rbx], i.op
            break;
        case SubVal:
            gen_codes({0x80, 0x2b, (uint8_t)i.op});
            // sub  byte ptr [rbx], i.op
            break;
        case AddPtr:
            gen_codes({0x48, 0x81, 0xC3});
            gen_32bits_num(i.op);
            // add  rbx, i.op
            break;
        case SubPtr:
            gen_codes({0x48, 0x81, 0xEB});
            gen_32bits_num(i.op);
            //  sub  rbx, i.op
            break;
        case Write:
            gen_codes({0x0F, 0xB6, 0x3B});
            gen_codes({0x48, 0xB8});
            gen_64bits_num((uint64_t) putchar);
            gen_codes({0xFF, 0xD0});
            //  movzx  edi, byte ptr [rbx]
            //  movabsq  rax, (uintptr_t)putchar
            //  call   rax
            break;
        case Read:
            gen_codes({0x48, 0xB8});
            gen_64bits_num((uint64_t) getchar);
            gen_codes({0xFF, 0xD0});
            gen_codes({0x88, 0x03});
            //  movabsq  rax, (uintptr_t)getchar
            //  call   rax
            //  mov  byte ptr [rbx], al
            break;
        case Jz:
            gen_codes({0x80, 0x3B, 0x00});
            gen_codes({0x0F, 0x84, 0x00, 0x00, 0x00, 0x00});
            s.push(codes.size());
            // cmp  byte ptr [rbx], 0
            //  je  next ] 
            //  Here offset will be handled when meet the matched ]
            break;
        case Jnz:
            jump_target = s.top();
            next_instr = codes.size() + 9;
            s.pop();
            
            gen_codes({0x80, 0x3B, 0x00});
            gen_codes({0x0F, 0x85});
            gen_32bits_num(jump_target - next_instr);
            gen_32bits_num(next_instr - jump_target, jump_target - 4);
            
            //  cmp  byte [rbx], 0
            //  jne  last [
            break;
        default:
            break;
        }
    }
    // Function epilogue.
    //  pop  rbx
    //  ret

    gen_codes({0x5b});
    gen_codes({0xc3});

#ifdef DEBUG
    FILE *fp = fopen("bfcode.bin", "wb");
    fwrite(codes.data(), codes.size(), 1, fp);
    fclose(fp);
#endif

    size_t size = codes.size();
    void *bf = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    assert(bf != MAP_FAILED);
    memcpy(bf, codes.data(), codes.size());


    assert(mprotect(bf, size, PROT_EXEC | PROT_READ) == 0);

	((void (*)())bf)();
    assert(munmap(bf, size) == 0);
}